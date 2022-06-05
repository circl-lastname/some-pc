#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "lexer.h"

static void error(lex_state* s, const char* string) {
  print_error_and_exit(s->file, s->filename, s->cur_line, s->cur_char, string);
}

static void set_token_cur(lex_state* s) {
  s->token_cur_line = s->cur_line;
  s->token_cur_char = s->cur_char;
}

static void put_token(lex_state* s, lex_type type, lex_data_type data_type, lex_data* data) {
  lex_token* token = &s->tokens[s->tokens_amount];
  
  token->type = type;
  token->data_type = data_type;
  token->data = *data;
  token->cur_line = s->token_cur_line;
  token->cur_char = s->token_cur_char;
  
  s->tokens_amount++;
  
  if (s->tokens_amount >= s->tokens_size) {
    s->tokens_size *= 2;
    s->tokens = try(realloc(s->tokens, s->tokens_size*sizeof(lex_token)));
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
  
  consume(s);
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
              error(s, "Unexpected character in number");
            break;
          }
        } else {
          error(s, "Unexpected character in number");
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
  
  set_token_cur(s);
  put_token(s, TOKEN_EOF, TYPE_NONE, &data);
}
