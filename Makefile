CC       := gcc
CFLAGS   := -m32 -fno-stack-protector -ffreestanding -O2 -Wall -Wextra -fno-exceptions

LDD      := ld
LDDFLAGS := -m elf_i386

ASM      := nasm
ASMFLAGS := -f elf32

CSRC     := $(wildcard *.c)
ASMSRC   := $(wildcard *.asm)

OBJS     := $(ASMSRC:%.asm=%.asm_o) $(CSRC:%.c=%.o)
BIN      := kernel.bin

.PHONY: clean

all: $(BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

%.asm_o: %.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

$(BIN): link.ld $(OBJS)
	ld -m elf_i386 -T link.ld -o $(BIN) $(OBJS)

clean:
	rm -f *.o *.asm_o

