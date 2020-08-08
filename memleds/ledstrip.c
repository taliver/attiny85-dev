#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void reset_low() {
    PORTB &= ~(1 << PB1);
  _delay_us(500);
}

#define LOAD(n) "mov 17, %" #n  " \n" 


#define FIRSTBIT \
  "out 0x18, 16\n" \
  "nop \n" \
  "nop \n"	  \
  "sbrs 17, 0 \n" \
  "out 0x18, __zero_reg__\n" \
  "nop\n" \
  "nop\n" \
  "out 0x18, __zero_reg__\n" \
  "lsr 17\n" \
  "nop\n" \
  "nop\n"  

#define BIT \
  "nop \n" \
  FIRSTBIT

void fast_leds(char g, char r, char b) {
    asm(
	"ldi 16, %0\n"
        LOAD(1)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	LOAD(2)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
        LOAD(3)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT	
	:
	: "M" (2), "r" (g), "r"(r), "b"(b) : "%r16", "%r17");
}

int button_pushed = 0;
int toggle = 0;
ISR(PCINT0_vect) {
  sleep_disable();
  button_pushed = !(PINB & (1 << PB0));
  if (button_pushed) toggle = !toggle;
}


void sparkle() {
  int s;
  for (s = 0; s < 50; s++) {
    switch(s%5) {
    case 0: fast_leds(0x00, 0x00, 0x44); break;
    case 1: fast_leds(0x00, 0x88, 0x88); break;
    case 2: fast_leds(0x33, 0x00, 0x66); break;
    case 3: fast_leds(0xFF, 0xFF, 0xFF); break;
    case 4: fast_leds(0x00, 0x00, 0x00); break;
    }
    reset_low();
    _delay_ms(10);
  }
}
  

int main(void) {
  PORTB = (1 << PB0); // Makepull-up pins low, except for 1, to make it pull-up.
  DDRB = (1 << PB1) | (1 << PB2); // PORTB is output, all pins
  PORTB = (1 << PB0);
  GIMSK = (1 << PCIE);
  PCMSK = (1 << PB0);
  GIFR = 0;
  MCUSR = (1 << SM1); // Enable sleep mode.
  sei();
  

  for(;;) {
    //    while (button_pushed) {
    while(1) {
      PORTB |= (1 << PB2);
      int i,j;
      for (i = 15; i > 0; i--) {
	for (j = 0; j < i; j++) {
  	  fast_leds(0x00, 0x00, 0x00);
	}
        fast_leds(0x88, 0x88, 0x88);
	reset_low();
	_delay_ms(100);
      }
      sparkle();
    }
    int cnt = 0;
    while (!button_pushed) {
      reset_low();
      cnt++;
      if (cnt > 100) {
	int j = 0;
	for (j = 0; j < 15; j++) {
	  fast_leds(0x00, 0x00, 0x00);
	}
	if (cnt > 150) {
	  reset_low();
	  sleep_enable();
	  PRR = (1 << PRADC) | (1 << PRUSI) | ( 1 << PRTIM0) | (1 << PRTIM1);
	  sei();
	  sleep_cpu();
	}
      }
    }
  }
    return 0;
}
