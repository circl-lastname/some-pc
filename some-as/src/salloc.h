// libsalloc - https://github.com/circl-lastname/libsalloc
// Licensed under MIT, see LICENSE file in above repository

#ifndef SALLOC_H
#define SALLOC_H
typedef struct {
  long allocs_count;
  long allocs_capacity;
  void** allocs;
} ascope_t;

ascope_t* salloc();
int strack(ascope_t* scope, void* alloc);
void* smalloc(ascope_t* scope, size_t size);
void* srealloc(ascope_t* scope, void* alloc, size_t size);
void sfree(ascope_t* scope);
#endif
