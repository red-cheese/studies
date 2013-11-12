#include <stdio.h>
#include "alloc.h"

int main() {
  int bytes;
  scanf("%d", &bytes);
  void* begin = init(bytes);
  char arg0[5];
  size_t arg1;

  size_t n2;
  size_t n3;
  size_t n4;
  size_t n1;

  while (!feof(stdin)) {
    scanf("%s", arg0);
    if (feof(stdin)) {
      break;
    }
    switch(arg0[0]) {
    case 'A':
      scanf("%zu", &arg1);
      void* t = my_alloc(arg1);
      if (t == NULL) {
	puts("-");
      } else {
	printf("+ %zu\n", t - begin);
      }
      break;
    case 'F':
      scanf("%zu", &arg1);
      my_delete(begin + arg1);
      puts("+");
      break;
    case 'I':
      info(&n1, &n2, &n3, &n4);
      printf("%zu %zu %zu %zu\n", n1, n2, n3, n4);
      break;
    case 'M':
      map();
      break;
    }
  }

  return 0;
}
