# File author is Ítalo Lima Marconato Matias
#
# Created on February 24 of 2019, at 10:29 BRT
# Last edited on February 24 of 2019, at 10:30 BRT

VERBOSE ?= false
DEBUG ?= false

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all:
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chasm all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chdump all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chir all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chlink all

clean:
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chasm clean
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chdump clean
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chir clean
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chlink clean

clean-all:
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chasm clean-all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chdump clean-all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chir clean-all
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chlink clean-all

remake:
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chasm remake
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chdump remake
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chir remake
	$(NOECHO)VERBOSE=$(VERBOSE) DEBUG=$(DEBUG) make -C chlink reamke
