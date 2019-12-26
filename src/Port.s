.intel_syntax noprefix

.section .text
.global Port_in
Port_in:
  mov dx, di
  in al, dx
  ret

.global Port_out
Port_out:
  mov dx, di
  mov al, sil
  out dx, al
  ret
