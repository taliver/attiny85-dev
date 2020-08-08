// This is the attempt to develop effectiovely a framebuffer for the
// WS2812B leds

#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

// Pins:
// PB0: Timing checks.
// PB1: Output for lights


#ifdef MAIN
void setup() {
  DDRB = (1 << PB0) | (1<< PB1);
  cli();
}
#endif

void reset_low(int port_bit) {
    PORTB &= ~(1 << port_bit);
    _delay_loop_2(800);

}

// Move the value from the register and r17 (allows the variable).
#define MOV(r,n) "mov " #r  ", " #n  " \n"
#define LOAD(r,n) "ld " #r  ", " #n  " \n" 

#define OUT(r,v) "out " #r "," #v "\n"
#define NOP "nop\n"
#define SRBS(r,b) "sbrs " #r "," #b "\n"
#define BRBS(b,l) "brbs " #b "," #l "\n"
#define LSL(r) "lsl " #r "\n"
#define DEC(r) "dec " #r "\n"
#define RJMP(l) "rjmp " #l "\n"

// "out 0x18, %1\n" 
// Experiment 1: Setting the bits correcly.
#define FIRSTBIT \
  OUT(0x18, %1) \
  NOP \
  SRBS(17,7) \
  OUT(0x18,%0) \
  NOP \
  NOP \
  OUT(0x18,%0) \
  LSL(17) \
  NOP \
  NOP

#define BIT \
  "nop \n" \
  FIRSTBIT

#define LASTBIT \
  OUT(0x18, %1) \
  NOP \
  SRBS(17,7) \
  OUT(0x18,%0) \
  NOP \
  DEC(%3) \
  OUT(0x18,%0) \


void memleds(char *base, int num_leds, int port_bit) {
  char size = num_leds * 3; // Max is now 85 leds.
  char port_on = PORTB | (1 << port_bit);
  char port_off = PORTB & ~(1 << port_bit);
  asm(
      "loop: " LOAD(17, X+)
      FIRSTBIT
      BIT
      BIT
      BIT
      BIT
      BIT
      BIT
      LASTBIT
      BRBS(1,done)
      RJMP(loop)
      "done:"
      :
      : "r"(port_off), "r"(port_on), "x"(base), "r"(size));
}     

#if 0
char letter[8][24] =
  {
    {R,R,R,R,R,R,R,R},
    {R,R,R,R,R,R,R,R},
    {_,_,_,R,R,_,_,_},
    {_,_,_,R,R,_,_,_},
    {R,R,R,R,R,R,R,R},
    {R,R,R,R,R,R,R,R},
    {_,_,_,_,_,_,_,_},
    {_,_,_,_,_,_,_,_},
      };
#endif

#ifdef MAIN
#define R  0x00, 0x0F, 0x00
#define G  0x0F, 0x00, 0x00
#define B  0x00, 0x00, 0x0F
#define _ 0x00, 0x00, 0x00
char pattern[8][24] =
  {
    {R,G,G,_,_,_,B,B},
    {R,G,R,_,_,_,B,B},
    {_,G,G,R,R,_,_,_},
    {_,_,_,R,R,_,_,_},
    {_,_,_,R,R,_,_,_},
    {_,_,_,R,R,_,B,B},
    {_,_,_,_,_,_,B,B},
    {_,_,_,_,_,_,B,B},
      };


int main() {
setup();
//  char leds[46] = {G,G,G,G,G,B,B,B,B,B,R,R,R,R,R};
char zero[46] = {0};
memleds(zero,15,PB1);
int i =0;
for(;;i++)  {
reset_low(PB1);
_delay_loop_2(100);
memleds(pattern[i&0xF],8,PB1);
}
}
#endif
