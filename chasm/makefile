# File author is Ítalo Lima Marconato Matias
#
# Created on December 02 of 2018, at 10:47 BRT
# Last edited on Januaray 14 of 2019, at 13:05 BRT

VERBOSE ?= false
DEBUG ?= false

OBJECTS := arch.c.o bin.c.o codegen.c.o elf32.c.o
OBJECTS += exec.c.o lexer.c.o main.c.o parser.c.o
OBJECTS += x86.c.o

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
