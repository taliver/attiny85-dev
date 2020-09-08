#define F_CPU 8000000UL
#include "i2cmaster.h"
#include "ssd1306.h"
#include <avr/io.h>
#include <stdlib.h>

#include <util/atomic.h>
#include <util/delay.h>
#include <avr/interrupt.h>


// Process:
// 1. Wait for interrupt.
// 2. Count interrupts.
// Success.  Now adding timer.


// PB0 - Counter.
// PB1
// PB2
// PB3 - SDA
// PB4 - SCL
// PB5 


volatile int cnt = 0;
volatile unsigned char level = 0;
const char kFastTimerEnable =  _BV(CS00);
const char kFastTimerDisable = 0;

void setup() {
  TCCR0A =
    // _BV(COM0A1) |
    // _BV(COM0A0) |
    // _BV(COM0B1) |
    // _BV(COM0B0) |
    //  _BV(WGM01) |
    // _BV(WGM00) |
    0;

  // Prescaler now says /1024, which should be 8kHz.
  TCCR0B = kFastTimerEnable;
      // _BV(FOC0A) |
      // _BV(FOC0B) |
      // _BV(WGM02) |
      // _BV(CS02) |
      // _BV(CS01) |
      // _BV(CS00) |
      // 0;
  
   
  TCCR1 =
    _BV(CTC1) |
    // _BV(PWM1A) |
    // _BV(COM1A1) |
    // _BV(COM1A0) |
    // (1 <<CS13) |
    // (1 << CS12) |
    // (1 << CS11) | 
    (1 << CS10) |
    0;

  TIMSK =  _BV(TOIE0);
  GIMSK |= _BV(PCIE);        
  PCMSK = _BV(PB0);
  i2c_init();
  sei();
}

volatile unsigned long int cycles = 0;
ISR(TIM0_OVF_vect) {
  cycles++;
}


volatile unsigned long int measured = 0;

ISR(PCINT0_vect) {
  if (!level) { // Act on rising edge.
    measured = (cycles << 8) + TCNT0;
    TCNT0 = 0;
    cycles = 0;
  }
  level = !level;
}

int main(void) {
  setup();
  ssd1306_begin();
  char num[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  ssd1306_display(0, "Test 9");
  ssd1306_display(3,"");
  for(;;) {
    ultoa(measured, num, 10);
    ssd1306_display(1, num);
    double freq = (double) (F_CPU) / (double) measured;
    dtostrf(freq, 7, 5, num);
    ssd1306_display(2, num);
    ultoa(cycles, num, 10);
    ssd1306_display(3, num);
    _delay_ms(1000);
  }
  return 0;
}

