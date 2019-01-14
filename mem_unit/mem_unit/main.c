/*
 * main_unit.c
 *
 * Created: 11/7/2018 1:22:46 PM
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
    DDRA = 0xFF; PORTA = 0x00; // LED  
   
    USART_init(1,MYUBRR);
  
    unsigned char LED = 0x01;
    
    while (1) 
    {
        
        if(USART_HasReceived(1)) {
            LED = USART_receive(1);
        }
        
        PORTA = LED; 
    }
}

