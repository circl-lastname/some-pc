typedef struct {
  char* string;
  uint32_t value;
} sc_entry;

typedef struct {
  long amount;
  size_t size;
  sc_entry* entries;
} sc_table;

void* try(void* input);

uint32_t u32_pow(uint32_t a, uint32_t b);

sc_table* sc_alloc();
void sc_add(sc_table* table, char* string, uint32_t value);
uint32_t* sc_get(sc_table* table, char* string);
void sc_free(sc_table* table);

void print_error_and_exit(FILE* file, const char* filename, long cur_line, long cur_char, const char* string);
