/*
            ATmega328 Development
            Vernon Billingsley 2021
            ADC with Lowpass Filter

            Setup ADC..
            Use a Lowpass filter to smooth the results..

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

/************************* Defines ********************************/
/* Enable the Serial port and use the Plotter to
   display the results..
*/
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
/*Hold the adc values */
unsigned long adc_array[7];
/*The ADC channel to read */
uint8_t adc_count = 0;

/*About the filter  */
/* .125 * 256 = 32 */
unsigned long alpha = 32;
/*Store the filtered sample */
unsigned long adc_filtered[5];
/*Store the previous filtered sample */
unsigned long f_v[5][3];


/**************************  Functions ****************************/
/* An integer math low pass filter for smooting the ADC reads */
void filter(byte ary_num) {
  /*Calculate the new value */
  //f_v[ary_num][1] = (float)alpha * adc_array[ary_num] + (1 - alpha) * f_v[ary_num][0];
  f_v[ary_num][1] = (((adc_array[ary_num] << 8) * alpha) + ((256 - alpha) * ( f_v[ary_num][0] << 8))) >> 16;
  /*Store the old value */
  f_v[ary_num][0] = f_v[ary_num][1];

  /*Store the filtered value */
  adc_filtered[ary_num] = f_v[ary_num][1];
}

/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }


  /************************* Setup Pins ***************************/



  /*************************  Setup ADC ***************************/
  /*Set to Right Adjust for 1024 precision */
  cbi(ADMUX, ADLAR);

  /*Set to VRef to AVCC */
  cbi(ADMUX, REFS1);
  sbi(ADMUX, REFS0);

  /*Set to ADC0 to start */
  cbi(ADMUX, MUX3);
  cbi(ADMUX, MUX2);
  cbi(ADMUX, MUX1);
  cbi(ADMUX, MUX0);

  /*Set prescaler to 64 */
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  /*Turn off Auto Trigger */
  cbi(ADCSRA, ADATE);

  /*Turn the ADC ON  */
  sbi(ADCSRA, ADEN);

  /*Start the first conversion */
  sbi(ADCSRA, ADSC);


}/**************************  End Setup **************************/


/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {
  /*Check to see if ADC has finished */
  if (!(bitRead(ADCSRA, ADSC))) {
    /*Read and store the results  */
    uint8_t temp_adcl = ADCL;
    uint16_t temp_adc = (ADCH << 8) + temp_adcl;
    /*Keep a running average */
    adc_array[adc_count] = (adc_array[adc_count] + temp_adc) / 2;
    /*Filter the results using an integer math low pass filter */
    filter(adc_count);
    /*Send the results to the Plotter */
    Serial.println(adc_filtered[adc_count]);
    /*Start the next conversion */
    sbi(ADCSRA, ADSC);
  }


  }/*************************** End Loop *****************************/
