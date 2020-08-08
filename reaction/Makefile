MCU=attiny85
AVRDUDEMCU=t85
CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=$(MCU)
OBJ2HEX=/usr/bin/avr-objcopy
AVRDUDE=avrdude
TARGET=reaction
AVR_FLAGS=-p $(AVRDUDEMCU) -P /dev/spidev0.0 -c linuxspi -b 10000

all :
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex
	rm -f $(TARGET)

install : all
	gpio -g mode 22 out
	gpio -g write 22 0
	-sudo $(AVRDUDE) -p $(AVRDUDEMCU) $(AVR_FLAGS) -U flash:w:$(TARGET).hex
	sudo gpio -g write 22 1

noreset : all
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P /dev/spidev0.0 -c linuxspi -b 10000 -U flash:w:$(TARGET).hex


reset:
	gpio -g write 22 1
	sudo rmmod spi_bcm2835
	sudo modprobe spi_bcm2835

fuses:
	gpio -g write 22 0
	sudo avrdude $(AVR_FLAGS) -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
	gpio -g write 22 1
