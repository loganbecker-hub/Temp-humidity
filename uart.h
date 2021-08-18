#ifndef _UART_H
#define _UART_H
void uart_send_char(char character);
char uart_get_char(void);
void uart_send_string(char* string);
void uart_init(void);
#endif