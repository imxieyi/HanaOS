CXX	= g++
LD	= ld
NASM	= nasm
QEMU	= qemu-system-i386
BUILD	= build
ISODIR	= isodir
KERNEL	= $(BUILD)/kernel
ISOFILE	= $(BUILD)/hanaos.iso
CXXFLAGS= -ffreestanding -fno-exceptions -fno-rtti -m32 -fpermissive -Iinclude
LDFLAGS	= -m elf_i386 -N
CXXSRC	= $(wildcard *.cpp)
ASMSRC	= $(wildcard *.asm)
CXXOBJ	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.o)))
ASMOBJ	= $(addprefix $(BUILD)/,$(notdir $(ASMSRC:.asm=.a.o)))
DEPS	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.d)))

default: iso

ifneq ($(MAKECMDGOALS),clean)
    include $(DEPS)
endif

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.d: %.cpp
	set -e;rm -f $@; \
	$(CXX) -MM -Iinclude $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$*.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD)/%.a.o: %.asm
	$(NASM) -f elf $*.asm -o $(BUILD)/$*.a.o

$(BUILD)/%.o: %.cpp
	$(CXX) -c $*.cpp -o $(BUILD)/$*.o $(CXXFLAGS)

$(KERNEL): $(ASMOBJ) $(CXXOBJ)
	$(LD) $(ASMOBJ) $(CXXOBJ) -T link.ld -o $(KERNEL) $(LDFLAGS)

$(ISOFILE): $(KERNEL)
	cp $(KERNEL) $(ISODIR)/boot/
	strip $(ISODIR)/boot/kernel
	grub-mkrescue -d /usr/lib/grub/i386-pc -o $(ISOFILE) $(ISODIR)

iso: $(BUILD) $(ISOFILE)

run: iso
	$(QEMU) -m 32 -vga std -cdrom $(ISOFILE)

clean:
	rm -rf $(BUILD)
