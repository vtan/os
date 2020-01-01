# See https://wiki.osdev.org/Calling_Global_Constructors

.intel_syntax noprefix

.section .init
	pop %rbp
	ret

.section .fini
	pop %rbp
	ret
