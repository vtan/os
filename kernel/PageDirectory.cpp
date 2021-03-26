#include "kernel.hpp"

#include "String.hpp"
#include "PageAlloc.hpp"
#include "PageDirectory.hpp"

#define LOWER_4K_MASK 07777
#define LOWER_9_BIT_MASK 0777

#define PAGE_PRESENT 1
#define PAGE_WRITABLE (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_LARGE 0x80

extern void* kernelPageTableL3;

struct PageDirectoryEntry {
  uint64_t entry;

  PageDirectoryEntry(PhysicalAddress address, uint64_t flags) {
    assert((address.address & LOWER_4K_MASK) == 0);
    assert((flags & ~LOWER_4K_MASK) == 0);
    entry = address.address | flags;
  }

  PhysicalAddress pageAddress() const { return { entry & ~LOWER_4K_MASK }; }
  operator bool() const { return entry; }
};

void mapPhysicalMemoryToKernel() {
  // TODO: do not map kernel code here (or at least not as writable)
  uint64_t* const firstEntry = (uint64_t*) &kernelPageTableL3;
  for (uint64_t i = 0; i < 510; ++i) {
    *(firstEntry + i) = i << 30 | PAGE_PRESENT | PAGE_WRITABLE | PAGE_LARGE;
  }
  PageDirectory_flush();
}

PageDirectory PageDirectoryManager::create() {
  PageDirectory directory;
  directory.l4Table = (PageDirectoryEntry*) this->pageAllocator.allocate().toVirtual();
  memset(directory.l4Table, 0, 511 * sizeof(uint64_t));

  const uintptr_t kernelL3Flags = PAGE_PRESENT | PAGE_WRITABLE; // TODO writable is temporary
  directory.l4Table[511] = PageDirectoryEntry(PhysicalAddress::ofKernelStatic(&kernelPageTableL3), kernelL3Flags);

  return directory;
}

void PageDirectoryManager::addMapping(
  const UserAddress virtualAddress,
  const PhysicalAddress physicalAddress,
  PageDirectory& directory
) {
  const uintptr_t virtualPageBase = virtualAddress.address;
  kprintf("Mapping virtual page %p to physical %p\n", virtualPageBase, physicalAddress.address);

  assert((virtualPageBase & 0xFFFF800000000000) == 0);
  assert((physicalAddress.address & LOWER_4K_MASK) == 0)

  size_t l4Index = (virtualPageBase >> (12 + 9 + 9 + 9)) & LOWER_9_BIT_MASK;
  size_t l3Index = (virtualPageBase >> (12 + 9 + 9)) & LOWER_9_BIT_MASK;
  size_t l2Index = (virtualPageBase >> (12 + 9)) & LOWER_9_BIT_MASK;
  size_t l1Index = (virtualPageBase >> 12) & LOWER_9_BIT_MASK;

  PageDirectoryEntry* l3Table = getOrAllocSubtable(directory.l4Table, l4Index);
  PageDirectoryEntry* l2Table = getOrAllocSubtable(l3Table, l3Index);
  PageDirectoryEntry* l1Table = getOrAllocSubtable(l2Table, l2Index);
  assert(l1Table[l1Index] == 0);

  const uintptr_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
  l1Table[l1Index] = PageDirectoryEntry(physicalAddress, flags);
}

PageDirectoryEntry* PageDirectoryManager::getOrAllocSubtable(PageDirectoryEntry* table, const size_t index) {
  if (PageDirectoryEntry entry = table[index]) {
    return (PageDirectoryEntry*) entry.pageAddress().toVirtual();
  } else {
    PhysicalAddress subtableAddress = this->pageAllocator.allocate();
    const uintptr_t flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    const PageDirectoryEntry newEntry(subtableAddress, flags);
    table[index] = newEntry;
    return (PageDirectoryEntry*) subtableAddress.toVirtual();
  }
}
