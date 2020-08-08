#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <util/delay_basic.h>

#define HIGH 2
#define LOW 1

void one() {
  PORTB &= ~(1 << PB1);
  _delay_loop_1(LOW);
  PORTB |= (1 << PB1);
  _delay_loop_1(HIGH);
}

void zero() {
  PORTB &= ~(1 << PB1);
  _delay_loop_1(HIGH);
  PORTB |= (1 << PB1);
  _delay_loop_1(LOW);
}

void reset_low() {
    PORTB &= ~(1 << PB1);
  _delay_us(500);
}

void all_ones() {
 int i =0;
  for (i=0;i<24;i++) one();
}  

#define ZERO \
  "out 0x18, 16\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "out 0x18, 17\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n"

#define ONE \
  "out 0x18, 16\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "nop\n" \
  "out 0x18, 17\n" \
  "nop\n" \
  "nop\n" \
  "nop\n"

void all_red() {
  //char pin = 2;
  //char off = 0;
  asm("ldi 16, %0\n"
      "ldi 17, %1\n"
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO

      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO      
      :
      : "M" (2), "M" (0) : "%r16", "%r17");
}  



void fast_led(char g, char r, char b) {
    asm("ldi 16, %0\n"
      "ldi 17, %1\n"
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO

      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO      
      :
      : "M" (2), "M" (0) : "%r16", "%r17");
}

void all_green() {
  //char pin = 2;
  //char off = 0;
  asm("ldi 16, %0\n"
      "ldi 17, %1\n"
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE
      ONE

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO      
      :
      : "M" (2), "M" (0) : "%r16", "%r17");
}  

void all_off() {
    asm("ldi 16, %0\n"
      "ldi 17, %1\n"
 
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO

      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO
      ZERO      
      :
      : "M" (2), "M" (0) : "%r16", "%r17");
}

int main(void) {
  DDRB = 0xFF; // PORTB is output, all pins
  PORTB = 0x00; // Make pins low to start
#if 0
  int direction = 1;
  int num_lit = 1;
  for (;;) {
    reset_low();
    int i = 0;
    for (i = 0; i < num_lit; i++) {
      all_ones();
    }
    for (;i<4;i++) {
      all_zeros();
    }
    reset_low();
    _delay_ms(100);
    num_lit += direction;
    if (num_lit == 15) direction = -1;
    if (num_lit == 0) direction = 1;
  }
  #endif
  for(;;) {
    reset_low();
    all_red();
    all_green();
    all_off();
    all_red();
    all_green();
    all_off();
    reset_low();
    _delay_ms(1000);
  }
  return 0;
}
