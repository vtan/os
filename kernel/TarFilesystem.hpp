#pragma once

#include "kernel.hpp"

struct TarMetadataBlock {
  char filename[100];
  uint64_t mode;
  uint64_t owner;
  uint64_t group;
  char sizeOctal[12];
  char lastModificationTimeOctal[12];
  uint64_t checksum;
  uint8_t type;
  char linkedFilename[100];
  char magic[6];
  char version[2];
  char ownerName[32];
  char groupName[32];
  uint64_t deviceMajor;
  uint64_t deviceMinor;
  char filenamePrefix[155];

  size_t size() const;
  TarMetadataBlock* next() const;
  void* fileContent() const;
} __attribute__((packed));

class TarFilesystem {
public:
  const TarMetadataBlock* start;

  explicit TarFilesystem(void* data) : start((TarMetadataBlock*) data) {}

  void listFiles();
};
