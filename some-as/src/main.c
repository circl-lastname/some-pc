#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"
#include "export.h"
#include "debug.h"

void main(int argc, char** argv) {
  if (!(argc == 3 || argc == 4)) {
    puts("usage: some-as <input-asm> <output-binary> [label-export]");
    exit(1);
  }
  
  FILE* file_in = try(fopen(argv[1], "rb"));
  FILE* file_out = try(fopen(argv[2], "wb"));
  
  FILE* file_export;
  if (argv[3]) {
    file_export = try(fopen(argv[3], "wb"));
  }
  
  lex_state lexs;
  
  lexs.file = file_in;
  lexs.filename = argv[1];
  lexs.ch = fgetc(file_in);
  lexs.after_newline = false;
  lexs.statement_end_status = 1;
  lexs.include_status = 0;
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
  prss.pass = 0;
  prss.current_address = 0x1000;
  prss.global_table = sc_alloc();
  prss.local_table = NULL;
  prss.tokens_amount = lexs.tokens_amount;
  prss.tokens = lexs.tokens;
  
  // pass 0
  parse(&prss);
  
  prss.tk = &lexs.tokens[0];
  prss.current_token = 0;
  prss.pass = 1;
  prss.current_address = 0x1000;
  prss.local_table = NULL;
  
  // pass 1
  parse(&prss);
  
  if (argv[3]) {
    export_labels(&prss, file_export);
  }
  
  fclose(file_in);
  fclose(file_out);
  if (argv[3]) {
    fclose(file_export);
  }
  
  for (long i = 0; i < lexs.tokens_amount; i++) {
    if (lexs.tokens[i].data_type == TYPE_STRING) {
      free(lexs.tokens[i].data.string);
    }
  }
  
  free(lexs.tokens);
  
  sc_free(prss.global_table);
}
