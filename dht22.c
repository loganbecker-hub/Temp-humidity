#include "config.h"
#include "dht22.h"

void dht22_init(void){
  __delay_ms(1000); // Wait for dht sensor to stabilize
  DHT22_DIR_REG = 0; // Set to Output mode
  DHT22_DATA_REG = 1; // High Output
}

// THE DHT22 sensor options in proteus, 'time select low'
void start_signal(void){
  DHT22_DIR_REG = 0; // Output mode
  DHT22_DATA_REG = 0; // Low output
  __delay_ms(25); // For sensor to detect the start signal
  PORTCbits.RC5 = 1; // High output
  __delay_us(30); // waiting for the response from DHT22 
  DHT22_DIR_REG = 1; // Changed to input mode to receive response and data
}

bool response(void){
  /* RESETTING the timer values */
  TMR1L = 0; 
  TMR1H = 0; 
  T1CONbits.TMR1ON = 1; // Turn on TIMER1 
  while(!DHT22_DATA_REG && TMR1L < 100); // counts that 80us low voltage
  if(TMR1L > 99){return false;} // returns not working
  else{
    TMR1L = 0; 
    TMR1H = 0; // resets for the next 80us high voltage count 
    while(DHT22_DATA_REG && TMR1L < 100); // counts that 80us high voltage
    if(TMR1L > 90){return false;} //returns not workings
    else{
      return true;
    }
  }    
}

uint8_t read_data(void){
  uint8_t data = 0;
  for(uint8_t i = 0; i < 8; i++){
    TMR1L = 0; 
    TMR1H = 0; //TIMER1 reset
    while(!DHT22_DATA_REG); // Waits for 50us low voltage to go high
    if(TMR1L > 100){//Lcd_clear_4bit(); Lcd_string_4bit("Error reading Data");
    }
    else{
      TMR1L = 0; 
      TMR1H = 0; // RESET TIMER1 to check if data is '0' or '1'
      while(DHT22_DATA_REG); // Waits for DATA bit to go low
      if(TMR1 > 50){ // Data bit is '1'
        data |= 1U << (7U - i); // shifts a bit into the 8bit char data1 variable
      }
    }
  }
  return data; //returns the 8 bit value for reading
}

//Timer1 to detect the response from the DHT22 and 40bit data as well
void timer1_init(void){
  T1CONbits.TMR1CS = 0; // clock source is internal 16MHZ/4 osc
  // pre-scale 4 to give 1MHZ clock for timer1 for 1us period for DHT22
  T1CONbits.T1CKPS0 = 0; 
  T1CONbits.T1CKPS1 = 1; 
  // Making sure the timer is off 
  T1CONbits.TMR1ON = 0;   
}
