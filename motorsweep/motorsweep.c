#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// https://www.avrfreaks.net/forum/pwm-attiny85-0
void setup() {
  DDRB |= _BV(PB0) | _BV(PB1) | _BV(PB2);
  DDRB |= (1 << DDB1);

 TCCR0A = (1 << COM0A1) // PWM Pin on PB1
   | (1 << WGM01)       // 01/00 means PWM in fast mode.
   | (1 << WGM00);
 TCCR0B = (1 << CS00); // Divide clock by 256.
}

void set_direction(int dir) {
  if (dir > 0) {
    PORTB |= _BV(PB1);
    PORTB &= ~_BV(PB2);
  } else if (dir < 0) {
    PORTB |= _BV(PB2);
    PORTB &= ~_BV(PB1);
  } else {
    PORTB &= ~_BV(PB1);
    PORTB &= ~_BV(PB2);
  }
}

void sweep(int from, int to) {
  
  if (from == to) return;
  int p;
  if (to > from) {
    for (p = from; p < to; p+=5) {
      OCR0A = p;
      _delay_ms(80);
    }
  } else {
    for (p = from; p > to; p-=5) {
      OCR0A = p;
      _delay_ms(80);
    }
  }
}

int main(void) {
  setup();
  for (;;) {
    set_direction(1);
    sweep(0,50);
    sweep(50,0);
    set_direction(0);
    _delay_ms(50);
    set_direction(-1);
    sweep(0,50);
    sweep(50,0);
    set_direction(0);
    _delay_ms(50);
  }
    
  return 0;
}
