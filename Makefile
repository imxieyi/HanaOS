SHELL	= bash
CXX	= g++
LD	= ld
NASM	= nasm
QEMU	= qemu-system-i386
BUILD	= build
ISODIR	= isodir
KERNEL	= $(BUILD)/kernel
ISOFILE	= $(BUILD)/hanaos.iso
CXXFLAGS= -ffreestanding -fno-exceptions -fno-rtti -m32 -fpermissive -Wwrite-strings -Iinclude -w
LDFLAGS	= -m elf_i386 -N
CXXSRC	= $(wildcard *.cpp)
ASMSRC	= $(wildcard *.asm)
CXXOBJ	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.o)))
ASMOBJ	= $(addprefix $(BUILD)/,$(notdir $(ASMSRC:.asm=.a.o)))
DEPS	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.d)))

define colorecho
      @tput setaf 6
      @echo -e $1
      @tput sgr0
endef

default: iso

.PHONY: $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.d: %.cpp | $(BUILD)
	$(call colorecho,"GENDEP\t$<")
	@set -e;rm -f $@; \
	$(CXX) -MM -Iinclude $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$*.o $@ : ,g' < $@.$$$$ > $@; \
	echo -n '$(BUILD)/'|cat - $@ > $@.tmp;mv $@.tmp $@; \
	rm -f $@.$$$$

$(BUILD)/%.a.o: %.asm
	$(call colorecho,"NASM\t$<")
	@$(NASM) -f elf $*.asm -o $(BUILD)/$*.a.o

$(BUILD)/%.o: %.cpp
	$(call colorecho,"CXX\t$<")
	@$(CXX) -c $*.cpp -o $(BUILD)/$*.o $(CXXFLAGS)

$(KERNEL): $(ASMOBJ) $(CXXOBJ)
	$(call colorecho,"Link kernel...")
	@$(LD) $(ASMOBJ) $(CXXOBJ) -T link.ld -o $(KERNEL) $(LDFLAGS)

$(ISOFILE): $(KERNEL)
	$(call colorecho,"Generate iso image...")
	@cp $(KERNEL) $(ISODIR)/boot/
	@strip $(ISODIR)/boot/kernel
	@grub-mkrescue -d /usr/lib/grub/i386-pc -o $(ISOFILE) $(ISODIR)

iso: $(ISOFILE) | $(BUILD)

run: iso
	$(QEMU) -m 32 -vga std -cdrom $(ISOFILE)

clean:
	rm -rf $(BUILD)

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPS)
endif
