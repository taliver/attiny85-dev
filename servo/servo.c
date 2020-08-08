#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>
#include <math.h>

// Pins:
// PB0: PWM for motor
// PB1: Output for lights
// PB2: Button for attention
// PB3: Button for emphasis


void setup() {
  DDRB = (1 << PB0) | (1<< PB1);
  
  //TCCR0B =  _BV(CS01) | _BV(CS00);
  TCCR0B =  _BV(CS00) | _BV(CS01);
  TCCR0A =  _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  
  OCR0A = 40; // Zero degrees.  240 is 100 degrees.

  TCCR1 =
    (1 <<CS13) |
    (1 << CS12) |
    (1 << CS10);
  
  TIMSK =  (1 << TOIE1);
  
  GIMSK = (1 << PCIE);
  PCMSK = (1<<PB2) | (1 << PB3) | (1 << PB4);

  sei();
}


// Must be volatile because interrupt is changing global state.
volatile char pushed_PB2 = 0;
volatile char pushed_PB3 = 0;
volatile char pushed_PB4 = 0;

volatile int motor_step = 0;


void pushed(int pin) {
  switch(pin) {
  case PB2:
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
const char motor_points[] = {
  20,40,60,80,100,120,140,160,180,200,220
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
  MotorStep();
}

int main() {
  setup();
  for(;;)  {
  }
  return 0;
}
