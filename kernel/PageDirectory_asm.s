.intel_syntax noprefix

.section .text

.global PageDirectory_use
PageDirectory_use:
  mov cr3, rdi
  ret
