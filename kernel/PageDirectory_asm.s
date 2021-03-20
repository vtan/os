section .text

global PageDirectory_use
PageDirectory_use:
  mov cr3, rdi
  ret

global PageDirectory_flush
PageDirectory_flush:
  mov rax, cr3
  mov cr3, rax
  ret
