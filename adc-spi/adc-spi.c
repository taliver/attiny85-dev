#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


char spiTransmit(char dataOut){
  USIDR = dataOut;
  USISR = (1 << USIOIF);
  while(!(USISR & (1 << USIOIF)));
  return USIDR;
}

void setup() {
  USICR = (1<<USIWM0)|(1<<USICS1);
  DDRB = (1 << PB1) | (1 << PB4); 

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

//char adc_val;

ISR(ADC_vect) {
  //adc_val = ADCH;
  spiTransmit(ADCH);
}


int main(void) {
  setup();
  spiTransmit(0);
  for(;;) {
    //    _delay_ms(10);
    //spiTransmit(adc_val);
  }
}
