#define F_CPU 16000000UL
#include "i2cmaster.h"
#include "ssd1306.h"
#include <avr/io.h>
#include <stdlib.h>

#include <util/atomic.h>
#include <util/delay.h>


int main(void) {
  DDRB = _BV(PB1);
  i2c_init();
  ssd1306_begin();
  ssd1306_display(1, "This is line 1.");
  ssd1306_display(2, "This is line 2");
  ssd1306_display(3, "This is line 3");
  int i = 0;
  char num[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for(i = 0;;i++) {
    itoa(i, num, 10);
    ssd1306_display(0, num);
    _delay_ms(100);
  }
  return 0;
}

// Notes:
// The problem seems to be that the i2c pullup isn't actually pulling up.
// This is leading to the write never completing.
// And this means the commands never actually progress.

// We're tryng 5V for the pullup and device.  I'm now going to try a pullup of 3.3V,,
// That didn't work.
// This is starting to feel like I'm not at the right address...
// Nope,, cnfirmed 0x3c.
// Gotta be timing.
// Let's try to capture a "normal" I2Cinteraction.

// Okay, we ended with determining that I hadn't set the #define for the frequency in the
// i2cmaster file, so I needed to adjust and recompoile.  Now we're ready for the next debgging, of why we're not seeing anything ack.




