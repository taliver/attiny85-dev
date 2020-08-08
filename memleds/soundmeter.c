// So, we're tryingh to look at the RMS of the voltage as it's read.
// The sound sensor puts ut a value of 0.67 nominally, and makes a wave
// around that value.

// So, what we will do is a little math between samples.

// We'll sample at a rate of 10kHz.  Then, for each 10ms, I'll send out
// the total value seen.

// And some more math here:
// At 10,000 samples per second, in 10ms I should see 100 samples.

// I'm planning on scaling the value between 0 and 1.1V.  This means
// I should see 10 bit values fluctuating around 623.  Or 8-bit values
// around 155.

// This, in turn, means that there could be a maximum of 24336 for one reading.
// Or 2 433 600 for 100 readings.  This is basically three bytes...
// I'll always prefix with FF to make sure I can see them and interpret them
// correctly.


// So, steps for this are:
// One, make a timer, confirm that I am seeing 10k pulse rate. (Check).
// Two, write 2 bytes to SDI, and make sure I can read the values. (Check, but failed.)
//    Basically, Master reading interrups things, causes issues.
//    And, since I'm not really going to be using SPI for the real system,
//    I'm punting for now.  Maybe when I get closer to the robot, we'll
//    revisit.
// Three, connect the ADC, sample with attched to pot.
// Four, Add in sound sensor.

#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

// Pins:
// PB0: Timing checks.
// PB1: Output for lights
// PB3: Unused.
// PB4: Input for ADC.

void setup() {
  DDRB = (1 << PB0) | (1<< PB1) | (1<<PB2);
  OCR0A = 100; // 100 microseconds. One count per microsecond.
  TCCR0A = (1 << WGM01);
  TCCR0B = (1 << CS01);
  OCR1C = 75;

   ADCSRA =
    (1 << ADEN) |  // ADC Enable
    (1 << ADATE) | // Start a conversion on interrupt
    (1 << ADIE ) | // Interrup enable
    (1 << ADPS2) |   // Next three are divisors.
    (1 << ADPS1) |
    (1 << ADPS0);

  ADCSRB = (1 << ADTS1) | (1 << ADTS0); // Timer Match A.
  
  ADMUX =
    (1 << REFS1) |  // Internal 1.1V for reference.
    (1 << MUX1) | // Single ended input, PB4
    (1 << ADLAR); // Put the first 8 bits into ADCL.


  TCCR1 =
    (1 << CTC1) | // Clear timer on compare match.
    //(1 << COM1A0) | // Toggle the OC1A output line. (PB1)
    (0 <<CS13) |
    (1 << CS10);
  
  TIMSK = (1 << OCIE0A);

  sei();
}

int delay;

void reset_low(int port_bit) {
    PORTB &= ~(1 << port_bit);
    _delay_loop_2(800);

}

// Move the value from the register and r17 (allows the variable).
#define LOAD(n) "mov 17, %" #n  " \n" 

#if 0
// The original one, modified to have the port bit set correctly.
// First command sets the port to the value that is in register 16.
// Register 16 is assumed to be set with the the port bits for "On"
// While register 20 is the port bits for "Off".
#define FIRSTBIT \
  "out 0x18, %1\n" \
  "nop \n" \
  "nop \n"	  \
  "sbrs 17, 0 \n" \
  "out 0x18, %0\n" \
  "nop\n" \
  "nop\n" \
  "out 0x18, %0\n" \
  "lsr 17\n" \
  "nop\n" \
  "nop\n"  
#endif



#if 0
// This worked.  However, the bits are being set in reverse order.
// Timing experiment 1: removing a nop to make the bit faster.
#define FIRSTBIT \
  "out 0x18, %1\n" \
  "nop \n"	  \
  "sbrs 17, 0 \n" \
  "out 0x18, %0\n" \
  "nop\n" \
  "nop\n" \
  "out 0x18, %0\n" \
  "lsr 17\n" \
  "nop\n" \
  "nop\n"  
#endif

// Experiment 1: Setting the bits correcly.
#define FIRSTBIT \
  "out 0x18, %1\n" \
  "nop \n"	  \
  "sbrs 17, 7 \n" \
  "out 0x18, %0\n" \
  "nop\n" \
  "out 0x18, %0\n" \
  "lsl 17\n" \
  "nop\n" \
  "nop\n"  



#define BIT \
  "nop \n" \
  FIRSTBIT

void fast_leds(char r, char g, char b, char port_bit) {
  char port_on = PORTB | (1 << port_bit);
  char port_off = PORTB & ~(1 << port_bit);
    asm(
        LOAD(2)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	LOAD(3)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
        LOAD(4)
        FIRSTBIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT
	BIT	
	:
	: "r"(port_off), "r"(port_on), "r" (g), "r"(r), "r"(b): "%r17");
}

void LightBar(int level) {
  cli();
  switch(level) {
  case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 1:
    fast_leds(0,G,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 2:
    fast_leds(0,G,0,2);
    fast_leds(0,G,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 3:
    fast_leds(0,G,0,2);
    fast_leds(0,G,0,2);
    fast_leds(0,G,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 4:
    fast_leds(0,G,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
      case 0:
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    fast_leds(0,0,0,2);
    break;
  }
  reset_low(2);
  sei();
  
    
    
}

int HighBit(int value) {
#define BIT_SET(b)   if (value && (1 << b)) return b;
  if (value < 1) return 30;
  BIT_SET(15);
  BIT_SET(14);
  BIT_SET(13);
  BIT_SET(12);
  BIT_SET(11);
  BIT_SET(10);
  BIT_SET(9);
  BIT_SET(8);
  BIT_SET(7);
  BIT_SET(6);
  BIT_SET(5);
  BIT_SET(4);
  BIT_SET(3);
  BIT_SET(2);
  BIT_SET(1);
  return 0;
#undef BIT_SET
}


int sample = 1024;
long sum = 0;

ISR(ADC_vect) {
  int high = ADCH - 156;
  int sq = high * high;
  sum += sq;
  sample--;
  if (sample == 0) {
    sample = 1024;
    int avg_sq = sum >> 10;
    sum = 0;
    //    LightBar(HighBit(avg_sq));
  }
}


EMPTY_INTERRUPT(TIMER0_COMPA_vect);
int cnt;
int main() {
  setup();
  char i =0;
  for(;;)  {
    //LightBar(i & 0xf);
    cli();
    for (i = 8; i != 0; i--) {
      switch(i) {
      case 8: fast_leds(0x0f, 0x00, 0x00, 2); break;
      case 7: fast_leds(0x00, 0x0f, 0x00, 2);break;
      case 6: fast_leds(0x00, 0x00, 0x0f, 2);break;
      case 5: fast_leds(0x0f, 0x0f, 0x00, 2);break;
      case 4: fast_leds(0x0f, 0x00, 0x0f, 2);break;
      case 3: fast_leds(0x0f, 0x0f, 0x0f, 2);break;
      case 2: fast_leds(0x0f, 0x0f, 0x0f, 2);break;
      case 1: fast_leds(0x0f, 0x0f, 0x0f, 2);break;
      }
    }
    reset_low(2);
    sei();
    for (cnt =0; cnt < 100; cnt++) {
    _delay_loop_2(10000);
    }
    if (i&0x01) { PORTB |= (1<< PB0); } else {
      PORTB &= ~(1<<PB0);
    }
  }

}
