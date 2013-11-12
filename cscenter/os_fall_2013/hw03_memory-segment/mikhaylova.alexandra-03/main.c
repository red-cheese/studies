#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char logical_addr[9];
char selector_string[5];
unsigned int gdt_length, ldt_length;
char gdt[8192][17];
char ldt[8192][17];

int main() {
  while(!feof(stdin)) {
    scanf("%s", logical_addr);

    if (feof(stdin)) {
      break;
    }

    scanf("%s", selector_string);

    unsigned int i;
    
    // GDT.
    scanf("%d", &gdt_length);
    for (i = 0; i < gdt_length; i++) {
      scanf("%s", gdt[i]); 
    }

    // LDT.
    scanf("%d", &ldt_length);
    for (i = 0; i < ldt_length; i++) {
      scanf("%s", ldt[i]);
    }

    unsigned short selector = strtoul(selector_string, NULL, 16);
    unsigned short index = (selector & (unsigned short) 0xFFF8) >> 3;
    uint64_t segment_descriptor;

    if (!(selector & (unsigned short) 4)) {
      // From GDT.
      if (!index || index >= gdt_length) {
	printf("INVALID\n");
	continue;
      }
      segment_descriptor = (uint64_t) strtoull(gdt[index], NULL, 16); 
    } else {
      // From LDT.
      if (index >= ldt_length) {
	printf("INVALID\n");
	continue;
      }
      segment_descriptor = (uint64_t) strtoull(ldt[index], NULL, 16);
    }

    unsigned int segment_limit = 0;
    segment_limit += (unsigned int) (segment_descriptor & ((uint64_t) 0xFFFF));
    segment_limit += (unsigned int) ((segment_descriptor & ((uint64_t) 0xF000000000000)) >> 32);
    int granularity = ((segment_descriptor & ((uint64_t) 0x80000000000000)) != 0);
    if (granularity) {
      segment_limit = (segment_limit + 1) * 4096 - 1;
    }

    unsigned int offset = strtoul(logical_addr, NULL, 16);

    int expand_down = ((segment_descriptor & ((uint64_t) 0x40000000000)) != 0);
    if (!expand_down) {
      if (offset > segment_limit) {
	printf("INVALID\n");
	continue;
      }
    } else {
      if (offset <= segment_limit) {
	printf("INVALID\n");
	continue;
      }
    }

    // "Build" the physical address.
    unsigned int physical_addr = 0;
    // First (less significant) 24 bits of the base address.
    physical_addr += (unsigned int) ((segment_descriptor & ((uint64_t) 0xFFFFFF0000)) >> 16);
    // 25-32 bits.
    physical_addr += (unsigned int) ((segment_descriptor & ((uint64_t) 0xFF00000000000000)) >> 32);    
    physical_addr += offset;

    printf("%08X", physical_addr);
    printf("\n");
  }

  return 0;
}
