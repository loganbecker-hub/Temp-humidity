#ifndef _I2C
#define _I2C
void i2c_master_init(long int freq);
void i2c_master_start(void);
void i2c_master_wait(void);
void i2c_master_write(unsigned char data);
void i2c_master_stop(void);
#endif