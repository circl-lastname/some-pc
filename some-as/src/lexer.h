#define ENUMERATE_TOKENS(f) \
  f(TOKEN_SYMBOL) \
  f(TOKEN_NUMBER) \
  \
  f(TOKEN_LABEL) \
  f(TOKEN_LOCAL_LABEL) \
  f(TOKEN_DIRECTIVE) \
  f(TOKEN_STRING) \
  \
  f(TOKEN_STATEMENT_END) \
  \
  f(TOKEN_EOF)

typedef enum {
  #define F(t) t,
  ENUMERATE_TOKENS(F)
  #undef F
} lex_type;

typedef enum {
  TYPE_NONE,
  TYPE_STRING,
  TYPE_NUMBER,
} lex_data_type;

typedef union {
  char* string;
  uint32_t number;
} lex_data;

typedef struct {
  lex_type type;
  
  lex_data_type data_type;
  lex_data data;
  
  long cur_line;
  long cur_char;
} lex_token;

typedef struct {
  FILE* file;
  const char* filename;
  
  char ch;
  bool after_newline;
  
  int statement_end_status;
  
  long cur_line;
  long cur_char;
  long token_cur_line;
  long token_cur_char;
  bool counter_disabled;
  
  long tokens_amount;
  size_t tokens_size;
  lex_token* tokens;
} lex_state;

void lex(lex_state* s);
