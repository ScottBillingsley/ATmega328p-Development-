/*
                    Create Triangle Wave
                   Vernon Billingsley c2022
                    
            Routines to generate a 256 step triangle wave in 
            +/- 127 steps in inline assembler...Useful in 
            sound generation on the arduino..
            


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
                 7
                 8        
                 9
                 10
                 11
                 12
                 13
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
/* About the accumulator */
/* The word accumulator */
volatile uint16_t mAccu[2];
/* The accu increment */
volatile uint16_t mIcnt[2];
/* The accu word */
volatile uint16_t mWord[2];
/* Bit mask to form the triangle */
volatile uint8_t invert_mask[2];
/* Bit mask to form the zero center triangle */
volatile uint8_t neg_mask[2];
/* Hold the output sample */
volatile int8_t out_sample[2];

/**************************  Functions ****************************/


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /************************* Setup Pins ***************************/

  /* Set the accumulator count    */
  mWord[0] = 512;


}/**************************  End Setup **************************/



/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {

  /* Increment the accumlator                         */
  asm(                              /* mAccu += mWord */
    "add %A1, %A2     \n"
    "adc %B1, %B2     \n"
    :"=r" (mAccu[0]): "r" (mAccu[0]), "r" (mWord[0])
  );

  /* Get the top 9 bits                                   */
  asm(                              /* mIcnt = mAccu >> 7 */
    "mov %A0, %B1     \n"
    "clr %B0          \n"
    "lsl %A0          \n"
    "rol %B0          \n"
    :"+r" (mIcnt[0]): "r" (mAccu[0])
  );

  /* Get the bit mask to reverse for triangle wave          */
  asm(                              /* mask = mIcnt >> 8    */
    "mov %A0, %B1     \n"
    :"=r" (invert_mask[0]): "r" (mIcnt[0])
  );

  /* Get the bit to set the wave about zero                        */
  asm(                              /* mask = (mInct >> 7) & 0x01  */
    "mov %A0, %A1     \n"
    "clr %B0          \n"
    "lsl %A0          \n"
    "rol %B0          \n"
    "mov %A0, %B0     \n"
    :"=r" (neg_mask[0]): "r" (mIcnt[0])
  );

  /* Convert the phase to triangle                      */
  asm(                        /* (~mIcnt[0]) & 0x7f     */
  "mov r16, %A1         \n"
  "ldi r17, 1           \n"
  "cp  r16, r17         \n"
  "brne A_%=            \n"
  "mov r16, %A2         \n"
  "ldi r17, 0x7f        \n"
  "com r16              \n"
  "and r16, r17         \n"
  "mov %A0, r16         \n"
  "jmp B_%=             \n"
  "A_%=:                \n"
  "mov r16, %A2         \n"
  "ldi r17, 0x7f        \n"
  "and r16, r17         \n"
  "mov %A0, r16         \n"
  "B_%=:                \n"
  :"=r" (out_sample[0]): "r" (neg_mask[0]), "r" (mIcnt[0]): "r16", "r17"
  );

  /* Convert 0 to 255 wave to 127 to -127 wave                        */
  asm(                          /* out_sample = (out_sample * -1)     */
    "mov r16, %A1       \n"     /* Load invert_mask into r16          */
    "ldi r17, 1         \n"     /* Load 1 into r17                    */
    "cp  r16, r17       \n"     /* Compare r16 to r17                 */
    "brne L_%=          \n"     /* Branch to label L_## if not equal  */
    "mov r16, %A2       \n"     /* Load out_sample into r16           */
    "neg r16            \n"     /* Negate r16                         */
    "mov %A0, r16       \n"     /* Overwrite out_sample with r16      */
    "jmp S_%=           \n"     /* Jump to label S_##                 */
    "L_%=:              \n"     /* Unique label L_##                  */
    "mov %A0, %A2       \n"     /* Move out_sample to out_sample      */
    "S_%=:              \n"     /* Unigue labek S_##                  */
    :"=r" (out_sample[0]): "r" (invert_mask[0]), "r" (out_sample[0]) : "r16", "r17"
  );

  /* Display using the serial plotter */
  Serial.println(out_sample[0]);

  delay(25);

}/*************************** End Loop *****************************/
