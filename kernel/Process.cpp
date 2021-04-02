#include "kernel.hpp"

#include "Elf64.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"
#include "Process.hpp"
#include "String.hpp"

static struct Elf64::SectionHeaderEntry* findTextSection(struct Elf64::Header*);

static InterruptFrame* initializeKernelStack(void* stackTop, UserAddress entryPoint, UserAddress userStackTop) {
  InterruptFrame* initialProcessState = (InterruptFrame*) ((uint8_t*) stackTop - sizeof(InterruptFrame));
  memset(initialProcessState, 0, sizeof(InterruptFrame));
  initialProcessState->rip = entryPoint.address;
  initialProcessState->rsp = userStackTop.address;
  initialProcessState->rbp = userStackTop.address;
  initialProcessState->cs = USER_CODE_SEGMENT;
  initialProcessState->ss = USER_DATA_SEGMENT;
  initialProcessState->rflags = 0x202;
  return initialProcessState;
}

void ProcessLoader::load(void* elf, struct Process* process) {
  struct Elf64::Header* header = (Elf64::Header*) elf;

  assert(header->programHeaderEntrySize > 0);

  struct Elf64::ProgramHeaderEntry* segment =
    (Elf64::ProgramHeaderEntry*) ((uint8_t*) header + header->programHeaderOffset);
  assert(segment->alignment == PAGE_SIZE);
  assert(segment->memorySize <= PAGE_SIZE);

  PageDirectory pageDirectory = this->pageDirectoryManager.create();
  PhysicalAddress segmentPage = this->pageAllocator.allocate();
  PhysicalAddress kernelStackPage = this->pageAllocator.allocate();
  PhysicalAddress userStackPage = this->pageAllocator.allocate();
  this->pageDirectoryManager.addMapping(
    segment->virtualAddress,
    segmentPage,
    pageDirectory
  );
  UserAddress userStackBottom = segment->virtualAddress + PAGE_SIZE;
  this->pageDirectoryManager.addMapping(
    userStackBottom,
    userStackPage,
    pageDirectory
  );

  struct Elf64::SectionHeaderEntry* textSection = findTextSection(header);
  assert(textSection->virtualAddress >= segment->virtualAddress);
  assert(textSection->virtualAddress + textSection->size < segment->virtualAddress + PAGE_SIZE);
  memcpy(
    (segmentPage + (textSection->virtualAddress - segment->virtualAddress)).toVirtual(),
    (uint8_t*) elf + textSection->offset,
    textSection->size
  );

  auto kernelStackTop = (kernelStackPage + PAGE_SIZE).toVirtual();
  auto userStackTop = userStackBottom + PAGE_SIZE;
  process->kernelStackPointer = kernelStackTop;
  process->continuation = initializeKernelStack(kernelStackTop, header->entryPoint, userStackTop);
  process->pageDirectory = PhysicalAddress::ofVirtual(pageDirectory.l4Table);
  process->runnable = true;
}

static struct Elf64::SectionHeaderEntry* findTextSection(struct Elf64::Header* header) {
  for (int i = 0; i < header->sectionHeaderEntryCount; ++i) {
    struct Elf64::SectionHeaderEntry* section =
      (struct Elf64::SectionHeaderEntry*)
      ((uint8_t*) header + header->sectionHeaderOffset + i * header->sectionHeaderEntrySize);
    if (section->type == Elf64::SECTION_PROGBITS) {
      return section;
    }
  }
  return 0;
}
