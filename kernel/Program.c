#include "Program.h"
#include "elf64.h"

struct Elf64_SectionHeaderEntry* Program_findTextSecion(void* elf) {
  struct Elf64_Header* header = elf;
  for (int i = 0; i < header->sectionHeaderEntryCount; ++i) {
    struct Elf64_SectionHeaderEntry* section =
      (struct Elf64_SectionHeaderEntry*)
      (elf + header->sectionHeaderOffset + i * header->sectionHeaderEntrySize);
    if (section->type == Elf64_SECTION_PROGBITS) {
      return section;
    }
  }
  return 0;
}
