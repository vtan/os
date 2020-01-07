#pragma once

class PageAllocator {
  void* nextPage;

public:
  PageAllocator(void* memoryBase) : nextPage(memoryBase) {}

  void* allocate();
};
