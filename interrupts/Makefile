MCU=attiny85
AVRDUDEMCU=t85
CC=/usr/bin/avr-gcc
CFLAGS=-Os -Wall -mcall-prologues -mmcu=$(MCU)
OBJ2HEX=/usr/bin/avr-objcopy
AVRDUDE=avrdude
TARGET=int

all :
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
	$(OBJ2HEX) -R .eeprom -O ihex $(TARGET) $(TARGET).hex
	rm -f $(TARGET)

install : all
	gpio -g mode 22 out
	gpio -g write 22 0
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P /dev/spidev0.0 -c linuxspi -b 10000 -U flash:w:$(TARGET).hex
	sudo gpio -g write 22 1

noreset : all
	sudo $(AVRDUDE) -p $(AVRDUDEMCU) -P /dev/spidev0.0 -c linuxspi -b 10000 -U flash:w:$(TARGET).hex


