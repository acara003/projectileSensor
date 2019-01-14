/*
 * main_unit.c
 *
 * Created: 11/7/2018 1:22:46 PM
 * Author : Alan Carabes
 */ 

#include <avr/io.h>

#include "usart.h"
#include "i2cmaster.h"

//#define FOSC 1843200
#define FOSC 8000000UL
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define dev8SEG  0x70      // device address of 8 segment adafruit LED

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00; // LED
    DDRB = 0x01; PORTB = 0xFE; // LED and buttons
    DDRC = 0xFF; PORTC = 0x00; // I2C
    DDRD = 0x00; PORTD = 0xFF; // USART
    
    //usart
    USART_init(0,MYUBRR);
    USART_init(1,MYUBRR);
    
    //i2c
    i2c_init();
    
    unsigned char LED = 0x00;
    unsigned char SendLED = 0x00;
    unsigned char buttonYellow = 0x00;
    
    //for i2c
    unsigned char ret;
    
    //for LED
    uint16_t displaybuffer[8]; 
    
    ret = i2c_start((dev8SEG<<1)+I2C_WRITE);       // set device address and write mode
    //ret = i2c_start(0xE1);       // set device address and write mode
        
    if ( ret ) {
        // failed to issue start condition, possibly no device found 
        i2c_stop();
        PORTA = 0x81;                         // activate bit 0 and 3 LED to show error
    } else {
        // issuing start condition ok, device accessible 
        i2c_write(0x21);                      // turn oscillator on
        i2c_stop();
        
        i2c_start((dev8SEG<<1)+I2C_WRITE);
        i2c_write(0x81);                      //blink rate is bit 2
        i2c_stop();
        
        i2c_start((dev8SEG<<1)+I2C_WRITE);
        i2c_write(0xE0 | 15);                 //brightness is between 0 and 15
        i2c_stop();
        
        i2c_start((dev8SEG<<1)+I2C_WRITE);
        i2c_write((uint8_t)0x00);            //begin at address 0;
        
        //this is what is going to be printed
        displaybuffer[2] = 0x02;
        
        
        //go through stored things to display
        for (uint8_t i=0; i<8; i++) {       
            i2c_write(displaybuffer[i] & 0xFF);
            i2c_write(displaybuffer[i] >> 8);
        }
        
        //i2c_write(displaybuffer[2]);
        
        i2c_stop();
            
        PORTA = 0xFF;                         // output to LEDs to show success
    }
    
    while (1) 
    {
        buttonYellow = ~PINB;
        if((buttonYellow & 0x02) == 0x02) {
            SendLED = 0x01;
        } else {
            SendLED = 0x00;
        }
        
        
        if(USART_HasReceived(0)) {
            LED = USART_receive(0);
        }
        
        if(USART_IsSendReady(1)) {
            USART_send(SendLED,1);
        }
        
        PORTB = LED; 
        
    }
}

