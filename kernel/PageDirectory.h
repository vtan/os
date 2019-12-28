#pragma once

#include "kernel.h"

uint64_t* PageDirectory_new();
void PageDirectory_map(uintptr_t virtualPageBase, uintptr_t physicalPageBase, uint64_t* l4Table);
void PageDirectory_use(uint64_t* physicalL4Table);