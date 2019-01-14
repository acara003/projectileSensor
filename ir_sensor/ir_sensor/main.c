/*
 * ir_sensor.c
 *
 * Created: 11/7/2018 1:08:24 PM
 * Author : Alan Carabes
 */ 

#include <avr/io.h>

#include "usart.h"

//#define FOSC 1843200
#define FOSC 8000000UL
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // IR sensor
    DDRB = 0xFF; PORTB = 0x00; // LED
    
    USART_init(0,MYUBRR);
    
    unsigned char LED = 0x00;
    
    while (1) 
    {
        if((~PINA & 0x01) == 0x01) {
            LED = 0x01;
        } else {
            LED = 0x00;
        }
        
        //if(USART_IsSendReady(0)) {
            USART_send(LED,0);
        //}
        
        PORTB = LED;
    }
}

