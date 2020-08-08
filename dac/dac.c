#define F_CPU 16000000L
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void setup() {

  DDRB = _BV(PB0); // Pin 1, output.
  TCCR0B =  _BV(CS00);
  TCCR0A =  _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  ADCSRA =
    (1 << ADEN) |  // ADC Enable
    (1 << ADATE) | // Start a conversion on interrupt
    (1 << ADIE ) | // Interrup enable
    (1 << ADSC)  | // And start now.
    (1 << ADPS2) |   // Next three are divisors.
    (1 << ADPS1) |
    (1 << ADPS0);
  ADCSRB = 0; // Free running mode.
  ADMUX =
    (1 << MUX1) | // Single ended input, PB4.
    (1 << ADLAR); // Put the first 8 bits into ADCL.
  sei();
}


ISR(ADC_vect) {
  int s = rand() & 0x1f;
  if (ADCH < OCR0A) {
    OCR0A-=s;
  } else {
    OCR0A+=s;
  }
}

int main(void) {
  setup();
  for (;;);
  return 0;
}
