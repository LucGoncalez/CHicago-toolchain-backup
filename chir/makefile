# File author is Ítalo Lima Marconato Matias
#
# Created on February 23 of 2019, at 16:56 BRT
# Last edited on February 23 of 2019, at 18:16 BRT

VERBOSE ?= false
DEBUG ?= false

OBJECTS := lexer.c.o main.c.o parser.c.o

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
	$(NOECHO)gcc -std=gnu11 -Iinclude -O3 -Wall -Wextra -Wno-stringop-truncation $(CFLAGS) -c $< -o $@
endif
