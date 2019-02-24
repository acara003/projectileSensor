#ifndef USART_H
#define USART_H

void USART_init(unsigned char num, unsigned int ubrr) {
    if(num != 1) {
        /*Set baud rate */
        UBRR0H = (unsigned char)(ubrr>>8);
        UBRR0L = (unsigned char)ubrr;
        /*Enable receiver and transmitter */
        UCSR0B = (1<<RXEN0)|(1<<TXEN0);
        /* Set frame format: 8data, 2stop bit */
        //UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
        UCSR0C = (1<<USBS0)|(3<<UCSZ00);
    } else {
        /*Set baud rate */
        UBRR1H = (unsigned char)(ubrr>>8);
        UBRR1L = (unsigned char)ubrr;
        /*Enable receiver and transmitter */
        UCSR1B = (1<<RXEN1)|(1<<TXEN1);
        /* Set frame format: 8data, 2stop bit */
        UCSR1C = (1<<USBS1)|(3<<UCSZ11);
        //UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
    }
}

unsigned char USART_IsSendReady(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << UDRE0)) : (UCSR1A & (1 << UDRE1));
}

unsigned char USART_HasTransmitted(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << TXC0)) : (UCSR1A & (1 << TXC1));
}

unsigned char USART_HasReceived(unsigned char usartNum)
{
	return (usartNum != 1) ? (UCSR0A & (1 << RXC0)) : (UCSR1A & (1 << RXC1));
}

void USART_Flush(unsigned char usartNum)
{
    static unsigned char dummy;
    if (usartNum != 1) {
        while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
    }
    else {
        while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
    }
    (void)dummy;
}
void USART_send(unsigned char data, unsigned char num) {
    if(num != 1) {
        /* Wait for empty transmit buffer */
        while( !( UCSR0A & (1<<UDRE0)) );
        /* Put data into buffer, sends the data */
        UDR0 = data;
    } else {
        /* Wait for empty transmit buffer */
        while( !( UCSR1A & (1<<UDRE1)) );
        /* Put data into buffer, sends the data */
        UDR1 = data;
    }
}

unsigned char USART_receive(unsigned char num) {
    if(num != 1) {
        /* Wait for data to be received */
        while( !(UCSR0A & (1<<RXC0)) );
        /* Get and return received data from buffer */
        return UDR0;
    } else {
        /* Wait for data to be received */
        while( !(UCSR1A & (1<<RXC1)) );
        /* Get and return received data from buffer */
        return UDR1;
    }
}

#endif
