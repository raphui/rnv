export

TARGET=rnv

ifeq (${MAKELEVEL}, 0)
INCLUDES	+= -Iinclude

ASFLAGS	:= -g $(INCLUDES)
CFLAGS  :=  -Wall -g $(INCLUDES) -MD -MP
LDFLAGS	:= -g $(INCLUDES) -Wl,-Map=rnv.map

CC := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)as
AR := $(CROSS_COMPILE)ar
LD := $(CROSS_COMPILE)ld

endif

obj-y := rnv.o
obj-y += vm.o
obj-y += mm.o
obj-y += inst.o

.PHONY: all clean $(TARGET)

ifdef DEBUGMAKE
else
PREFIX=@
endif

ifeq (${MAKELEVEL}, 0)
all: ${TARGET}

$(TARGET):
	$(PREFIX)rm -f objects.lst
	$(PREFIX)$(MAKE) -f Makefile.common dir=. all
	$(PREFIX)$(CC) -o $@ `cat objects.lst | tr '\n' ' '` $(LDFLAGS)

cscope:
	@@echo "GEN " $@
	$(PREFIX)cscope -b -q -k -R
 
include $(wildcard *.d)
 
clean:
	$(PREFIX)$(MAKE) -f Makefile.common dir=. $@
	$(PREFIX)rm $(TARGET)

dist-clean: clean
	$(PREFIX)$(RM) `find . -name *.d`
endif

ifndef VERBOSE
.SILENT:
endif
