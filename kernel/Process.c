#include "kernel.h"
#include "string.h"
#include "Elf64.h"
#include "PageAlloc.h"
#include "PageDirectory.h"
#include "Process.h"

PRIVATE struct Elf64_SectionHeaderEntry* findTextSection(struct Elf64_Header*);

void Process_load(void* elf, struct Process* process) {
  struct Elf64_Header* header = elf;

  if (header->programHeaderEntrySize == 0) {
    klog("No program header in ELF");
    return;
  }

  struct Elf64_ProgramHeaderEntry* segment =
    (struct Elf64_ProgramHeaderEntry*)
    ((uint8_t*) header + header->programHeaderOffset);
  if (segment->alignment != PAGE_SIZE) {
    klog("Segment is not page-aligned in ELF: %x", segment->alignment);
    return;
  }
  if (segment->memorySize > PAGE_SIZE) {
    klog("Segment needs more than one page; size: %d", segment->memorySize);
    return;
  }

  uint64_t* pageDirectory = PageDirectory_new();
  void* segmentPage = PageAlloc_alloc();
  void* stackPage = PageAlloc_alloc();
  PageDirectory_map(segment->virtualAddress, (uintptr_t) segmentPage - KERNEL_MEMORY_OFFSET, pageDirectory);
  uintptr_t stackBottom = segment->virtualAddress + PAGE_SIZE;
  PageDirectory_map(stackBottom, (uintptr_t) stackPage - KERNEL_MEMORY_OFFSET, pageDirectory);
  PageDirectory_use((uint64_t*) (((uintptr_t) pageDirectory) - KERNEL_MEMORY_OFFSET));

  struct Elf64_SectionHeaderEntry* textSection = findTextSection(header);
  memcpy(
    (uint8_t*) textSection->virtualAddress,
    (uint8_t*) elf + textSection->offset,
    textSection->size
  );

  process->entryPoint = header->entryPoint;
  process->stackTop = stackBottom + PAGE_SIZE - 16;
}

PRIVATE struct Elf64_SectionHeaderEntry* findTextSection(struct Elf64_Header* header) {
  for (int i = 0; i < header->sectionHeaderEntryCount; ++i) {
    struct Elf64_SectionHeaderEntry* section =
      (struct Elf64_SectionHeaderEntry*)
      ((uint8_t*) header + header->sectionHeaderOffset + i * header->sectionHeaderEntrySize);
    if (section->type == Elf64_SECTION_PROGBITS) {
      return section;
    }
  }
  return 0;
}
