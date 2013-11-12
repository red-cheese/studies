#include <malloc.h>
#include <stdio.h>
#include "alloc.h"

void* buffer;
size_t N;
size_t chunk_size;
size_t u_mem_count;

const size_t BS = sizeof(void*);

void* init(size_t size) {
  N = size;
  u_mem_count = 0;
  chunk_size = (((size % BS == 0) ? 0 : 1) + size / BS ) * BS;
  buffer = malloc(N);
  void** t = (void**) ((void*) buffer + chunk_size - BS);
  *t = buffer;
  return buffer;
}

void* my_alloc(size_t size) {
  void** ptr = buffer;
  int spaceOk = 0;
  void** tmp;
  do {
    if (*ptr == NULL) {
      // Try to find space.
      tmp = ptr;
      int fail = 0;
      while (!spaceOk && !fail) {
	tmp++;
	if (*tmp == NULL) {
	  if ((void*) tmp - (void*) ptr >= size) {
	    spaceOk = 1;
	    *ptr = tmp;
	    u_mem_count += (((size % BS == 0) ? 0 : 1) + size / BS) * BS;
	  }
	} else {
	  ptr = tmp;
	  fail = 1;
	}
      }
    } else {
      ptr = (*ptr) + BS;
    }

  } while (*ptr != buffer && spaceOk == 0);
  return (*ptr == buffer) ? NULL : ++ptr;
}

void my_delete(void* ptr) {
  ptr -= BS;
  void* end = *((void**) ptr);

  size_t size = end - ptr;
  u_mem_count -= size;

  while (ptr != end) {
    void** tmp = (void**) ptr;
    ptr += BS;
    *tmp = NULL;
  }
}

void map() {
  void** ptr = buffer;
  while (*ptr != buffer) {
    if (*ptr != NULL) {
      void** end = *ptr;
      printf("mmmm");
      ptr++;
      while (ptr <= end) {
	printf("uuuu");
	ptr++;
      }
    } else {
      printf("ffff");
      ptr++;
    }
  }
  printf("mmmm\n");
}

void info(size_t* n1, size_t* n2, size_t* n3, size_t* n4) {
  size_t u_count;

  size_t f_mem_count;
  size_t f_max_mem;

  size_t max_seq = 0;
  size_t seq = 0;

  void** ptr = buffer;
  while (*ptr != buffer) {
    if (*ptr != NULL) {
      seq = 0;
      void** end = *ptr;
      ptr++;
      while (ptr <= end) {
	u_count++;
	ptr++;
      }
    } else {
      f_mem_count += BS;
      seq++;
      ptr++;
      max_seq = (seq > max_seq) ? seq : max_seq;
    }
  }
  *n1 = u_count;
  *n2 = u_mem_count;
  *n3 = (f_mem_count < BS) ? 0 : f_mem_count - BS;
  *n4 = (max_seq * BS < BS) ? 0 : max_seq * BS - BS;
  return;
}
