section .text

struc Process
  .kernelStackPointer: resq 1
  .entryPoint: resq 1
  .userStackPointer: resq 1
endstruc

extern taskStateSegment

global processRun
processRun:
  mov rax, [rdi + Process.kernelStackPointer]
  mov [taskStateSegment + 4], rax    ; Putting kernel stack pointer in TSS.RSP0

  mov rcx, [rdi + Process.entryPoint]
  mov rbp, [rdi + Process.userStackPointer]

  mov rsp, rbp          ; Setting up stack for process
  mov r11, 0x0202
  o64 sysret
