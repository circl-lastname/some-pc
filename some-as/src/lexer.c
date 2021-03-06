#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "salloc.h"
#include "shared.h"
#include "lexer.h"

static void error(lex_state* s, const char* string) {
  print_error_and_exit(s->filename, s->cur_line, s->cur_char, string);
}

static void error_tk(lex_state* s, const char* string) {
  print_error_and_exit(s->filename, s->token_cur_line, s->token_cur_char, string);
}

static void set_token_cur(lex_state* s) {
  s->token_cur_line = s->cur_line;
  s->token_cur_char = s->cur_char;
}

static void lex_file(lex_state* s, char* filename) {
  FILE* file_in = fopen(filename, "rb");
  if (!file_in) {
    error_tk(s, strerror(errno));
  }
  
  lex_state lexs;
  
  lexs.file = file_in;
  lexs.filename = filename;
  lexs.ch = fgetc(file_in);
  lexs.after_newline = false;
  lexs.statement_end_status = 1;
  lexs.include_status = 0;
  lexs.put_eof = false;
  lexs.cur_line = 0;
  lexs.cur_char = 0;
  lexs.token_cur_line = 0;
  lexs.token_cur_char = 0;
  lexs.counter_disabled = false;
  lexs.tokens_scope = s->tokens_scope;
  lexs.tokens_amount = s->tokens_amount;
  lexs.tokens_size = s->tokens_size;
  lexs.tokens = s->tokens;
  
  lex(&lexs);
  
  fclose(file_in);
  
  s->tokens_amount = lexs.tokens_amount;
  s->tokens_size = lexs.tokens_size;
  s->tokens = lexs.tokens;
}

static void put_token(lex_state* s, lex_type type, lex_data_type data_type, lex_data* data) {
  // this parses in place an include directive, allowing it to be easily appended to the token array
  switch (s->include_status) {
    case 0:
      if (type == TOKEN_DIRECTIVE) {
        if (!strcmp(data->string, "include")) {
          free(data->string);
          
          s->include_status = 1;
          return;
        }
      }
    break;
    case 1:
      if (type == TOKEN_STRING) {
        strack(s->tokens_scope, data->string);
        lex_file(s, data->string);
        
        s->include_status = 2;
        return;
      } else {
        error_tk(s, "Expected string");
      }
    break;
    case 2:
      if (type == TOKEN_STATEMENT_END) {
        s->include_status = 0;
        return;
      } else {
        error_tk(s, "Too many arguments for .include");
      }
    break;
  }
  
  if (data_type == TYPE_STRING) {
    strack(s->tokens_scope, data->string);
  }
  
  lex_token* token = &s->tokens[s->tokens_amount];
  
  token->type = type;
  token->data_type = data_type;
  token->data = *data;
  token->filename = s->filename;
  token->cur_line = s->token_cur_line;
  token->cur_char = s->token_cur_char;
  
  s->tokens_amount++;
  
  if (s->tokens_amount >= s->tokens_size) {
    s->tokens_size *= 2;
    s->tokens = try(srealloc(s->tokens_scope, s->tokens, s->tokens_size*sizeof(lex_token)));
  }
}

static void consume(lex_state* s) {
  s->ch = fgetc(s->file);
  
  // this is a big if block to run every character, but
  // - it can't run during printing an error
  // - it needs to mark the next character after a newline for reset
  // - it can't update on an EOF, since it can result in unhelpful errors
  if (!s->counter_disabled) {
    if (s->after_newline) {
      if (s->ch != EOF) {
        s->after_newline = false;
        s->cur_line++;
        s->cur_char = 0;
      }
    } else {
      s->cur_char++;
    }
    
    switch (s->ch) {
      case '\n':
        s->after_newline = true;
      break;
      case EOF:
        s->counter_disabled = true;
      break;
    }
  }
}

static void skip_space(lex_state* s) {
  while (isspace(s->ch) && s->ch != '\n') {
    consume(s);
  }
}

static void skip_until_newline(lex_state* s) {
  while (true) {
    switch (s->ch) {
      case '\n':
        goto break_loop;
      break;
      case EOF:
        goto break_loop;
      break;
    }
    
    consume(s);
  }
  break_loop:
}

static char* get_until_disallowed(lex_state* s) {
  char* string = try(malloc(256));
  size_t string_size = 256;
  
  size_t i = 0;
  while (true) {
    if (isspace(s->ch) || s->ch == ':') {
      break;
    }
    
    string[i] = s->ch;
    i++;
    consume(s);
    
    if (i >= string_size) {
      string_size *= 2;
      string = try(realloc(string, string_size));
    }
  }
  
  string[i] = '\0';
  
  return string;
}

static char* get_string(lex_state* s) {
  char* string = try(malloc(256));
  size_t string_size = 256;
  
  size_t i = 0;
  while (true) {
    switch (s->ch) {
      case '"':
        goto break_loop;
      break;
      case '\\':
        consume(s);
        
        switch (s->ch) {
          case '"':
            string[i] = '"';
          break;
          case '\\':
            string[i] = '\\';
          break;
          default:
            error(s, "Unknown escape character");
          break;
        }
        
      break;
      case EOF:
        error(s, "Unexpected EOF");
      break;
      default:
        string[i] = s->ch;
      break;
    }
    
    i++;
    consume(s);
    
    if (i >= string_size) {
      string_size *= 2;
      string = try(realloc(string, string_size));
    }
  }
  break_loop:
  
  consume(s);
  string[i] = '\0';
  
  return string;
}

static uint32_t parse_integer(lex_state* s, char* string, bool hex) {
  size_t length = strlen(string);
  
  uint32_t divider = (hex ? 16 : 10);
  uint32_t place = u32_pow(divider, length-1);
  
  uint32_t result = 0;
  
  for (size_t i = 0; i < length; i++) {
    switch (string[i]) {
      case '0':
      break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        result += (string[i] - 0x30) * place;
      break;
      default:
        if (hex) {
          switch (string[i]) {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
              result += (string[i] - 0x60 + 9) * place;
            break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
              result += (string[i] - 0x40 + 9) * place;
            break;
            default:
              error_tk(s, "Unexpected character in number");
            break;
          }
        } else {
          error_tk(s, "Unexpected character in number");
        }
      break;
    }
    
    place /= divider;
  }
  
  return result;
}

void lex(lex_state* s) {
  switch (s->ch) {
    case '\n':
      s->after_newline = true;
    break;
    case EOF:
      s->counter_disabled = true;
    break;
  }
  
  lex_data data;
  
  while (true) {
    skip_space(s);
    
    char* temp_string;
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
        s->statement_end_status++;
      break;
      case '.':
        set_token_cur(s);
        consume(s);
        
        data.string = get_until_disallowed(s);
        
        skip_space(s);
        if (s->ch == ':') {
          consume(s);
          put_token(s, TOKEN_LOCAL_LABEL, TYPE_STRING, &data);
        } else if (s->statement_end_status) {
          put_token(s, TOKEN_DIRECTIVE, TYPE_STRING, &data);
        } else {
          put_token(s, TOKEN_SYMBOL, TYPE_STRING, &data);
        }
      break;
      case '0':
        set_token_cur(s);
        
        temp_string = get_until_disallowed(s);
        
        if (temp_string[0] == '0' && temp_string[1] == 'x') {
          data.integer = parse_integer(s, &temp_string[2], true);
        } else {
          data.integer = parse_integer(s, temp_string, false);
        }
        
        put_token(s, TOKEN_NUMBER, TYPE_INTEGER, &data);
        
        free(temp_string);
      break;
      case '"':
        set_token_cur(s);
        consume(s);
        
        data.string = get_string(s);
        put_token(s, TOKEN_STRING, TYPE_STRING, &data);
      break;
      case '\n':
        set_token_cur(s);
        consume(s);
        
        if (s->statement_end_status == 0) {
          put_token(s, TOKEN_STATEMENT_END, TYPE_NONE, &data);
        }
        s->statement_end_status = 2;
      break;
      case EOF:
        goto break_loop;
      break;
      default:
        set_token_cur(s);
        temp_string = get_until_disallowed(s);
        
        skip_space(s);
        if (s->ch == ':') {
          consume(s);
          data.string = temp_string;
          put_token(s, TOKEN_LABEL, TYPE_STRING, &data);
        } else if (isdigit(temp_string[0])) {
          data.integer = parse_integer(s, temp_string, false);
          put_token(s, TOKEN_NUMBER, TYPE_INTEGER, &data);
          
          free(temp_string);
        } else {
          data.string = temp_string;
          put_token(s, TOKEN_SYMBOL, TYPE_STRING, &data);
        }
      break;
    }
    
    if (s->statement_end_status != 0) {
      s->statement_end_status--;
    }
  }
  break_loop:
  
  if (s->include_status) {
    error(s, "Unexpected EOF");
  }
  
  if (s->put_eof) {
    set_token_cur(s);
    put_token(s, TOKEN_EOF, TYPE_NONE, &data);
  }
}
