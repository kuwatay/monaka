###############################################################################
# Makefile for the project AVR-MIDI
###############################################################################

TARGET = ALL

all : ginger/ginger-monaka.hex pepper/pepper-monaka.hex arduino/arduino-monaka.hex

ginger/ginger-monaka.hex:
	cd ginger ; make

pepper/pepper-monaka.hex:
	cd pepper ; make

arduino/arduino-monaka.hex:
	cd arduino ; make

copy: ginger/ginger-monaka.hex pepper/pepper-monaka.hex arduino/arduino-monaka.hex
	cp ginger/ginger-monaka.hex  HEX/ginger-monaka.hex
	cp ginger/ginger-monaka.eep  HEX/ginger-monaka.eep
	cp pepper/pepper-monaka.hex  HEX/pepper-monaka.hex
	cp pepper/pepper-monaka.eep  HEX/pepper-monaka.eep
	cp arduino/arduino-monaka.hex HEX/arduino-monaka.hex
	cp arduino/arduino-monaka.eep HEX/arduino-monaka.eep

clean:
	-rm *~
	( cd ginger ; make clean)
	( cd pepper ; make clean)
	( cd arduino ; make clean)
