#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

void* try(void* input) {
  if (input == NULL) {
    perror("some-as");
    exit(1);
  } else {
    return input;
  }
}

uint32_t u32_pow(uint32_t a, uint32_t b) {
  uint32_t result = 1;
  for (uint32_t i = 0; i < b; i++) {
    result *= a;
  }
  return result;
}

sc_table* sc_alloc() {
  sc_table* table = try(malloc(sizeof(sc_table)));
  
  table->amount = 0;
  table->size = 256;
  table->entries = try(malloc(256*sizeof(sc_entry)));
  
  return table;
}

void sc_add(sc_table* table, char* string, uint32_t value, sc_table* local_table) {
  sc_entry* entry = &table->entries[table->amount];
  
  entry->string = try(malloc(strlen(string)+1));
  strcpy(entry->string, string);
  entry->value = value;
  entry->local_table = local_table;
  
  table->amount++;
  
  if (table->amount >= table->size) {
    table->size *= 2;
    table->entries = try(realloc(table->entries, table->size*sizeof(sc_entry)));
  }
}

sc_entry* sc_get(sc_table* table, char* string) {
  for (long i = 0; i < table->amount; i++) {
    // ew
    if (!strcmp(string, table->entries[i].string)) {
      return &table->entries[i];
    }
  }
  return NULL;
}

void sc_free(sc_table* table) {
  for (long i = 0; i < table->amount; i++) {
    free(table->entries[i].string);
    if (table->entries[i].local_table) {
      sc_free(table->entries[i].local_table);
    }
  }
  free(table->entries);
  free(table);
}

// this is is a mini-lexer, ripping out some code from it to iterate over the file
// there may be a more libc-this way to do this
void print_error_and_exit(const char* filename, long cur_line, long cur_char, const char* string) {
  FILE* file = try(fopen(filename, "rb"));
  
  char ch = fgetc(file);
  
  for (int i = 0; i < cur_line; i++) {
    while (true) {
      switch (ch) {
        case '\n':
          goto break_loop_1;
        break;
        case '\r':
          ch = fgetc(file);
          if (ch == '\n') {
            goto break_loop_1;
          }
        break;
        case EOF:
          goto break_loop_1;
        break;
      }
      
      ch = fgetc(file);
    }
    break_loop_1:
    
    ch = fgetc(file);
  }
  
  while (true) {
    switch (ch) {
      case '\n':
        goto break_loop_2;
      break;
      case '\r':
        ch = fgetc(file);
        if (ch == '\n') {
          goto break_loop_2;
        }
      break;
      case EOF:
        goto break_loop_2;
      break;
    }
    
    fputc(ch, stderr);
    ch = fgetc(file);
  }
  break_loop_2:
  fputc('\n', stderr);
  
  char* char_arrow = try(malloc(cur_char+2));
  
  for (int i = 0; i < cur_char; i++) {
    char_arrow[i] = ' ';
  }
  char_arrow[cur_char] = '^';
  char_arrow[cur_char+1] = '\0';
  
  fprintf(stderr, "%s\n", char_arrow);
  free(char_arrow);
  
  fprintf(stderr, "%s:%li:%li  %s\n", filename, cur_line+1, cur_char+1, string);
  
  fclose(file);
  exit(1);
}
