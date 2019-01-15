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

#endif /* ADAFRUIT_7SEG_H_ */