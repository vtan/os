#include "kernel.hpp"

#include "String.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"

#define LOWER_4K_MASK 07777
#define LOWER_9_BIT_MASK 0777
#define TABLE_ENTRY_TO_PAGE_BASE(entry) (((uintptr_t) entry & ~LOWER_4K_MASK) + KERNEL_MEMORY_OFFSET)
#define PAGE_BASE_TO_TABLE_ENTRY(base) ((uintptr_t) base - KERNEL_MEMORY_OFFSET)

#define PAGE_PRESENT 1
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)

extern void* kernelPageTableL3;

PageDirectory PageDirectoryManager::create() {
  PageDirectory directory;
  directory.l4Table = (uint64_t*) this->pageAllocator.allocate();
  memset((uint8_t*) directory.l4Table, 0, 511 * sizeof(uint64_t));

  const uintptr_t kernelL3Flags = PAGE_PRESENT | PAGE_WRITABLE; // TODO writable is temporary
  directory.l4Table[511] = PAGE_BASE_TO_TABLE_ENTRY(&kernelPageTableL3) | kernelL3Flags;

  return directory;
}

void PageDirectoryManager::addMapping(
  const uintptr_t virtualPageBase,
  const uintptr_t physicalPageBase,
  PageDirectory& directory
) {
  kprintf("Mapping page 0x%x to 0x%x\n", virtualPageBase, physicalPageBase);

  if (virtualPageBase & 0xFFFF800000000000) {
    klog("Invalid virtual page base: %x", virtualPageBase);
    return;
  }

  size_t l4Index = (virtualPageBase >> (12 + 9 + 9 + 9)) & LOWER_9_BIT_MASK;
  size_t l3Index = (virtualPageBase >> (12 + 9 + 9)) & LOWER_9_BIT_MASK;
  size_t l2Index = (virtualPageBase >> (12 + 9)) & LOWER_9_BIT_MASK;
  size_t l1Index = (virtualPageBase >> 12) & LOWER_9_BIT_MASK;

  uint64_t* l3Table = getOrAllocSubtable(directory.l4Table, l4Index);
  uint64_t* l2Table = getOrAllocSubtable(l3Table, l3Index);
  uint64_t* l1Table = getOrAllocSubtable(l2Table, l2Index);

  if (l1Table[l1Index] == 0) {
    const uintptr_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    l1Table[l1Index] = (physicalPageBase & ~LOWER_4K_MASK) | flags;
  } else {
    const uintptr_t physicalDestination = TABLE_ENTRY_TO_PAGE_BASE(l1Table[l1Index]) + KERNEL_MEMORY_OFFSET;
    klog("Virtual page is already mapped: %x; to %x", virtualPageBase, physicalDestination);
  }
}

uint64_t* PageDirectoryManager::getOrAllocSubtable(uint64_t* table, const size_t index) {
  uint64_t* subtable = (uint64_t*) table[index];
  if (subtable == 0) {
    subtable = (uint64_t*) this->pageAllocator.allocate();
    const uintptr_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    table[index] = PAGE_BASE_TO_TABLE_ENTRY(subtable) | flags;
  } else {
    subtable = (uint64_t*) TABLE_ENTRY_TO_PAGE_BASE((uintptr_t) subtable);
  }
  return subtable;
}
