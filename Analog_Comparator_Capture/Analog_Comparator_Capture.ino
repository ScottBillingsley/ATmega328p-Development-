/*
                  Analog Comparator Capture
                  Vernon Billingsley c2023

            How to setup the compatator/timer1 capture..
            Read the time from rising edge to rising edge
            and print the results to the serial monitor..
            With timer1 divisor at 1024, the input capture
            time is from 1 Hz to aprox 20 Hz...

     Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
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
    

                       Arduino Nano
                     -----------------
        TXD PD1   --| D1              |-- Vin
        RXD PD0   --| D0              |-- GND
        Rest      --|                 |-- PC6 Reset
        GND       --|                 |-- 5 volt
        INT0 PD2  --| D2              |-- ADC7
   OC2B INT1 PD3  --| D3              |-- ADC6
             PD4  --| D4         ADC5 |-- PC5 ADC5 SCL
   OC0B      PD5  --| D5         ADC4 |-- PC4 ADC4 SDA
   OC0A AIN0 PD6  --| D6         ADC3 |-- PC3 ADC3
        AIN1 PD7  --| D7         ADC2 |-- PC2 ADC2
        CLK0 PB0  --| D8         ADC1 |-- PC1 ADC1
        OC1A PB1  --| D9         ADC0 |-- PC0 ADC0
   SS   OC1B PB2  --| D10        AREF |--
   MOSI OC2A PB3  --| D11             |-- 3.3 volt
   MISO      PB4  --| D12         D13 |-- PB5 SCK LED
                      ----------------

                 Pins:
                 Nano    Function
                 0
                 1
                 2
                 3
                 4
                 5
                 6
                 7      Clock input 
                 8
                 9
                 10
                 11
                 12
                 13     Built in LED 
                 A0
                 A1
                 A2
                 A3
                 A4
                 A5
                 A6
                 A7

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
volatile uint16_t capture_time;

volatile boolean new_time = false;

/**************************  Functions ****************************/


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /************************* Setup Pins ***************************/
  DDRB |= _BV (5); // pinMode (13, OUTPUT);


  /****************  Setup analog comparator *********************/
  /* ATmega328p datasheet p.202   */
  /* Enable the analog comparator */
  cbi(ACSR, ACD);
  /* Select bandgap as positive input */
  sbi(ACSR, ACBG);
  /* Disable the comparator interrupt */
  cbi(ACSR, ACIE);
  /* Enable Input Capture */
  sbi(ACSR, ACIC);
  /* Disable digital buffers */
  sbi(DIDR1, AIN1D);  /* D7 */

  /*************************  Setup Timer1 ************************/
  cli();                //stop interrupts

  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0

  /* Falling edge of comparator */
  cbi(TCCR1B, ICES1);

  /* Waveform Normal */
  cbi(TCCR1B, WGM13);
  cbi(TCCR1B, WGM12);
  cbi(TCCR1B, WGM11);
  cbi(TCCR1B, WGM10);

  /*Set prescaler to 256 */
  /* 16 uS per clock */
  sbi(TCCR1B, CS12);
  cbi(TCCR1B, CS11);
  cbi(TCCR1B, CS10);
  /* Enable input capture interrupt */
  sbi(TIMSK1, ICIE1);
  /* Enable interrupts */
  sei();


}/**************************  End Setup **************************/
ISR(TIMER1_CAPT_vect) {
  /* Store the clock value */
  capture_time =  ICR1;
  /* Clear the count register */
  TCNT1 = 0;
  /* Set the boolean */  
  new_time = true;
}


/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {
  if (new_time == true) {

    Serial.print(capture_time);
    Serial.print("\t");
    Serial.print((float)capture_time * .000016, 4);
    Serial.println(" seconds"); 

    new_time = false;
  }

  /* Read the analog comparator output and set */
  /* the built in LED                          */
  if (bitRead(ACSR, ACO) == 0) {
    PORTB |= _BV (5); // digitalWrite (13, HIGH);
  } else {
    PORTB &= ~_BV (5); // digitalWrite (13, LOW);
  }
  

}/*************************** End Loop *****************************/
