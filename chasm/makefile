# File author is Ítalo Lima Marconato Matias
#
# Created on December 02 of 2018, at 10:47 BRT
# Last edited on Januaray 10 of 2019, at 11:02 BRT

VERBOSE ?= false
DEBUG ?= false

OBJECTS := arch.c.o codegen.c.o elf32.c.o exec.c.o
OBJECTS += lexer.c.o main.c.o parser.c.o x86.c.o

OBJECTS := $(addprefix build/,$(OBJECTS))
OUTPUT := build/chasm

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
	$(NOECHO)gcc -std=gnu11 -Iinclude -O3 -Wall -Wextra -Wno-stringop-truncation $(CFLAGS) -c $< -o $@
endif
