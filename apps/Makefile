SHELL	= bash
CXX	= g++
LD	= ld
BUILD	= ../build
CXXFLAGS= -ffreestanding -fno-exceptions -fno-rtti -m32 -fpermissive -Wwrite-strings -I../include -w
CXXSRC	= $(wildcard *.cpp)
CXXOBJ	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.o)))
DEPS	= $(addprefix $(BUILD)/,$(notdir $(CXXSRC:.cpp=.d)))

define colorecho
      @echo -e "\033[36m$1\033[0m"
endef

default: $(BUILD)/apps.o

.PHONY: $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.d: %.cpp | $(BUILD)
	$(call colorecho,GENDEP\t$<)
	@set -e;rm -f $@; \
	$(CXX) -MM -I../include $< > $@.$$$$; \
	sed 's,.*\.o[ :]*,$*.o $@ : ,g' < $@.$$$$ > $@; \
	echo -n '$(BUILD)/'|cat - $@ > $@.tmp;mv $@.tmp $@; \
	rm -f $@.$$$$

$(BUILD)/%.o: %.cpp
	$(call colorecho,CXX\t$<)
	@$(CXX) -c $*.cpp -o $(BUILD)/$*.o $(CXXFLAGS)

$(BUILD)/apps.o: $(CXXOBJ)
	@ld -r $(CXXOBJ) -o $(BUILD)/apps.o -m elf_i386

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPS)
endif
