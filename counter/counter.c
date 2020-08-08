#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int cnt = 0;

void Setup() {
  DDRB = (1 << PB0) | ( 1 << PB1) | ( 1 << PB2);
  GIMSK = (1 << PCIE);
  PCMSK = (1<<PB3) | (1 << PB4);
  TCCR1 = (1 << CTC1);
  OCR1C = 100;
  TIMSK = (1 << OCIE1A);
  sei();
}

void Update(char val) {
  PORTB = (PORTB & ~0x7) | val;
}



void Inc() {
  cnt++;
  Update((cnt >> 2) & 0x7);
}

ISR(TIMER1_COMPA_vect) {
  Inc();
}


ISR(PCINT0_vect) {
  if (PINB & (1 << PB3)) {
    TIMSK ^= (1 << OCIE1A);
  } 
	 
  if (PINB & (1 << PB4)) {
    if (TIMSK & (1<<OCIE1A)) {
      GTCCR |= (1 << TSM);
      char bits = TCCR1 & 0x0f;
      char left = TCCR1 & 0xf0;
      bits++;
      TCCR1 = left | (bits & 0xf);
      GTCCR &= ~(1 << TSM);
    } else {
      Inc();
    }
  }
}

int main(void) {
  Setup();
  for (;;) {  }
  return 0;
}
