/*
 *          ATMega 328 Development
 *          Timer 2
 *          
 *          Setup timer 2
 *          
 */

/************************* Defines ********************************/
#define DEBUG 1

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

    
  /* Setup timer 2 to provide data clock */
  cli();                //stop interrupts
  //set timer1
  TCCR2A = 0;// set entire TCCR1A register to 0
  TCCR2B = 0;// same for TCCR1B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register 1 kHz
  OCR2A = 124;  /* 245 Hz for testing  */
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
