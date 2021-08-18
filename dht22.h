#ifndef DHT22_h
#define	DHT22_h
#include <stdint.h>
#include <stdbool.h>

/* Define output pin used for DHT22 */
#define DHT22_DIR_REG  TRISCbits.TRISC5
#define DHT22_DATA_REG PORTCbits.RC5

void dht22_init(void);
void start_signal(void);
bool response(void);
uint8_t read_data(void);
void timer1_init(void);
#endif	