
#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// https://www.avrfreaks.net/forum/pwm-attiny85-0
void setup() {
  DDRB |= _BV(PB0);

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
   (1 << ADLAR); // Put the first 8 bits into ADCH.

 TCCR0A = (1 << COM0A1) // PWM Pin on PB1
   | (1 << WGM01)       // 01/00 means PWM in fast mode.
   | (1 << WGM00);
 TCCR0B = _BV(CS02); // Divide clock by 64
 OCR0A = 30;
 sei();
}
// At prescaler of 256
// 60 = ~2.0ms
// 50 = ~1.6ms
// 40 = ~1.3ms
// 30 = ~1.0ms


ISR(ADC_vect) {
  int v = ADCH >> 3;
  OCR0A = v + 30;
}

int main(void) {
  setup();
  for(;;) {}
  return 0;
}
