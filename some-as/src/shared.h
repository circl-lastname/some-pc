typedef struct sc_table sc_table;

typedef struct {
  char* string;
  uint32_t value;
  sc_table* local_table;
} sc_entry;

struct sc_table {
  long amount;
  size_t size;
  sc_entry* entries;
};

void* try(void* input);

uint32_t u32_pow(uint32_t a, uint32_t b);

sc_table* sc_alloc();
void sc_add(sc_table* table, char* string, uint32_t value, sc_table* local_table);
sc_entry* sc_get(sc_table* table, char* string);
void sc_free(sc_table* table);

void print_error_and_exit(const char* filename, long cur_line, long cur_char, const char* string);
