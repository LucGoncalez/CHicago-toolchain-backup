# File author is Ítalo Lima Marconato Matias
#
# Created on November 19 of 2018, at 17:52 BRT
# Last edited on November 24 of 2018, at 10:25 BRT

VERBOSE ?= false
DEBUG ?= false

OBJECTS := codegen.c.o main.c.o ir.c.o ir-int.c.o str.c.o x86.c.o

OBJECTS := $(addprefix build/,$(OBJECTS))
OUTPUT := build/chir

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(OUTPUT)

clean:
	$(NOECHO)rm -f $(OBJECTS) $(OUTPUT)

clean-all:
	$(NOECHO)rm -rf build

remake: clean all

$(OUTPUT): $(OBJECTS)
	$(NOECHO)echo Linking $@
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(NOECHO)gcc -O3 -o $@ $(OBJECTS) $(LDFLAGS)

build/%.c.o: %.c
	$(NOECHO)echo Compiling $<
	$(NOECHO)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
ifeq ($(DEBUG),yes)
	$(NOECHO)gcc -g -std=gnu11 -DDEBUG -Iinclude -O0 -Wall -Wextra $(CFLAGS) -c $< -o $@
else
	$(NOECHO)gcc -std=gnu11 -Iinclude -O3 -Wall -Wextra $(CFLAGS) -c $< -o $@
endif
