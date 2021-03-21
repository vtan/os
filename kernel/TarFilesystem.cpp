#include "TarFilesystem.hpp"
#include "kernel.hpp"

static uint64_t readOctal(const char* str, size_t size) {
  uint64_t result = 0;
  while (size > 0 && *str != 0) {
    result = (result << 3) | (*str - '0');
    ++str;
    --size;
  }
  return result;
}

size_t TarMetadataBlock::size() {
  return readOctal(sizeOctal, 12);
}

TarMetadataBlock* TarMetadataBlock::next() {
  const size_t sizeWithPadding = ROUND_UP_POW2(size(), 512);
  TarMetadataBlock* candidate = (TarMetadataBlock*) ((uintptr_t) this + 512 + sizeWithPadding);
  if (candidate->magic[0] == 'u') {
    return candidate;
  } else {
    return nullptr;
  }
}

void* TarMetadataBlock::fileContent() {
  return (void*) ((uintptr_t) this + 512);
}

void TarFilesystem::listFiles() {
  kprintf("Listing tar filesystem\n");
  for (TarMetadataBlock* block = start; block != nullptr; block = block->next()) {
    kprintf("- %s (%d bytes)\n", block->filename, block->size());
  }
}
