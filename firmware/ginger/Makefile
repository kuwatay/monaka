###############################################################################
# Makefile for the project AVR-MIDI
###############################################################################

## General Flags
PROJECT = ginger-monaka
MCU = atmega88
MODEL = -DGINGER
TARGET = $(PROJECT).elf
#DEBUG =  -DDEBUG_LEVEL=2
DEBUG = 
CC = avr-gcc $(MODEL)
AVRDUDE = avrdude -c usbtiny -p$(MCU)
AVRDUDE88P = avrdude -c usbtiny -pm88p

FUSEH = 0xc1
FUSEL = 0xdf
# Fuse high byte:
# 0xc1 = 1 1 0 0   0 0 0 1 <-- BOOTRST (boot reset vector at 0x3800)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   + --------- EESAVE (preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (full output swing)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ WDTON (WDT not always on)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse low byte:
# 0xdf = 1 1 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ CKOUT (1 = diable)
#        +-------------------- CKDEV8 (CLOCK DEV by 8)


## Options common to compile, link and assembly rules
COMMON = -g -mmcu=$(MCU) -DF_CPU=12000000UL

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall  -Os -fsigned-char  $(DEBUG)

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=$(PROJECT).map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom


## Include Directories
INCLUDES = -I. -I../usbdrv

## Objects that must be built in order to link
OBJECTS = usbdrv.o usbdrvasm.o oddebug.o main.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) $(PROJECT).hex $(PROJECT).lss $(PROJECT).eep size

$(OBJECTS): usbconfig.h Makefile

## Compile
usbdrv.o: ../usbdrv/usbdrv.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usbdrvasm.o: ../usbdrv/usbdrvasm.S
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

oddebug.o: ../usbdrv/oddebug.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c ../freq_table.c ../monaka.h ginger.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(PROJECT).eep $(PROJECT).elf $(PROJECT).hex $(PROJECT).lss $(PROJECT).map *~ 


.PHONY: flash
flash:	all
	$(AVRDUDE) -U flash:w:$(PROJECT).hex -U eeprom:w:$(PROJECT).eep

.PHONY: fuse
fuse:
	$(AVRDUDE) -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m

.PHONY: flash
flash88p:	all
	$(AVRDUDE88P) -U flash:w:$(PROJECT).hex -U eeprom:w:$(PROJECT).eep

.PHONY: fuse
fuse88p:
	$(AVRDUDE88P) -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m

