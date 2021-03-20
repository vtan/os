; See https://wiki.osdev.org/Calling_Global_Constructors

section .init
	pop rbp
	ret

section .fini
	pop rbp
	ret
