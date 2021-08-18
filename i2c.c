#include "config.h"
#include "i2c.h"

void i2c_master_init(long int freq){
  //Set RC3 and RC4 set as input for SCL and SDA
  TRISCbits.TRISC3 = 1; 
  TRISCbits.TRISC4 = 1; 
  // I2C Master mode, clock = FOSC / (4 * (SSPADD+1)) 
  SSPCONbits.SSPM0 = 0;
  SSPCONbits.SSPM1 = 0;
  SSPCONbits.SSPM2 = 0;
  SSPCONbits.SSPM3 = 1;
  SSPADD = (_XTAL_FREQ/(4*freq))- 1; // 100KHZ or 400KHZ or 1MHZ
  //Enables the serial port and configures the SDA and SCL pins 
  //as the source of the serial port pins
  SSPCONbits.SSPEN = 1; 
}

void i2c_master_start(void){
  i2c_master_wait();
  SSPCON2bits.SEN = 1; // START CONDITION
}

void i2c_master_wait(void){
  while((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}

void i2c_master_write(unsigned char data){
  i2c_master_wait();
  SSPBUF = data;
}

void i2c_master_stop(void){
  i2c_master_wait();
  SSPCON2bits.PEN = 1; // STOP CONDITION
}
