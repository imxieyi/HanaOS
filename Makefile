CC	= gcc
CXX	= g++
LD	= ld
NASM	= nasm
QEMU	= qemu-system-i386
BUILD	= build
ISODIR	= isodir
ISOFILE	= $(BUILD)/hanaos.iso
EFIBIOS	= /usr/share/ovmf/OVMF.fd
CCFLAGS	= -nostdlib -nodefaultlibs -lgcc -m32
CXXFLAGS= -ffreestanding -fno-exceptions -fno-rtti -m32 -fpermissive -Iinclude
LDFLAGS	= -m elf_i386 -N
OBJECTS	= $(BUILD)/kernel.o $(BUILD)/boot.a.o $(BUILD)/graphics.o $(BUILD)/gdt.o $(BUILD)/idt.o $(BUILD)/table_flush.a.o $(BUILD)/int_stubs.a.o $(BUILD)/isr.o $(BUILD)/irq.o $(BUILD)/asmfunc.a.o

default:
	make kernel

%.a.o: %.asm Makefile
	$(NASM) -f elf $*.asm -o $(BUILD)/$*.a.o

%.o: %.cpp Makefile
	$(CXX) -c $*.cpp -o $(BUILD)/$*.o $(CXXFLAGS)

kernel: boot.a.o asmfunc.a.o isr.o irq.o table_flush.a.o int_stubs.a.o graphics.o gdt.o idt.o kernel.o link.ld
	$(LD) $(OBJECTS) -T link.ld -o $(BUILD)/kernel $(LDFLAGS)

iso: kernel
	cp $(BUILD)/kernel $(ISODIR)/boot/
	grub-mkrescue -o $(ISOFILE) $(ISODIR)

run: iso
	qemu-system-i386 -vga std -cdrom $(ISOFILE)

clean:
	rm $(BUILD)/*
