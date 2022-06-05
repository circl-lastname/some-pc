typedef struct {
  FILE* file;
  
  FILE* file_in;
  const char* filename_in;
  
  lex_token* tk;
  long current_token;
  
  int pass;
  
  uint32_t current_address;
  sc_table* global_table;
  sc_table* local_table;
  
  long tokens_amount;
  lex_token* tokens;
} prs_state;

void parse(prs_state* s);
