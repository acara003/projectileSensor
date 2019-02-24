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
enum state{init,waitSense,sense,changeVal,setVal,switchVel,switchSense,waitVel,irWait,irCalc} senseStates;
void tick();

//variables for SM
unsigned char Ybtn = 0x00;
unsigned char Bbtn = 0x00;
unsigned char Gbtn = 0x00;

//sensors
unsigned char IR1 = 0x00;
unsigned char IR2 = 0x00;
unsigned char rec = 0x00;

//counters
uint16_t dcnt = 0x00;
uint16_t tcnt = 0x00;

//modifiers
char val = 0x01;
float fts = 83.3333;
//float ms = 25.4;
float vel = 0;

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00; // LED
    DDRB = 0x01; PORTB = 0xFE; // LED and buttons
    DDRC = 0xFF; PORTC = 0x00; // I2C
    DDRD = 0x00; PORTD = 0xFF; // USART
    
    //usart
    USART_init(1,MYUBRR);
    
    //i2c
    i2c_init();
    
    //timer set to 2 msec
    TimerSet(2);
    TimerOn();
    
    //turn on the LED 7seg
    init7seg(0,15,dev7SEG);
    
    //write number
    writeNum(0x0000,0);
        
    //write to the display
    writeDisplay(dev7SEG);
    
    while (1) 
    {   
        //checks
        Ybtn = (~PINB & 0x02) >> 1;
        Bbtn = (~PINB & 0x04) >> 2;
        Gbtn = (~PINB & 0x08) >> 3;
        
        //sensor data
        rec = USART_receive(1) & 0x03;
        IR1 = rec & 0x01;
        IR2 = (rec & 0x02) >> 1;
        
        //display
        PORTB = rec;
       
        //SM
        tick();
       
        //Wait
        while (!TimerFlag);
        TimerFlag = 0;
        
    }
}

void tick() {
    switch(senseStates) {
        case init:
            senseStates = waitSense;
            break;
        case waitSense:
            if( (IR1) && !(Ybtn) && !(Bbtn) && !(Gbtn) ) {
                senseStates = sense;
            } else if( !(IR1) && (Ybtn) && !(Bbtn) && !(Gbtn) ) {
                senseStates = changeVal;
            } else if( !(IR1) && !(Ybtn) && (Bbtn) && !(Gbtn) ) {
                senseStates = setVal;
            } else if( !(IR1) && !(Ybtn) && !(Bbtn) && (Gbtn) ) {
                senseStates = switchVel;
            } else {
                senseStates = waitSense;
            }
            break;
        case sense:
            if(IR1) {
                senseStates = sense;
            } else {
                senseStates = waitSense;
                //action
                if( !( ((val) && (dcnt >= 9999)) || (!(val) && (dcnt <= 0)) ) ) {
                    dcnt += val;
                }
            }
            break;
        case changeVal:
            if(Ybtn) {
                senseStates = changeVal;    
            } else {
                senseStates = waitSense;
                val = val * -1;
            }
            break;
        case setVal:
            if(Bbtn) {
                senseStates = setVal;
            } else {
                senseStates = waitSense;
            }
            break;
        case switchVel:
            if(Gbtn) {
                senseStates = switchVel;
            } else {
                senseStates = waitVel;
            }
            break;
        case switchSense:
            if(Gbtn) {
                senseStates = switchSense;
            } else {
                senseStates = waitSense;
            }
            break;
        case waitVel:
            if(IR1 && !(Gbtn)) {
                senseStates = irWait;
            } else if(!(IR1) && Gbtn) {
                senseStates = switchSense;
            } else {
                senseStates = waitVel;
            }
            break;
        case irWait:
            if(IR2) {
                senseStates = irCalc;
            } else {
                senseStates = irWait;
            }
            break;
        case irCalc:
            senseStates = waitVel;
            break;
        default:
            senseStates = init;
            break;
    }
    
    switch(senseStates) {
        case init:
            dcnt = 0;
            tcnt = 0;
            val = 1;
            vel = 0;
            fts = 83.3333;
            break;
        case waitSense:
            //write counter
            writeNum(dcnt,0);
            writeDisplay(dev7SEG);
            break;
        case sense:
            //action complete in transitions.
            break;
        case changeVal:
            //action complete in transitions.
            break;
        case setVal:
            (val) ? (dcnt = 0) : (dcnt = 120);
            break;
        case switchVel:
            vel = 0;
            break;
        case switchSense:
            dcnt = 0;
            break;
        case waitVel:
            //display vel
            writeNum(vel,0);
            writeDisplay(dev7SEG);
            //set counter to 0
            tcnt = 0;
            break;
        case irWait:
            tcnt++;
            break;
        case irCalc:
            //calculate velocity
            vel = fts / tcnt;
            break;
        default:
            senseStates = init;
            break;
    }    
}
          

