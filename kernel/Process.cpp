#include "kernel.hpp"

#include "Elf64.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"
#include "Process.hpp"
#include "String.hpp"

PRIVATE struct Elf64_SectionHeaderEntry* findTextSection(struct Elf64_Header*);

void ProcessLoader::load(void* elf, struct Process* process) {
  struct Elf64_Header* header = (Elf64_Header*) elf;

  assert(header->programHeaderEntrySize > 0);

  struct Elf64_ProgramHeaderEntry* segment =
    (struct Elf64_ProgramHeaderEntry*)
    ((uint8_t*) header + header->programHeaderOffset);
  assert(segment->alignment == PAGE_SIZE);
  assert(segment->memorySize <= PAGE_SIZE);

  PageDirectory pageDirectory = this->pageDirectoryManager.create();
  void* segmentPage = this->pageAllocator.allocate();
  // TODO: doesn't map the kernel stack page, assumes it's in the already mapped superpage
  void *kernelStackPage = this->pageAllocator.allocate();
  void* userStackPage = this->pageAllocator.allocate();
  this->pageDirectoryManager.addMapping(
    segment->virtualAddress,
    (uintptr_t) segmentPage - KERNEL_STATIC_MEMORY_OFFSET,
    pageDirectory
  );
  uintptr_t userStackBottom = segment->virtualAddress + PAGE_SIZE;
  this->pageDirectoryManager.addMapping(
    userStackBottom,
    (uintptr_t) userStackPage - KERNEL_STATIC_MEMORY_OFFSET,
    pageDirectory
  );

  struct Elf64_SectionHeaderEntry* textSection = findTextSection(header);
  assert(textSection->virtualAddress >= segment->virtualAddress);
  assert(textSection->virtualAddress + textSection->size < segment->virtualAddress + PAGE_SIZE);
  memcpy(
    (uint8_t*) ((uintptr_t) segmentPage + (textSection->virtualAddress - segment->virtualAddress)),
    (uint8_t*) elf + textSection->offset,
    textSection->size
  );

  // TODO: instead of storing an entry point, push a syscall frame onto the kernel stack of the new process, then run it by returning into it
  process->entryPoint = header->entryPoint;
  process->userStackPointer = userStackBottom + PAGE_SIZE;
  process->kernelStackPointer = (uintptr_t) kernelStackPage + PAGE_SIZE;
  process->pageDirectory = pageDirectory;
  process->runnable = true;
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
