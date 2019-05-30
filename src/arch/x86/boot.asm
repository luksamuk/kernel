bits 32
section .multiboot
align 4
	dd 0x1badb002 		 ; Número mágico do cabeçalho
	dd 0x00 		 ; Flags. Ignorar.
	dd - (0x1badb002 + 0x00) ; Checksum. Deve valer zero

global start
extern kmain

section .text
start:
	cli 			; Bloqueie interrupções
	mov esp, stack_top 	; Configura o endereço da pilha
	;push eax		; Magic number (arg 2)
	push ebx		; Multiboot info (arg 1)
	call kmain 		; Chame a função principal
	hlt			; Halt
	
section .bss
align 16
stack_bottom:
	resb 16384  		; Pilha do sistema com 16 KB
stack_top:
