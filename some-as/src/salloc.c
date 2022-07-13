// libsalloc - https://github.com/circl-lastname/libsalloc
// Licensed under MIT, see LICENSE file in above repository

#include <stdlib.h>

#include "salloc.h"

ascope_t* salloc() {
  ascope_t* scope = malloc(sizeof(ascope_t));
  if (!scope) {
    return NULL;
  }
  
  scope->allocs_count = 0;
  scope->allocs_capacity = 64;
  scope->allocs = malloc(sizeof(void*)*64);
  if (!scope->allocs) {
    return NULL;
  }
  
  return scope;
}

int strack(ascope_t* scope, void* alloc) {
  scope->allocs[scope->allocs_count] = alloc;
  scope->allocs_count++;
  
  if (scope->allocs_count == scope->allocs_capacity) {
    scope->allocs_capacity *= 2;
    scope->allocs = realloc(scope->allocs, sizeof(void*)*scope->allocs_capacity);
    if (!scope->allocs) {
      return 0;
    }
  }
  
  return 1;
}

void* smalloc(ascope_t* scope, size_t size) {
  void* alloc = malloc(size);
  
  if (!strack(scope, alloc)) {
    return NULL;
  }
  
  return alloc;
}

void* srealloc(ascope_t* scope, void* alloc, size_t size) {
  void* new_alloc = realloc(alloc, size);
  
  long i = 0;
  for (; scope->allocs[i] != alloc; i++) {
    if (i == scope->allocs_count-1) {
      return NULL;
    }
  }
  scope->allocs[i] = new_alloc;
  
  return new_alloc;
}

void sfree(ascope_t* scope) {
  for (long i = 0; i < scope->allocs_count; i++) {
    free(scope->allocs[i]);
  }
  
  free(scope->allocs);
  free(scope);
}
