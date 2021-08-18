#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "i2c.h"
#include "1306.h"
#include "dht22.h"
#include "uart.h"

typedef struct dht22_data{
  uint8_t total[5];
  uint16_t rh;
  uint16_t temp;
}dht22_data;

void get_data(dht22_data *data);
void process_data(dht22_data *data);
void display_data(dht22_data *result);

void oled_startup(void);
void oled_default(void);

void main(void){
  /* DHT22 structure */
  dht22_data data;
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
      get_data(&data);
      process_data(&data);
      display_data(&data);
    }
    else{}  
  }
}

void get_data(dht22_data *data){
  for(uint8_t i = 0; i < 5; i++){
    data->total[i] = read_data();
  }
}

void process_data(dht22_data *data){
  data->rh = ( (data->total[0] << 8) | data->total[1] ) /10;
  data->temp = ( (data->total[2] << 8) | data->total[3]) /10;
}

void display_data(dht22_data *result){
  char buffer[4];
  sprintf(buffer, "%d", result->temp);
  uart_send_char('T');
  uart_send_string(buffer);
  row_col(0,0);
  oled_string("TEMP");
  battery(20, result->temp);
  oled_space(20);
  oled_string(buffer);
  sprintf(buffer, "%d", result->rh);
  uart_send_char('H');
  uart_send_string(buffer);
  row_col(2,0);
  oled_string("HUMID");
  battery(14, result->rh);
  oled_space(19);
  oled_string(buffer); //humid value
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
