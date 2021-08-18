#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "i2c.h"
#include "1306.h"
#include "dht22.h"
#include "uart.h"

void oled_startup(void);
void oled_default(void);

void main(void){ 
  /* DHT variables */
  uint16_t temp; 
  uint16_t rh;
  uint8_t check_sum; 
  uint8_t byte_1; 
  uint8_t byte_2; 
  uint8_t byte_3; 
  uint8_t byte_4;
  char buffer[4];
  
  /* Reset Pin for OLED in Proteus */
  TRISDbits.TRISD1 = 0;
  PORTDbits.RD1 = 0;
  /* System setup */  
  uart_init(); // Initializes the UART
  ssd1306_init(); // Initializes the OLED 
  timer1_init(); // Initializes TIMER1 for DHT22 timing
  dht22_init(); // Initializes DHT22
  /* Startup on OLED */
  oled_startup();
  /* Initial values on OLED */
  oled_default();
  while(1){
    start_signal(); // Send the starting signal to DHT22
    if(response()){ // Checks if the DHT22 sends a response to MCU
      byte_1 = read_data(); // read and store first 8bits
      byte_2 = read_data(); // read and store second 8bits
      byte_3 = read_data(); // read and store third 8bits
      byte_4 = read_data(); // read and store fourth 8bits
      check_sum = read_data(); // read and store fifth 8bits
      // HUMIDITY
      rh = ( (byte_1 << 8) | byte_2 ) / 10;
      //  TEMPERATURE
      temp = ( (byte_3 << 8) | byte_4 ) / 10;
      sprintf(buffer, "%d", temp);
      uart_send_char('T');
      uart_send_string(buffer);
      row_col(0,0);
      oled_string("TEMP");
      battery(20, temp);
      oled_space(20);
      oled_string(buffer);     
      //RH
      sprintf(buffer, "%d", rh);
      uart_send_char('H');
      uart_send_string(buffer);
      row_col(2,0);
      oled_string("HUMID");
      battery(14, rh);
      oled_space(19);
      oled_string(buffer); //humid value
    }
    else{}    
  }
}

void oled_startup(void){
  /* Percentage Variables */
  uint8_t percent = 0;
  char buff[4] = "  ";
  row_col(0,0);
  oled_string("TEMP&HUMID SENSOR");
  row_col(1,0);
  for(uint8_t i = 0; i < 101; i++){ 
    __delay_ms(5);
    row_col(1, 50);
    sprintf(buff, "%d", percent);
    oled_string(buff);
    oled_char('%');
    percent++;
  }
}

void oled_default(void){
  clear_display();
  oled_string("TEMP");
  battery(20, 0);
  oled_space(20);
  oled_string("00.0 C");
  row_col(2,0);
  oled_string("HUMID");
  battery(14, 0);
  oled_space(19);
  oled_string("00.0 %");
}
