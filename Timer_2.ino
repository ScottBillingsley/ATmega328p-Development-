/*
 *          ATmega328 Development
 *          Vernon Billingsley 2021
 *          Timer 2
 *          
 *          Setup timer 2 in CTC Mode..
 *          Use interrupt vector to toggle a pin..
 *          
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
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
 *          
 */

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


/************************** Variables *****************************/
boolean flip = false;

/**************************  Functions ****************************/


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }


  /************************* Setup Pins ***************************/
  /* Pin D2 for testing output */
  DDRD |= _BV (2); // pinMode (2, OUTPUT);

    
 /*************************  Setup Timer2 ************************/

  cli();                //stop interrupts
 
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register 8 kHz
  OCR2A = 124;  
  // Set to CTC
  cbi(TCCR2B, WGM22);
  sbi(TCCR2A, WGM21);
  cbi(TCCR2A, WGM20);
  /*Set prescaler to 8 */
  cbi(TCCR2B, CS22);
  sbi(TCCR2B, CS21);
  cbi(TCCR2B, CS20);
  // enable timer compare interrupt
  sbi(TIMSK2, OCIE2A);
  sei();                //allow interrupts

}/**************************  End Setup **************************/

ISR(TIMER2_COMPA_vect) {
  flip = !flip;
  if(flip){
    PORTD |= _BV (2); // digitalWrite (2, HIGH);
  }else{
    PORTD &= ~_BV (2); // digitalWrite (2, LOW);
  }
}

/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {



}/*************************** End Loop *****************************/
