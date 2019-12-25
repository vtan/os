.intel_syntax noprefix

.section .text
.global Port_in
Port_in:
  mov dx, [esp + 4]
  in al, dx
  ret

.global Port_out
Port_out:
  mov dx, [esp + 4]
  mov al, [esp + 4 + 4]
  out dx, al
  ret
