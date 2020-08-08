#define F_CPU 1000000L
#include <avr/io.h>
#include <util/delay.h>

char spiTransmit(char dataOut){
  USIDR = dataOut;
  USISR = (1 << USIOIF);
  while(!(USISR & (1 << USIOIF)));
  return USIDR;
}

int main(void) {
  USICR = (1<<USIWM0)|(1<<USICS1);
  DDRB = (1 << PB1) | (1 << PB4); 
  PORTB &= ~(1 << PB4);
  spiTransmit(0);
  for (;;) {
    int i = 0;
    for (i = 32; i<127; i++) {
      spiTransmit(i);
      _delay_ms(100);
      PORTB |= (1 << PB4);
      _delay_ms(100);
      PORTB &= ~(1 << PB4);
      
    }
  }
}
