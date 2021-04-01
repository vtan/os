section .text

global processRun
processRun:
  mov rax, [rdi]
  extern taskStateSegment
  mov [taskStateSegment + 4], rax    ; Putting kernel stack pointer in TSS.RSP0

  mov rcx, [rdi + 8]     ; Entry point
  mov rbp, [rdi + 16]    ; User stack pointer

  mov rsp, rbp          ; Setting up stack for process
  mov r11, 0x0202
  o64 sysret
