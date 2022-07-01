#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <isa.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"

static void error(prs_state* s, const char* string) {
  print_error_and_exit(s->tk->filename, s->tk->cur_line, s->tk->cur_char, string);
}

static void consume(prs_state* s) {
  s->current_token++;
  s->tk = &s->tokens[s->current_token];
}

static void write(prs_state* s, void* data, size_t size) {
  if (s->pass == 1) {
    if (!fwrite(data, size, 1, s->file)) {
      perror("some-as");
      exit(1);
    }
  }
  
  s->current_address += size;
}

static uint32_t get_number_or_label(prs_state* s) {
  uint32_t value;
  
  switch (s->tk->type) {
    case TOKEN_SYMBOL:
      if (s->pass == 0) {
        value = 0;
      } else if (s->pass == 1) {
        sc_entry* entry;
        
        if (s->local_table) {
          entry = sc_get(s->local_table, s->tk->data.string);
        } else {
          entry = NULL;
        }
        
        if (!entry) {
          entry = sc_get(s->global_table, s->tk->data.string);
          
          if (!entry) {
            error(s, "Unknown label");
          }
        }
        value = entry->value;
      }
    break;
    case TOKEN_NUMBER:
      value = s->tk->data.integer;
    break;
    default:
      error(s, "Expected integer or label");
    break;
  }
  
  consume(s);
  return value;
}

static uint8_t get_inst(prs_state* s) {
  // ew
  #define F(n, a, c) \
    if (!strcmp(s->tk->data.string, #n)) { \
      consume(s); \
      return c; \
    } else
  ENUMERATE_INSTS(F)
  #undef F
  {
    error(s, "Invalid instruction");
  }
}

static int num_of_args(uint8_t inst) {
  switch (inst) {
    #define F(n, a, c) \
      case c: \
        return a;
    ENUMERATE_INSTS(F)
    #undef F
  }
}

static void parse_acs(prs_state* s, bool allow_offset);

static void parse_acs_ap(prs_state* s, uint8_t type, bool allow_offset) {
  uint8_t acs = 0;
  
  acs |= type;
  
  switch (s->tk->data.string[1]) {
    case 'S':
      acs |= ACS_WIDTH_S;
    break;
    case 'D':
      acs |= ACS_WIDTH_D;
    break;
    case 'Q':
      acs |= ACS_WIDTH_Q;
    break;
    default:
      error(s, "Invalid access specifier width");
    break;
  }
  
  uint32_t address;
  bool get_offset = false;
  
  switch (s->tk->data.string[2]) {
    case 'N':
      acs |= ACS_OFFSET_N;
    break;
    case 'B':
      acs |= ACS_OFFSET_B;
      get_offset = true;
    break;
    case 'A':
      if (type == ACS_TYPE_P) {
        acs |= ACS_OFFSET_A;
        get_offset = true;
      } else {
        error(s, "'After' offset can only be used by 'Pointer' type");
      }
    break;
    default:
      error(s, "Invalid access specifier offset type");
    break;
  }
  
  write(s, &acs, 1);
  consume(s);
  
  address = get_number_or_label(s);
  write(s, &address, 4);
  
  if (get_offset) {
    if (allow_offset) {
      parse_acs(s, false);
    } else {
      error(s, "Nested offsets are not allowed");
    }
  }
}

static void parse_acs(prs_state* s, bool allow_offset) {
  if (s->tk->type != TOKEN_SYMBOL) {
    error(s, "Expected access specifier");
  }
  if (strlen(s->tk->data.string) > 3) {
    error(s, "Invalid access specifier length");
  }
  
  uint8_t acs = 0;
  
  switch (s->tk->data.string[0]) {
    case 'I':
      acs |= ACS_TYPE_I;
      
      size_t width;
      
      switch (s->tk->data.string[1]) {
        case 'S':
          acs |= ACS_WIDTH_S;
          width = 1;
        break;
        case 'D':
          acs |= ACS_WIDTH_D;
          width = 2;
        break;
        case 'Q':
          acs |= ACS_WIDTH_Q;
          width = 4;
        break;
        default:
          error(s, "Invalid access specifier width");
        break;
      }
      write(s, &acs, 1);
      consume(s);
      
      uint32_t integer = get_number_or_label(s);
      write(s, &integer, width);
    break;
    case 'R':
      acs |= ACS_TYPE_R;
      
      switch (s->tk->data.string[1]) {
        case 'A':
          acs |= ACS_REG_A;
        break;
        case 'B':
          acs |= ACS_REG_B;
        break;
        case 'C':
          acs |= ACS_REG_C;
        break;
        case 'S':
          acs |= ACS_REG_S;
        break;
        default:
          error(s, "Invalid access specifier register");
        break;
      }
      
      switch (s->tk->data.string[2]) {
        case 'S':
          acs |= ACS_WIDTH_S;
        break;
        case 'D':
          acs |= ACS_WIDTH_D;
        break;
        case 'Q':
          acs |= ACS_WIDTH_Q;
        break;
        default:
          error(s, "Invalid access specifier width");
        break;
      }
      
      write(s, &acs, 1);
      consume(s);
    break;
    case 'A':
      parse_acs_ap(s, ACS_TYPE_A, allow_offset);
    break;
    case 'P':
      parse_acs_ap(s, ACS_TYPE_P, allow_offset);
    break;
    default:
      error(s, "Invalid access specifier type");
    break;
  }
}

void parse(prs_state* s) {
  while (true) {
    switch (s->tk->type) {
      case TOKEN_SYMBOL:
        uint8_t inst = get_inst(s);
        int args = num_of_args(inst);
        
        write(s, &inst, 1);
        
        for (int i = 0; i < args; i++) {
          parse_acs(s, true);
        }
        
        if (s->tk->type != TOKEN_STATEMENT_END) {
          error(s, "Too many arguments for instruction");
        }
        consume(s);
      break;
      case TOKEN_LABEL:
        sc_table* local_table;
        
        if (s->pass == 0) {
          local_table = sc_alloc();
          sc_add(s->global_table, s->tk->data.string, s->current_address, local_table);
        } else if (s->pass == 1) {
          local_table = sc_get(s->global_table, s->tk->data.string)->local_table;
        }
        
        consume(s);
        
        s->local_table = local_table;
        
        if (s->tk->type != TOKEN_STATEMENT_END) {
          error(s, "Labels must be followed by a newline");
        }
        consume(s);
      break;
      case TOKEN_LOCAL_LABEL:
        if (s->pass == 0) {
          if (s->local_table) {
            sc_add(s->local_table, s->tk->data.string, s->current_address, NULL);
          } else {
            error(s, "Local labels must be below a global label");
          }
        }
        
        consume(s);
        
        if (s->tk->type != TOKEN_STATEMENT_END) {
          error(s, "Labels must be followed by a newline");
        }
        consume(s);
      break;
      case TOKEN_DIRECTIVE:
        if (!strcmp("data", s->tk->data.string)) {
          consume(s);
          
          while (s->tk->type != TOKEN_STATEMENT_END) {
            switch (s->tk->type) {
              case TOKEN_SYMBOL:
                uint32_t integer = get_number_or_label(s);
                write(s, &integer, 4);
              break;
              case TOKEN_NUMBER:
                write(s, &s->tk->data.integer, 1);
                consume(s);
              break;
              case TOKEN_STRING:
                write(s, s->tk->data.string, strlen(s->tk->data.string));
                consume(s);
              break;
              default:
                error(s, "Expected integer or string");
              break;
            }
          }
          consume(s);
        } else if (!strcmp("at", s->tk->data.string)) {
          consume(s);
          
          if (s->tk->type != TOKEN_NUMBER) {
            error(s, "Expected integer");
          }
          s->current_address = s->tk->data.integer;
          consume(s);
          
          if (s->tk->type != TOKEN_STATEMENT_END) {
            error(s, "Too many arguments for .at");
          }
          consume(s);
        } else {
          error(s, "Invalid directive");
        }
      break;
      case TOKEN_EOF:
        goto break_loop;
      break;
      default:
        error(s, "Expected instruction, label definition, or directive.");
      break;
    }
  }
  break_loop:
}
