#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void Setup() {
  DDRB = (1 << PB0) | ( 1 << PB1) | ( 1 << PB2);
  GIMSK = (1 << PCIE);
  PCMSK = (1<<PB3) | (1 << PB4);
  sei();
}


ISR(PCINT0_vect) {
  if (PINB & (1 << PB3)) {
    PORTB |= (1 << PB0);
  } else {
    PORTB &= ~(1<< PB0);
  }
}

int main(void) {
  Setup();
  for (;;);
  return 0;
}
