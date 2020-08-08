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
#include <math.h>

// Pins:
// PB0: Timing checks.
// PB1: Output for lights
// PB3: Unused.
// PB4: Input for ADC.

void reset_low(int port_bit);
void memleds(char *base, int num_leds, int port_bit);

#define _R  0x00, 0x02, 0x00
#define _G  0x02, 0x00, 0x00
#define _B  0x00, 0x00, 0x02
char kLightBar[60] = {
  _G, _G, _G, _G, _G, _B, _B, _B, _B, _B, _R, _R, _R, _R, _R
};
char kZero[46] = {0};


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
    (0 << ADLAR); // Put the first 8 bits into ADCL.


  TCCR1 =
    (1 << CTC1) | // Clear timer on compare match.
    //(1 << COM1A0) | // Toggle the OC1A output line. (PB1)
    (0 <<CS13) |
    (1 << CS10);
  
  TIMSK = (1 << OCIE0A);

  sei();
}


int HighBit(long value) {
#define BIT_SET(b)   if (value & ((long)1 << b)) return b;
  if (value < 1) return 0;
  BIT_SET(30);
  BIT_SET(29);
  BIT_SET(28);
  BIT_SET(27);
  BIT_SET(26);
  BIT_SET(25);
  BIT_SET(24);
  BIT_SET(23);
  BIT_SET(22);
  BIT_SET(21);
  BIT_SET(20);
  BIT_SET(19);
  BIT_SET(18);
  BIT_SET(17);
  BIT_SET(16);
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

#ifdef DEBUG
char bits[60];
void ShowValue(long value) {
  int b = 0;
  for (;b<16;b++) {
    if (value & (long)(1 << b)) {
      bits[b * 3] = 0x08;
      bits[b * 3 + 1] = 0;
      bits[b * 3 + 2] = 0;
    } else {
      bits[b * 3] = 0;
      bits[b * 3 + 1] = 0;
      bits[b * 3 + 2] = 0;
    }
  }
  memleds(bits, 45, PB2);
  reset_low(PB2);
}
#endif


int old_level;
void LightBar(int level) {
  if (level == old_level) return;
  if (level > 15) level = 15;
  if (level < 1) level = 0;
  old_level = level;
  memleds(kZero, 15, PB2);
  reset_low(PB2);  
  memleds(kLightBar, level, PB2);
  reset_low(PB2);
}

long sum = 0;
#define DIVSHIFT 4
#define SAMPLES (1 << DIVSHIFT)
int sample = SAMPLES;

    
float level = 60.0;
float kCharge = 0.8;
float kDischarge = 0.99;


int numLeds(unsigned int val) {
  if ((val > level) && (level < 16000)) {
    level = level + (kCharge * (val - level));
    //level = (level * (1 - kCharge)) + (val * kCharge);
  } else {
    level = level * kDischarge;
  }
  int level_bits = (int)level; 
  int hb = HighBit(level_bits);
  return (hb * 2) + (level_bits >> (hb - 2));
}

// From https://www.johndcook.com/blog/standard_deviation/
int m_oldM = 679;
int m_oldS = 0;
int m_newM = 679;
int m_newS = 0;
int m_n = 0;

ISR(ADC_vect) {
  m_n++;
  int low = ADCL;
  int high = ADCH &0x3;
  int val = (high << 8) + low;
  m_newM = m_oldM + (val - m_oldM)/m_n;
  m_newS = m_oldS + (val - m_oldM)*(val - m_newM);

  // set up for next iteration
  m_oldM = m_newM; 
  m_oldS = m_newS;
  if (m_n == 16) {
#ifdef DEBUG
    ShowValue(m_newS>>4);
#else
    int leds = numLeds(m_newS >> 4);
    LightBar(leds);
#endif
    m_n = 0;
    m_oldS = 0;
  }
}



EMPTY_INTERRUPT(TIMER0_COMPA_vect);

int main() {
  setup();
  for(;;)  {
  }

}
