#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "salloc.h"
#include "shared.h"
#include "lexer.h"
#include "parser.h"

void export_labels(prs_state* s, FILE* file) {
  for (long i = 0; i < s->global_table->amount; i++) {
    fprintf(file, ".at 0x%"PRIx32"\n%s:\n\n", s->global_table->entries[i].value, s->global_table->entries[i].string);
  }
}
