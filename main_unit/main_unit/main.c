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

//SM
enum state{init,waitUp,countUp,countZero,waitDown,countDown,countNum} states;
void tick();

//variables for SM
unsigned char Ybtn = 0x00;
unsigned char Bbtn = 0x00;
unsigned char IR1 = 0x00;
unsigned char waitCnt = 0x00;

uint16_t counter = 0x00;

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
    
    //timer set to 2 msec
    TimerSet(2);
    TimerOn();
    
    //turn on the LED 7seg
    init7seg(0,15,dev7SEG);
    
    //write number
    writeNum(0x1234,0);
        
    //write to the display
    writeDisplay(dev7SEG);   
    
    while (1) 
    {   
        Ybtn = (~PINB & 0x02) >> 1;
        Bbtn = (~PINB & 0x04) >> 2;
        
        //if(USART_HasReceived(1)) {
            IR1 = USART_receive(1);
        //}
        
        PORTB = IR1;
       
        //SM
        tick();
       
        //Wait
        while (!TimerFlag);
        TimerFlag = 0;
        
    }
}

void tick() {
    switch(states) {
        case init:
            states = waitUp;
            break;
        case waitUp:
            if(IR1 && !(Ybtn) && !(Bbtn)) {
                states = countUp;
                if(counter < 9999) {
                    counter++;
                }
            } else if(!(IR1) && Ybtn && !(Bbtn)) {
                states = countZero;
            } else if(!(IR1) && !(Ybtn) && Bbtn) {
                states = waitDown;
            } else {
                states = waitUp;
            }
            break;
        case countUp:
            if(!(IR1)) {
                states = waitUp;
            } else {
                states = countUp;
            }
            break;
        case countZero:
            states = waitUp;
            break;
        case waitDown:
            if(IR1 && !(Ybtn) && !(Bbtn)) {
                states = countDown;
                if(counter > 0) {
                    counter--;
                }
            } else if(!(IR1) && Ybtn && !(Bbtn)) {
                states = countNum;
            } else if(!(IR1) && !(Ybtn) && Bbtn) {
                states = waitUp;
            } else {            
                states = waitDown;
            }
            break;
        case countDown:
            if(!(IR1)) {
                states = waitDown;
            } else {
                states = countDown;
            }
            break;
        case countNum:
            states = waitDown;
            break;
        default:
            states = init;
            break;
    }
    
    switch(states) {
        case init:
            counter = 0;
            break;
        case waitUp:
            //write number
            writeNum(counter,0);
            //write to the display
            writeDisplay(dev7SEG);
            break;
        case countUp:
            break;
        case countZero:
            counter = 0;
            break;
        case waitDown:
            //write number
            writeNum(counter,0);
            //write to the display
            writeDisplay(dev7SEG);
            break;
        case countDown:
            break;
        case countNum:
            counter = 120;
            break;    
        default:
            break;
    }    
}
          

