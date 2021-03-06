#pragma once

#include "kernel.hpp"

namespace Elf64 {

constexpr uint32_t MAGIC_NUMBER = 0x464C457F;

constexpr uint8_t SECTION_PROGBITS = 1;

struct Header {
  uint32_t magicNumber;
  uint8_t identification[12];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  UserAddress entryPoint;
  uint64_t programHeaderOffset;
  uint64_t sectionHeaderOffset;
  uint32_t flags;
  uint16_t headerSize;
  uint16_t programHeaderEntrySize;
  uint16_t programHeaderEntryCount;
  uint16_t sectionHeaderEntrySize;
  uint16_t sectionHeaderEntryCount;
  uint16_t sectionNamesEntryIndex;
} PACKED;

struct ProgramHeaderEntry {
  uint32_t type;
  uint32_t flags;
  uint64_t offset;
  UserAddress virtualAddress;
  uint64_t physicalAddress;
  uint64_t fileSize;
  uint64_t memorySize;
  uint64_t alignment;
} PACKED;

struct SectionHeaderEntry {
  uint32_t nameOffset;
  uint32_t type;
  uint64_t flags;
  UserAddress virtualAddress;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t alignment;
  uint64_t entrySize;
} PACKED;

}
