#define F_CPU 16000000L
#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


  int interval = 100;
  int c = 0;
int button_pushed;

ISR(PCINT0_vect) {
  sleep_disable();
  button_pushed = !(PINB & (1 << PB0));
  if (button_pushed) {
    PORTB |= (1<<PB1);
  } else {
    PORTB &= ~(1<<PB1);    
  }
  interval = 200;
  c = 0;
}

int main(void) {
  PORTB = (1<<PB0);
  DDRB = (1<<PB1); // P1 is output, 
  GIMSK = (1 << PCIE);
  PCMSK = (1 << PB0);
  GIFR = 0;
  MCUSR = (1 << SM1);
  for (c=0;;c++) {
    if (!button_pushed) {
      PORTB ^= (1<<PB1);
      if (interval > 100) interval -=5;
      if (c>5) {
	PORTB &= ~(1 << PB1);
      sleep_enable();
      sei();
      sleep_cpu();
      interval = 50;
      c = 0;
      }
    } 
    _delay_ms(interval); // wait some time
  }
  return 0;
}
