KEIL_ROOT := C:/Keil_v5/ARM/ARMCC
CC := $(KEIL_ROOT)/bin/armcc.exe
CXX := $(CC)
LD := $(KEIL_ROOT)/bin/armlink.exe
AR := $(KEIL_ROOT)/bin/armar.exe
FROMELF := $(KEIL_ROOT)/bin/fromelf.exe

COMMONFLAGS += --cpu Cortex-M3 --apcs=interwork

LDFLAGS += --cpu Cortex-M3 \
--ro-base 0x08000000 \
--entry 0x08000000 \
--rw-base 0x20000000 \
--entry Reset_Handler \
--first __Vectors \
--strict \
--summary_stderr \
--info summarysizes \
--map \
--xref \
--callgraph \
--symbols \
--info sizes \
--info totals \
--info unused \
--info veneers \
--list "$(BINARYDIR)/ebox.map"