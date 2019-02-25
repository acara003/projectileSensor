/*
 * adafruit_7seg.h
 *
 * Created: 1/14/2019 2:39:57 PM
 *  Author: Alan
 */ 

//This file is based off the adafruit included library for 7segment
//https//www.github.com/adafruit/Adafruit_LED_Backpack

#ifndef ADAFRUIT_7SEG_H_
#define ADAFRUIT_7SEG_H_

#include "i2cmaster.h"

//for LED
uint16_t displaybuffer[8];

static const uint8_t numbertable[] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F, /* 9 */
    0x77, /* a */
    0x7C, /* b */
    0x39, /* C */
    0x5E, /* d */
    0x79, /* E */
    0x71, /* F */
};

//turn on
unsigned char init7seg(unsigned char blink, unsigned char brightness, unsigned char devAddr) {
    if(i2c_start((devAddr<<1)+I2C_WRITE)) {
        i2c_stop();
        return 1;
    } else {
        if(blink > 3) return 1;
		
		if(brightness > 15) return 1;
        
        // turn oscillator on
        i2c_write(0x21);
        i2c_stop();
        
        //blink rate is bit 2
        i2c_start((devAddr<<1)+I2C_WRITE);
        i2c_write(0x81 | (blink<<1));
        i2c_stop();
        
        //brightness is between 0 and 15
        i2c_start((devAddr<<1)+I2C_WRITE);
        i2c_write(0xE0 | brightness);
        i2c_stop();
    }
    return 0;
}

void setBrightness(unsigned char brightness, unsigned char devAddr) {
    if(brightness > 15) {
        return;
    }
    //brightness is between 0 and 15
    i2c_start((devAddr<<1)+I2C_WRITE);
    i2c_write(0xE0 | brightness);
    i2c_stop();
}

void setBlinkRate(unsigned char blink, unsigned char devAddr) {
    if(blink > 3) {
        return;
    }
    //blink rate is bit 2
    i2c_start((devAddr<<1)+I2C_WRITE);
    i2c_write(0x81 | (blink<<1));
    i2c_stop();
}

void writeDisplay(unsigned char addr) {
    i2c_start((addr<<1)+I2C_WRITE);
    i2c_write((uint8_t)0x00);            //begin at address 0;
    
    //go through stored things to display
    for (uint8_t i=0; i<8; i++) {
        i2c_write(displaybuffer[i] & 0xFF);
        i2c_write(displaybuffer[i] >> 8);
    }
    
    i2c_stop();
}

void clear7seg() {
    //set everything to nothing to clear
    for (uint8_t i=0; i<8; i++) {
        displaybuffer[i] = 0;
    }
}

void drawColon(unsigned char d) {
     if (d) {
        displaybuffer[2] = 0x2;
     }else {
        displaybuffer[2] = 0;
     }     
}

void writeDigitRaw(unsigned char d, uint8_t bitmask) {
    if (d > 4) return;
    
    displaybuffer[d] = bitmask;
}

void writeDigitNum(unsigned char d, uint8_t num, unsigned char dot) {
    if (d > 4) return;
    if (dot > 2) return;
    
    writeDigitRaw(d, numbertable[num] | (dot << 7));
}

void writeNum(uint16_t number,unsigned char drawDots) {
    if (drawDots > 2) return;
    
    writeDigitNum(0, (number / 1000), drawDots);
    writeDigitNum(1, (number / 100) % 10, drawDots);
    drawColon(drawDots);
    writeDigitNum(3, (number / 10) % 10, drawDots);
    writeDigitNum(4, number % 10, drawDots);
}

/* need to fix bugs */

void printError() {
    for(uint8_t i = 0; i < 4; ++i) {
        writeDigitRaw(i, (i == 2 ? 0x00 : 0x40));
    }
}

void printFloat(double n, uint8_t fracDigits, uint8_t base) {
     // available digits on display
     uint8_t numericDigits = 4;
     
     // true if the number is negative
     char isNegative = 0;
     
      // is the number negative?
      if(n < 0) {
          // need to draw sign later
          isNegative = 1;
          // the sign will take up one digit
          --numericDigits;
          // pretend the number is positive
          n *= -1;
      }
      
      // calculate the factor required to shift all fractional digits
      // into the integer part of the number
      double toIntFactor = 1.0;
      for(int i = 0; i < fracDigits; ++i) { toIntFactor *= base; }
          
      // create integer containing digits to display by applying
      // shifting factor and rounding adjustment
      uint32_t displayNumber = n * toIntFactor + 0.5;
      
      // calculate upper bound on displayNumber given
      // available digits on display
      uint32_t tooBig = 1;
      for(int i = 0; i < numericDigits; ++i)  { tooBig *= base; }
          
      // if displayNumber is too large, try fewer fractional digits
      while(displayNumber >= tooBig) {
          --fracDigits;
          toIntFactor /= base;
          displayNumber = n * toIntFactor + 0.5;
      }
      
      // did toIntFactor shift the decimal off the display?
      if (toIntFactor < 1) {
          printError();
      } else {
          // otherwise, display the number
          int8_t displayPos = 4;
          
          if (displayNumber)  //if displayNumber is not 0
          {
              for(uint8_t i = 0; displayNumber || i <= fracDigits; ++i) {
                  char displayDecimal = (fracDigits != 0 && i == fracDigits);
                  writeDigitNum(displayPos--, displayNumber % base, displayDecimal);
                  if(displayPos == 2) writeDigitRaw(displayPos--, 0x00);
                  displayNumber /= base;
              }
          }
          else {
              writeDigitNum(displayPos--, 0, 0);
          }
          
          // display negative sign if negative
          if(isNegative) writeDigitRaw(displayPos--, 0x40);
          
          // clear remaining display positions
          while(displayPos >= 0) writeDigitRaw(displayPos--, 0x00);
    }
}

void printNumber(float n, uint8_t base)
{
    printFloat(n, 0, base);
}

#endif /* ADAFRUIT_7SEG_H_ */