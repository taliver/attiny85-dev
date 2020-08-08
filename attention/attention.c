#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>
#include <math.h>
#include "memleds.h"

// Pins:
// PB0: PWM for motor
// PB1: Output for lights
// PB2: Button for attention
// PB3: Button for emphasis


void setup() {
  DDRB = (1 << PB0) | (1<< PB1);
  
  TCCR0A =
    _BV(COM0A1)
    // | _BV(WGM01)
    | _BV(WGM00)
    ;

  TCCR0B =
    _BV(CS02)
    //| _BV(CS01)
    //| _BV(CS00)
    // | _BV(WGM02)
    ;

  
  OCR0A = 10; // Zero degrees.  240 is 100 degrees.

  TCCR1 =
    (1 <<CS13) |
    (1 << CS12) |
    (1 << CS10);
  
  TIMSK =  (1 << TOIE1);
  
  GIMSK = (1 << PCIE);
  PCMSK = (1<<PB2) | (1 << PB3) | (1 << PB4);

  sei();
}



#define _R  0x00, 0xFF, 0x00
#define _G  0xFF, 0x00, 0x00
#define _B  0x00, 0x00, 0xFF
char kLightBar[100] = {
  _G, _G, _G, _G, _G, _B, _B, _B, _B, _B, _R, _R, _R, _R, _R,
  _B, _B, _B, _B, _B, _G, _G, _G, _G, _G, 
};
char kZero[46] = {0};

volatile int light_state = -1;

void DoLightState() {
  if (light_state == -1) {
    memleds(kZero, 15, PB1);
  } else {
    memleds(kLightBar + light_state, 15, PB1);
    light_state++;
    if (light_state > 20) light_state = 0;
  }
  reset_low(PB1);
}


// Must be volatile because interrupt is changing global state.
volatile char pushed_PB2 = 0;
volatile char pushed_PB3 = 0;
volatile char pushed_PB4 = 0;

volatile int motor_step = 0;


void pushed(int pin) {
  switch(pin) {
  case PB2:
    if (!pushed_PB2) light_state = 0;
    DoLightState();
    break;
  case PB3:
    motor_step = 1;
    break;
  case PB4:
    motor_step = -1;
    break;
  }
}

void released(int pin) {
  switch(pin) {
  case PB2:
    light_state = -1;
    DoLightState();
    break;
  case PB3:
  case PB4:
    motor_step = 0;
    break;
  }
}

ISR(PCINT0_vect) {
#define PUSHBUTTON(button) if (PINB & _BV(button) && !pushed_##button) { \
    pushed(button);\
    pushed_##button = 1;\
  }\
  \
  if (!(PINB & _BV(button)) && pushed_##button) {\
    released(button);\
    pushed_##button = 0;\
  }

  PUSHBUTTON(PB2);
  PUSHBUTTON(PB3);
  PUSHBUTTON(PB4);
#undef PUSHBUTTON    
}

#if 0
const char motor_points[] = {
  120, 130, 150, 180, 200, 220, 230, 240
};
#endif

// For phased corrected WS01 pick, the fequency is:
// 8MHz/(510*256) [ the 510 is a constant from the datasheet, the 256
// from the clock selection ]
// This leads to a frequency of 61.2745098 Hz
// Which means a period of 16.32 ms
// Which means that each increment of OCR0A is 63.75 microseconds
// So for 1000 microseconds, we need to have 15.686 units.
// For 2000, it's 31.3725...



//const char motor_points[] = {
//  16, 31
//};

//const char motor_points[] = {
//  0, 5, 10, 16, 31, 40, //60, 80, 100
//};

const char motor_points[] = {
  0, 5, 40, //60, 80, 100
};
volatile int motor_index = 0;

void MotorStep() {
  if (motor_step == 0) return;
  motor_index += motor_step;
  if (motor_index < 0) motor_index = 0;
  if (motor_index >= sizeof(motor_points)) motor_index = sizeof(motor_points) - 1;
  OCR0A = motor_points[motor_index];
}
  
ISR(TIMER1_OVF_vect) {
  DoLightState();
  MotorStep();
}

int main() {
  setup();
  for(;;)  {
  }
  return 0;
}
