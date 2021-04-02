section .text

struc Process
  .kernelStackPointer: resq 1
  .continuation: resq 1
  .pageDirectory: resq 1
endstruc

extern taskStateSegment
extern runningProcess
extern jumpViaInterruptFrame

global switchToProcess
switchToProcess:
  mov rax, [rdi + Process.kernelStackPointer]
  mov [taskStateSegment + 4], rax    ; Putting kernel stack pointer in TSS.RSP0

  mov rax, [rdi + Process.pageDirectory]
  mov cr3, rax

  mov rsp, [rdi + Process.continuation]
  jmp jumpViaInterruptFrame
