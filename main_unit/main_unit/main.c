/*
 * main_unit.c
 *
 * Created: 11/7/2018 1:22:46 PM
 * Author : Alan Carabes
 */ 

#include <avr/io.h>

#include "usart.h"
//#include "i2cmaster.h"
#include "adafruit_7seg.h"
#include "ATmegaTimer.h"

//#define FOSC 1843200
#define FOSC 8000000UL
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define dev7SEG  0x70      // device address of 8 segment adafruit LED

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
    
    //timer set to 100 msec
    TimerSet(100);
    TimerOn();
    
    unsigned char LED = 0x00;
    unsigned char SendLED = 0x00;
    unsigned char Ybtn = 0x00;
    
    //turn on the LED 7seg
    init7seg(0,15,dev7SEG);
    
    //setup for the 7segment    
    i2c_start((dev7SEG<<1)+I2C_WRITE);
    
    
    //write number
    writeNum(0x1234,0);
        
    //write to the display
    writeDisplay(dev7SEG);   
    
    while (1) 
    {   
        Ybtn = ~PINB;
        if((Ybtn & 0x02) == 0x02) {
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
       
        while (!TimerFlag);	// Wait .1 sec
        TimerFlag = 0;
        
    }
}

