# File author is Ítalo Lima Marconato Matias
#
# Created on January 28 of 2018, at 16:46 BRT
# Last edited on February 15 of 2019, at 15:24 BRT

VERBOSE ?= false
DEBUG ?= false

OBJECTS := chexec32.c.o context.c.o exec.c.o main.c.o

OBJECTS := $(addprefix build/,$(OBJECTS))
OUTPUT := build/chdump

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
	$(NOECHO)gcc -g -std=gnu11 -DDEBUG -Iinclude -O0 -Wall -Wextra -Wno-stringop-truncation -Wno-format $(CFLAGS) -c $< -o $@
else
	$(NOECHO)gcc -std=gnu11 -Iinclude -O3 -Wall -Wextra -Wno-stringop-truncation -Wno-format $(CFLAGS) -c $< -o $@
endif
