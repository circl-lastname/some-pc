#ifdef DEBUG
  #include <inttypes.h>
  #include <stdbool.h>
  #include <stdio.h>
  
  #include "lexer.h"
  
  void print_tokens(lex_state* s) {
    for (long i = 0; i < s->tokens_amount; i++) {
      char* token_name;
      
      switch (s->tokens[i].type) {
        #define F(t) \
          case t: \
            token_name = #t; \
          break;
        ENUMERATE_TOKENS(F)
        #undef F
      }
      
      switch (s->tokens[i].data_type) {
        case TYPE_NONE:
          printf("%s\n", token_name);
        break;
        case TYPE_STRING:
          printf("%s\t\t%s\n", token_name, s->tokens[i].data.string);
        break;
        case TYPE_NUMBER:
          printf("%s\t\t%"PRIu32"\n", token_name, s->tokens[i].data.number);
        break;
      }
    }
  }
#endif
