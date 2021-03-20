section .text

global port_in8
port_in8:
  mov dx, di
  in al, dx
  ret

global port_out8
port_out8:
  mov dx, di
  mov al, sil
  out dx, al
  ret
