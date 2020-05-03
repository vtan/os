.intel_syntax noprefix

.global _start
_start:
  call main
  syscall
