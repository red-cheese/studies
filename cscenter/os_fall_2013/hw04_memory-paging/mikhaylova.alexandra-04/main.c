#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char logical_addr[9];
char selector_string[5];

unsigned int gdt_length, ldt_length;
uint64_t gdt[8192];
uint64_t ldt[8192];

unsigned int pd_length, pt_length;
uint64_t pd[1024];
uint64_t pt[1024];

int ss_rpl = 0; // Segment selector privilege level.


unsigned int linear_address() {
  unsigned short selector = strtoul(selector_string, NULL, 16);
  ss_rpl = (int) (selector & ((unsigned short) 3));
  unsigned short index = (selector & (unsigned short) 0xFFF8) >> 3;
  uint64_t segment_descriptor;

  if (!(selector & (unsigned short) 4)) {
    // From GDT.
    if (!index || index >= gdt_length) {
      return 0;
    }
    segment_descriptor = gdt[index]; 
  } else {
    // From LDT.
    if (index >= ldt_length) {
      return 0;
    }
    segment_descriptor = ldt[index];
  }

  int dpl = (int) ((segment_descriptor >> 45) & ((uint64_t) 3));
  if (dpl < ss_rpl) {
    return 0;
  }

  unsigned int segment_limit = 0;
  segment_limit += (unsigned int) (segment_descriptor & ((uint64_t) 0xFFFF));
  // Very ugly, but works:
  segment_limit += (unsigned int) ((segment_descriptor & ((uint64_t) 0xF000000000000)) >> 32);
  int granularity = ((segment_descriptor & ((uint64_t) 0x80000000000000)) != 0);
  if (granularity) {
    segment_limit = (segment_limit + 1) * 4096 - 1;
  }

  unsigned int offset = strtoul(logical_addr, NULL, 16);

  int expand_down = ((segment_descriptor & ((uint64_t) 0x40000000000)) != 0);
  if (!expand_down) {
    if (offset > segment_limit) {
      return 0;
    }
  } else {
    if (offset <= segment_limit) {
      return 0;
    }
  }

  unsigned int linear_addr = 0;
  // First (less significant) 24 bits of the base address.
  linear_addr += (unsigned int) ((segment_descriptor >> 16) & ((uint64_t) 0xFFFFFF));
  // 25-32 bits.
  linear_addr += (unsigned int) ((segment_descriptor & ((uint64_t) 0xFF00000000000000)) >> 32);    
  linear_addr += offset;

  return linear_addr;
}

int check_presence(uint64_t entry) {
  return ((entry & ((uint64_t) 1)) != 0);
}

int check_access(uint64_t entry) {
  int user_access_allowed = (int) ((entry >> 2) & ((uint64_t) 1));
  return user_access_allowed || ss_rpl < 3;
}

int main() {
  while(!feof(stdin)) {
    scanf("%s", logical_addr);

    if (feof(stdin)) {
      break;
    }

    scanf("%s", selector_string);

    unsigned int i = 0;

    char tstr[17];
    
    // GDT.
    scanf("%u", &gdt_length);
    for (i = 0; i < gdt_length; i++) {
      scanf("%s", tstr);
      gdt[i] = strtoull(tstr, NULL, 16);
    }

    // LDT.
    scanf("%u", &ldt_length);
    for (i = 0; i < ldt_length; i++) {
      scanf("%s", tstr);
      ldt[i] = (uint64_t) strtoull(tstr, NULL, 16);
    }

    // PD.
    scanf("%u", &pd_length);
    for (i = 0; i < pd_length; i++) {
      scanf("%s", tstr);
      pd[i] = (uint64_t) strtoull(tstr, NULL, 16);
    }

    // PT.
    scanf("%u", &pt_length);
    for (i = 0; i < pt_length; i++) {
      scanf("%s", tstr);
      pt[i] = (uint64_t) strtoull(tstr, NULL, 16);
    }

    //
    // Calculations.
    //

    unsigned int physical_addr = 0;

    unsigned int linear_addr = linear_address();
    if (linear_addr == 0) {
      printf("INVALID\n");
      continue;
    }

    unsigned short pde_index = (unsigned short) ((linear_addr & ((unsigned int) 0xFFC00000)) >> 22);
    if (pde_index >= pd_length) {
      printf("INVALID\n");
      continue;
    }
    uint64_t pde = pd[pde_index];

    if (!check_access(pde)) {
      printf("INVALID\n");
      continue;
    }

    int page_presence = 0;

    // Check granularity.
    int ps_bit = ((pde & ((uint64_t) 0x80)) != 0);
    if (!ps_bit) {
      // 4-KByte pages.
      if (!check_presence(pde)) {
	printf("INVALID\n");
	continue;
      }
      unsigned short pte_index = (unsigned short) ((linear_addr & ((unsigned int) 0x3FF000)) >> 12);
      if (pte_index >= pt_length) {
	printf("INVALID\n");
	continue;
      }
      uint64_t pte = pt[pte_index];
      if (!check_access(pte)) {
	printf("INVALID\n");
	continue;
      }
      page_presence = check_presence(pte);
      unsigned int page_base = (unsigned int) ((pte & ((uint64_t) 0xFFFFF000)) >> 12);
      unsigned int offset = linear_addr & ((unsigned int) 0xFFF);
      if (offset >= 4096) { // In fact, if (offset == 4096).
	printf("INVALID\n");
	continue;
      }
      physical_addr = page_base + offset;
    } else {
      // 4-MByte pages.
      page_presence = check_presence(pde);
      unsigned int page_base = 0;
      page_base += (unsigned int) ((pde & ((uint64_t) 0xFFC00000)) >> 22);
      unsigned int offset = linear_addr & ((unsigned int) 0x3FFFFF);
      physical_addr = page_base + offset;
    }

    printf("%08X:%d\n", physical_addr, page_presence);
  }

  return 0;
}
