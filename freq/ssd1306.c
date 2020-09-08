#include "i2cmaster.h"
#include "ssd1306.h"

#include <avr/interrupt.h>
#include "oled_fonts.h"

#define BUFFERSIZE (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8)

//int buffer[10];

void ssd1306_command(unsigned char c) {
  // I2C
  // 0x3c = 0011 1100
  //  unsigned int control = 0x00;	// Co = 0, D/C = 0
  cli();
  i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
  i2c_write(0x00);
  i2c_write(c);       // write value c
  i2c_stop();
  sei();
}

// Need to be able to display value.
// Let's make this easy for us.
// We'll start with just a number, and work up.

void ssd_page_commands(unsigned char cmd, unsigned char start, unsigned char end) {
  cli();
  i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
  i2c_write(0x00);
  i2c_write(cmd);       // write value c
  i2c_write(start);
  i2c_write(end);
  i2c_stop();
  sei();
}

void ssd1306_cmd_data(unsigned char cmd, unsigned char data) {
  cli();
  i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
  i2c_write(0x00);
  i2c_write(cmd);       // write value c
  i2c_write(data);
  i2c_stop();
  sei();
}

int position = 0;

void ssd1306_display(int line, char *string) {
  ssd_page_commands(SSD1306_COLUMNADDR, 0, 127);
  ssd_page_commands(SSD1306_PAGEADDR, 0, 3);
  ssd1306_command(0xB0 + line);
  
  int i = 0; // which character.
  char c = string[i];
  char char_lines[5];
  while(c) {
    memcpy_P(char_lines, font + (c*5), 5); 
    int j = 0;
    i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
    i2c_write(0x40);       // write value No continuation, data byte
    i2c_write(char_lines[0]);
    i2c_stop();
    for (j = 1; j < 5; j++) {
      i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
      i2c_write(0x40);       // write value No continuation, data byte
      i2c_write(char_lines[j]);
      i2c_stop();
    }
    i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
    i2c_write(0x40);       // write value No continuation, data byte
    i2c_write(0);
    i2c_stop();
    i++;
    c = string[i];
  }
  // Blank the rest of the line.
  for (i = i*6; i < 128; i++) {
    i2c_start_wait(0x78 + I2C_WRITE);    // write address = 0x3c
    i2c_write(0x40);       // write value No continuation, data byte
    i2c_write(0);
    i2c_stop();
  }
  
} 

// Init SSD1306
void ssd1306_begin() {
  // Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);	// 0xAE

  ssd1306_cmd_data(SSD1306_SETDISPLAYCLOCKDIV, 0x80);	// 0xD5
  
  ssd1306_cmd_data(SSD1306_SETMULTIPLEX, SSD1306_LCDHEIGHT - 1);// 0xA8
  
  ssd1306_cmd_data(SSD1306_SETDISPLAYOFFSET, 0);	// 0xD3
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);	// line #0
  ssd1306_cmd_data(SSD1306_CHARGEPUMP, 0x14);	// 0x8D
 
  ssd1306_cmd_data(SSD1306_MEMORYMODE, 0);	// 0x20
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);
  
  ssd1306_cmd_data(SSD1306_SETCOMPINS, 2);	// 0xDA
  ssd1306_cmd_data(SSD1306_SETCONTRAST, 0x8F);	// 0x81
  
  ssd1306_cmd_data(SSD1306_SETPRECHARGE, 0xF1);	// 0xd9

  ssd1306_cmd_data(SSD1306_SETVCOMDETECT, 0x40);	// 0xDB

  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);	// 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);	// 0xA6
  
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);


  ssd1306_command(SSD1306_DISPLAYON);	// --turn on oled panel
}
