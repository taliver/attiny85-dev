#define F_CPU 1000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// https://www.avrfreaks.net/forum/pwm-attiny85-0
void setup() {
  DDRB |= _BV(PB0) | _BV(PB1);
  DDRB |= (1 << DDB1) | ( 1<<DDB2) | (1 << DDB3);

  ADCSRA =
    (1 << ADEN) |  // ADC Enable
    (1 << ADATE) | // Start a conversion on interrupt
    (1 << ADIE ) | // Interrup enable
    (1 << ADSC)  | // And start now.
    (1 << ADPS2) |   // Next three are divisors.
    (1 << ADPS1) |
    (0 << ADPS0);

  ADCSRB = 0; // Free running mode.
 ADMUX =
   (1 << MUX1) | // Single ended input, PB4.
   (1 << ADLAR); // Put the first 8 bits into ADCH.

 TCCR0A = (1 << COM0A1) // PWM Pin on PB1
   | (1 << WGM01)       // 01/00 means PWM in fast mode.
   | (1 << WGM00);
 TCCR0B = (1 << CS00); // Divide clock by 256.

 sei(); // Enable Interrupts.
}

ISR(ADC_vect) {
  //  PORTB |= _BV(PB1);
  OCR0A = (ADCH); // Read measurement into PWM register.
  //PORTB &= ~_BV(PB1);
  //ADCSRB |= (1 << ADSC);
}


int main(void) {
  setup();
  for(;;) {
  //    _delay_ms(100);
  //  PORTB |= _BV(PB2);
    _delay_ms(100);
    //  PORTB &= ~_BV(PB2);
  }
  return 0;
}
