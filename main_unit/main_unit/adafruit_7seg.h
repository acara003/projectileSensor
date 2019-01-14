/*
 * adafruit_7seg.h
 *
 * Created: 1/14/2019 2:39:57 PM
 *  Author: Alan
 */ 


#ifndef ADAFRUIT_7SEG_H_
#define ADAFRUIT_7SEG_H_

#include "i2cmaster.h"

//turn on
unsigned char init7seg(unsigned char blink, unsigned char brightness, unsigned char devAddr) {
    if(i2c_start((devAddr<<1)+I2C_WRITE)) {
        i2c_stop();
        return 1;
    } else {
        if(blink > 3) {
            return 1;
        } else if(brightness > 15) {
            return 1;
        }
        
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



#endif /* ADAFRUIT_7SEG_H_ */