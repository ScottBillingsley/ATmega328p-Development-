/*
            ATmega328 Development
            Vernon Billingsley 2021
            Note Timer

            A set of routines to produce several timers using
            Timer1 as the base timer


     Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission
    notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

*/

#include "Note_Timer.h"

/* Create an instance of the timer */
NoteTimer myNote;
/* Create a second timer */
NoteTimer clock1;

/************************* Defines ********************************/
#define DEBUG 0

#if DEBUG == 1
#define dprint(expression) Serial.print("# "); Serial.print( #expression ); Serial.print( ": " ); Serial.println( expression )
#define dshow(expression) Serial.println( expression )
#else
#define dprint(expression)
#define dshow(expression)
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define My_Measured_Speed 32748


/************************** Variables *****************************/
/* Used to turn LED on or off */
boolean LED_ON = false;
/* Logic to check the LED once per timer */
boolean NEW_CLOCK = false;

/**************************  Functions ****************************/


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /* Set the timer compare for the note */
  /* ie..A3 = (32748 / 220) / 2         */
  myNote.setCompare(74);
  /* 77 mS timer = 13 Hz */
  clock1.setCompare(2521);

  /************************* Setup Pins ***************************/
  /* Pin D2 as OUTPUT */
  DDRD |= _BV (2); // pinMode (2, OUTPUT);
  /* Pin D13 as OUTPUT for LED */
  DDRB |= _BV (5); // pinMode (13, OUTPUT);

  /*************************  Setup Timer1 ************************/
  cli();                //stop interrupts
  //set timer1
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register to 32768  2^15
  OCR1A = 487;
  // turn on CTC mode
  sbi(TCCR1B, WGM12);
  /*Set prescaler to 1 */
  cbi(TCCR1B, CS12);
  cbi(TCCR1B, CS11);
  sbi(TCCR1B, CS10);
  // enable timer compare interrupt
  sbi(TIMSK1, OCIE1A);
  sei();

}/**************************  End Setup **************************/


ISR(TIMER1_COMPA_vect) {
  /*Increment the counters */
  myNote.count();
  clock1.count();

}

/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {
  /* Each time through the loop, check the count and set the
     output pin...
  */
  if (myNote.check()) {
    /* Toggle the output pin in 2 clocks */
    PIND = PIND | 0b000000100;
    myNote.init();
  }

  /* Using as a timer for an event  */
  if (clock1.check()) {
    NEW_CLOCK = true;
    clock1.init();
  }

  if (NEW_CLOCK) {
    LED_ON = !LED_ON;
    if (LED_ON) {
      PORTB |= _BV (5); // digitalWrite (13, HIGH);
    } else {
      PORTB &= ~_BV (5); // digitalWrite (13, LOW);
    }
    NEW_CLOCK = false;
  }

}/*************************** End Loop *****************************/
