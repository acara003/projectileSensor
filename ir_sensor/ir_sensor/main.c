/*
 * ir_sensor.c
 *
 * Created: 11/7/2018 1:08:24 PM
 * Author : Alan Carabes
 */ 

#include <avr/io.h>

#include "usart.h"
#include "ATmegaTimer.h"

//#define FOSC 1843200
#define FOSC 8000000UL
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

enum state{init,waitSense,sendSense} states;
void tick();

unsigned char IR = 0x00;
unsigned char md = 0x00;
unsigned char waitCnt = 0x00;

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // IR sensor
    DDRB = 0xFF; PORTB = 0x00; // LED
    
    USART_init(0,MYUBRR);
    
    while (1) 
    {
        IR = ~PINA & 0x03;
        
        //SM
        tick();
        
        PORTB = IR;
    }
}

void tick() {
    switch(states) {
        case init:
            states = waitSense;
            break;
        case waitSense:
            if(md == 2) {
                
            } else if((IR & 0x01) && !(md == 2)) {
                states = sendSense;
            } else {
                states = waitSense;
            }
            break;
        case sendSense:
            if(!(IR&0x01)) {
                states = waitSense;
            } else {
                states = sendSense;
            }
            break;
        default:
            states = init;
            break;
    }
    
    switch(states) {
        case init:
            md = 1;
            waitCnt = 0;
            break;
        case waitSense:
            if(USART_HasReceived(0)) {
                md = USART_receive(0);
            }
            if(USART_IsSendReady(0)) {
                USART_send(0,0);
            }
            break;
        case sendSense:
            if(USART_IsSendReady(0)) {
                USART_send(1,0);
            }
            break;
        default:
            break;
    }
}

