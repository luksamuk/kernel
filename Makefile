CC       := gcc
CFLAGS   := -m32 -fno-stack-protector -ffreestanding -O2 -Wall -Wextra -fno-exceptions

LDD      := ld
LDDFLAGS := -m elf_i386

ARCH     := x86
ASM      := nasm
ASMFLAGS := -f elf32

CSRC     := $(wildcard src/*.c)
ASMSRC   := $(wildcard src/arch/$(ARCH)/*.asm)

OBJS     := $(ASMSRC:src/arch/$(ARCH)/%.asm=bin/obj/$(ARCH)/%.o) $(CSRC:src/%.c=bin/obj/%.o)
BIN      := bin/kernel.bin

.PHONY: dirs clean

all: dirs $(BIN)

bin/obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/obj/$(ARCH)/%.o: src/arch/$(ARCH)/%.asm
	$(ASM) $(ASMFLAGS) $^ -o $@

$(BIN): src/link.ld $(OBJS)
	ld -m elf_i386 -T src/link.ld -o $(BIN) $(OBJS)

dirs:
	mkdir -p bin/obj/$(ARCH)

clean:
	rm -rf bin/obj

