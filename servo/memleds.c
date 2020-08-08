// This is the attempt to develop effectiovely a framebuffer for the
// WS2812B leds

#define F_CPU 8000000L
#include <avr/io.h>
#include <util/delay_basic.h>
#include <avr/interrupt.h>

// Pins:
// PB0: Timing checks.
// PB1: Output for lights

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

// Note, the DEC is where it is because of timing --
// it's actually part of the loop check, otherwise it would
// also be a "NOP"

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

