#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"
#include "debug.h"

void main(int argc, char** argv) {
  if (argc != 3) {
    puts("usage: some-as <input> <output>");
    exit(1);
  }
  
  FILE* file_in = try(fopen(argv[1], "rb"));
  FILE* file_out = try(fopen(argv[2], "wb"));
  
  lex_state lexs;
  
  lexs.file = file_in;
  lexs.filename = argv[1];
  lexs.ch = fgetc(file_in);
  lexs.after_newline = false;
  lexs.statement_end_status = 1;
  lexs.cur_line = 0;
  lexs.cur_char = 0;
  lexs.token_cur_line = 0;
  lexs.token_cur_char = 0;
  lexs.counter_disabled = false;
  lexs.tokens_amount = 0;
  lexs.tokens_size = 256;
  lexs.tokens = try(malloc(256*sizeof(lex_token)));
  
  lex(&lexs);
  
  #ifdef DEBUG
    print_tokens(&lexs);
  #endif
  
  prs_state prss;
  
  prss.file = file_out;
  prss.file_in = file_in;
  prss.filename_in = argv[1];
  prss.tk = &lexs.tokens[0];
  prss.current_token = 0;
  prss.current_address = 0x1000;
  prss.global_table = sc_alloc();
  prss.local_table = sc_alloc();
  prss.tokens_amount = lexs.tokens_amount;
  prss.tokens = lexs.tokens;
  
  parse(&prss);
  
  fclose(file_in);
  fclose(file_out);
  
  for (long i = 0; i < lexs.tokens_amount; i++) {
    if (lexs.tokens[i].data_type == TYPE_STRING) {
      free(lexs.tokens[i].data.string);
    }
  }
  
  free(lexs.tokens);
  
  sc_free(prss.global_table);
  sc_free(prss.local_table);
}
