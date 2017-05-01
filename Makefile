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
CXXFLAGS= -ffreestanding -fno-exceptions -fno-rtti -m32
LDFLAGS	= -m elf_i386 -N
OBJECTS	= $(BUILD)/kernel.o $(BUILD)/boot.o $(BUILD)/asmfunc.o

default:
	make kernel

boot.o: boot.asm
	$(NASM) -f elf boot.asm -o $(BUILD)/boot.o

asmfunc.o: asmfunc.asm
	$(NASM) -f elf asmfunc.asm -o $(BUILD)/asmfunc.o

%.o: %.cpp Makefile
	$(CXX) -c $*.cpp -o $(BUILD)/$*.o $(CXXFLAGS)

kernel: boot.o asmfunc.o kernel.o link.ld
	$(LD) $(OBJECTS) -T link.ld -o $(BUILD)/kernel $(LDFLAGS)

run: kernel
#	$(QEMU) -kernel $(BUILD)/kernel
	cp $(BUILD)/kernel $(ISODIR)/boot/
	grub-mkrescue -o $(ISOFILE) $(ISODIR)
	qemu-system-i386 -vga std -cdrom $(ISOFILE)

clean:
	rm $(BUILD)/*
