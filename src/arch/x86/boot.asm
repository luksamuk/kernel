bits 32
section .text

	; Coisas do Multiboot
align 4
	dd 0x1badb002 		 ; Número mágico do cabeçalho
	dd 0x00 		 ; Flags. Ignorar.
	dd - (0x1badb002 + 0x00) ; Checksum. Deve valer zero

global start

extern kmain
	
start:
	cli 			; Bloqueie interrupções
	mov esp, stack_space 	; Configura o endereço da pilha
	call kmain 		; Chame a função principal
	hlt			; Halt
	
section .bss
	resb 8192  		; Pilha do sistema com 8 KB
stack_space:
