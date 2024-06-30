/*
*                     Fisher-Yates Array Shuffle
*                     Vernon Billingsley c2024
*
*               Randomly shuffle an array using 
*               the Fisher-Yates algorithm..
*
*                             Arduino Nano
*                           -----------------
*              TXD PD1   --| D1              |-- Vin
*              RXD PD0   --| D0              |-- GND
*              Rest      --|                 |-- PC6 Reset
*              GND       --|                 |-- 5 volt
*              INT0 PD2  --| D2              |-- ADC7
*         OC2B INT1 PD3  --| D3              |-- ADC6
*                   PD4  --| D4         ADC5 |-- PC5 ADC5 SCL
*         OC0B      PD5  --| D5         ADC4 |-- PC4 ADC4 SDA
*         OC0A AIN0 PD6  --| D6         ADC3 |-- PC3 ADC3
*              AIN1 PD7  --| D7         ADC2 |-- PC2 ADC2
*              CLK0 PB0  --| D8         ADC1 |-- PC1 ADC1
*              OC1A PB1  --| D9         ADC0 |-- PC0 ADC0
*         SS   OC1B PB2  --| D10        AREF |--
*         MOSI OC2A PB3  --| D11             |-- 3.3 volt
*         MISO      PB4  --| D12         D13 |-- PB5 SCK LED
*                            ----------------
*
*                 Pins:     Function
*                 0
*                 1
*                 2       
*                 3       
*                 4      
*                 5       
*                 6       
*                 7       
*                 8       
*                 9
*                 10
*                 11
*                 12
*                 13
*                 A0     
*                 A1
*                 A2
*                 A3       
*                 A4      
*                 A5      
*                 AREF   
*
*     Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission
*    notice shall be included in all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*    THE SOFTWARE.
*
*    Reference:
*   https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
*
*/
/************************ Includes ********************************/
#include "globals.h"


/************************** Variables *****************************/
uint8_t start_array[8] = {
  36,
  83,
  55,
  25,
  8,
  12,
  17,
  10,
};
const uint8_t array_size = sizeof(start_array) / sizeof(start_array[0]);

/**************************  Functions ****************************/
void shuffle(uint8_t *array, uint8_t size) {
  uint8_t array_size = size;
  int8_t index = array_size - 1;

  while (index >= 0) {
    uint8_t this_random = random(index);
    /* Swap this_random array value for index array value */
    uint8_t this_value = array[this_random];
    uint8_t this_index = array[index];
    array[index] = this_value;
    array[this_random] = this_index;
    index--;
  }
}

/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG == 1) {
    Serial.begin(115200);
  }

  randomSeed(analogRead(A7));

  Serial.println("Fisher-Yates Array Shuffle ");

  Serial.print("The start array    : ");

  for (uint8_t x = 0; x < array_size; x++) {
    Serial.print(start_array[x]);
    Serial.print(", ");
  }
  Serial.println();

  Serial.print("The shuffled array : ");

  shuffle(start_array, array_size);

  for (uint8_t x = 0; x < array_size; x++) {
    Serial.print(start_array[x]);
    Serial.print(", ");
  }
  Serial.println();

  Serial.print("The shuffled array : ");

  shuffle(start_array, array_size);

  for (uint8_t x = 0; x < array_size; x++) {
    Serial.print(start_array[x]);
    Serial.print(", ");
  }
  Serial.println();


  /************************* Setup Pins ***************************/


} /**************************  End Setup **************************/



/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {



} /*************************** End Loop *****************************/
