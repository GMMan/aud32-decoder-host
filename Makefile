BINPATH	?= /c/projects/shared/gcc-arm-none-eabi-10.3-2021.10/bin

CC	:= ${BINPATH}/arm-none-eabi-gcc
CFLAGS	+= -g -mcpu=cortex-m3 -ffreestanding

LD	:= ${BINPATH}/arm-none-eabi-ld
OBJCOPY	:= ${BINPATH}/arm-none-eabi-objcopy

OBJDIR	= obj


.PHONY: all clean

all: convert.bin

%.bin: $(OBJDIR)/%.elf
		$(OBJCOPY) -O binary $< $@

$(OBJDIR)/convert.elf: convert.ld $(OBJDIR)/convert.o
		$(LD) -Map=convert.map -T convert.ld $(OBJDIR)/convert.o -o $@

$(OBJDIR)/%.o: %.S
		$(CC) -o $@ $(CFLAGS) -c $<

$(OBJDIR)/%.o: %.c
		$(CC) -o $@ $(CFLAGS) -c $<

clean:
		$(RM) $(OBJDIR)/* *.bin *.map
