// Library SPBRG value for 16MHZ crystal clock
#include "uart.h"
#include "config.h"

void uart_send_char(char character){
  while(!PIR1bits.TXIF)
  ;  // hold the program till TX buffer is free and placed on a separate line to remove the warning
  TXREG = character; //Load the transmitter buffer with the received value
}

char uart_get_char(void){
  if(RCSTAbits.OERR){
    RCSTAbits.CREN = 0; //If error -> Reset 
    RCSTAbits.CREN = 1; //If error -> Reset 
  }
  while(!PIR1bits.RCIF);  // hold the program till RX buffer is free
  return RCREG; //receive the value and send it to main function
}

void uart_send_string(char* string){
  while(*string) //if there is a char
  uart_send_char(*string++); //process it as a byte data
}

void uart_init(void){ // without an interrupt
  TRISCbits.TRISC6 = 0; //TX pin set as output
  TRISCbits.TRISC7 = 1; //RX pin set as input
    
  SPBRG = 103; // 9600 BPS speed selected at high speed for 16MHZ
  TXSTAbits.BRGH = 1; //High speed baud rate
    
  TXSTAbits.SYNC = 0; // Asynchronous mode
  RCSTAbits.SPEN = 1; // Serial Port Enabled for RX & TX on PORTC
  TXSTAbits.TXEN = 1; // Enables transmission
  RCSTAbits.CREN = 1; // Enables reception 
  TXSTAbits.TX9 = 0; // 8 bits mode for transmitting
  RCSTAbits.RX9 = 0; // 8 bits mode for receiving
}
