bits 32
section .irq

global read_port
global write_port
global load_idt
global keyboard_handler
global disable_cursor

extern kbd_handler_irq

read_port:
	mov edx, [esp + 4] 	; Copie o núm da porta (arg) para edx
	in   al, dx 		; Copie da porta [LSB de edx] para [LSB de eax]
	ret 			; Retorno de função Asm Intel x86 fica em eax

write_port:
	mov edx, [esp + 4] 	; Copie o núm. da porta (arg 1) para edx
	mov  al, [esp + 4 + 4] 	; Copie o ptr do dado (arg 2) para [LSB de eax]
	out  dx, al		; Escreva o dado na porta [LSB de edx]
	ret

load_idt:
	mov  edx, [esp + 4] 	; Copie o endereço do ptr do IDT para edx
	lidt [edx] 		; Carregue dados do IDT do ptr em edx
	sti 			; Ligue interrupções
	ret

keyboard_handler:
	call kbd_handler_irq 	; Chame o handler externo, escrito em C
	iretd 			; Retornando de um interrupt handler

disable_cursor:
	; https://wiki.osdev.org/Text_Mode_Cursor
	mov dx, 0x3d4		; Na porta 980
	mov al, 0xa		; Flag de formato do cursor
	out dx, al		; Escreva o dado na porta [LSB de edx]

	inc dx			; Na porta 981
	mov al, 0x20		; bit 5 desabilita cursor, 0~4 determinam forma
	out dx, al		; Escreva o dado na porta [LSB de edx]
	ret

