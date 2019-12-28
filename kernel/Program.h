#pragma once

struct Elf64_SectionHeaderEntry;

struct Elf64_SectionHeaderEntry* Program_findTextSecion(void* elf);
