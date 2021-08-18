#include "i2c.h"
#include "config.h"
#include "1306.h"

void ssd1306_init(void){
  i2c_master_init(400000); // 400KHZ speed i2c mode
#if PROTEUS 
  __delay_ms(100); // VDD becomes stable
  PORTDbits.RD0 = 0; // SET RESET LOW
  TRISDbits.TRISD0 = 0; // RESET PIN OUTPUT MODE
  __delay_ms(10); // waiting for atleast 3us 
  PORTDbits.RD0 = 1; // HIGH
#endif
  command_mode(DISPLAY_OFF);
  command_mode(SET_DISPLAY_CLOCK);
  command_mode(0x80); // RATIO EDIT THIS FOR MYSELF
    
  command_mode(SET_MULTIPLEX_RATIO);
  command_mode(OLED_HEIGHT-1);
    
  command_mode(SET_DISPLAY_OFFSET);
  command_mode(0x00); //NO OFFSET
    
  command_mode(SET_DISPLAY_START_LINE | 0x00);
  command_mode(CHARGE_PUMP);
  //newly added
  if (SWITCHCAPVCC == EXTERNALVCC){ 
    command_mode(0x10); 
  }
  else{ 
    command_mode(0x14); 
  }
  // newly added
  command_mode(SET_MEMORY_ADDRESS);
  command_mode(0x00);
  command_mode(SET_SEGMENT_REMAP_1);
  command_mode(RIGHT_LEFT_OUTPUT_SCAN); 
  // 128_32 **
  command_mode(SET_COM_PINS_HARDWARE);          // 0xDA
  command_mode(0x02);
  command_mode(SET_CONTRAST);                   // 0x81
  command_mode(0x8F); // 
  // 128_32 **  
  command_mode(SET_PAGE_ADDRESS);
  command_mode(0x00); // PAGE 0
  command_mode(0x03); // PAGE 3 only for 128x32 OLED
  //NEWLY ADDED
  command_mode(SET_PRE_CHARGE);                  // 0xd9
  if (SWITCHCAPVCC == EXTERNALVCC){ 
    command_mode(0x22); 
  }
  else{ 
    command_mode(0xF1); 
  }
  // newly added  
  command_mode(SET_VCOMH_DESELECT);
  command_mode(0x40);
  clear_display();
  command_mode(DISPLAY_RESUME);
  command_mode(NORMAL_DISPLAY);
  command_mode(DEACTIVATE_SCROLL);
  command_mode(DISPLAY_ON);
}
void row_col(uint8_t start_row, uint8_t start_col){
  command_mode(SET_PAGE_ADDRESS);
  command_mode(start_row); //start
  command_mode(3); //end
  command_mode(SET_COLUMN_START_END_ADDRESS);
  command_mode(start_col);
  command_mode(127);
}

void command_mode(uint8_t command){
  i2c_master_start(); // START CONDITION OF I2C
  i2c_master_write(SLAVE_ADDRESS); // To communicate with 128x32 OLED display
  //use restart write below if needed still deciding
  i2c_master_write(0x00); // Command mode selected
  i2c_master_write(command); // Command to execute
  i2c_master_stop(); // STOP CONDITION OF I2C
}

void data_mode(uint8_t data){
  i2c_master_start(); // START CONDITION OF I2C
  i2c_master_write(SLAVE_ADDRESS); // To communicate with 128x32 OLED display
  i2c_master_write(0x40); // Data mode selected
  i2c_master_write(data); // Data to Display
  i2c_master_stop(); // STOP CONDITION OF I2C
}

void clear_display(void){
  command_mode(SET_COLUMN_START_END_ADDRESS);
  command_mode(0); //start
  command_mode(127); //end
  
  command_mode(SET_PAGE_ADDRESS);
  command_mode(0); //page0
  command_mode(3); //page3 only for 128x32bit
  for(uint16_t i = 0; i < (OLED_HEIGHT*OLED_WIDTH); i++){
    data_mode(0); // loop clears the data
  }
}

void oled_char(const char c){
  // char can only store 256 values hence alpha was full and
  // extension to alpha1 was made
  uint8_t original = 0; //uint8_t is the same as "unsigned char"
  if(c < 83){ // 83 is decimal in 'S'
    original = (c - 32)*5; //resets it to '0' in alpha
    for(uint8_t i = original; i < (original+5); i++){
      data_mode(alpha[i]);
    }
  }
  // NOTE: I GOT A LOT OF PROBLEMS NOT USING "ELSE IF" for multiple conditions
  // HOWEVER, im using "if" and "else" hence no issues
  else{
    original = (c - 83)*5; // resets it to '0' in alpha1 
    for(uint8_t j = original; j < (original+5); j++){
      data_mode(alpha1[j]);
    }
  }    
  data_mode(0x00); // 1 space
}

void oled_string(char *msg){ // '*msg' points to memory location
  while( *msg != 0x00 ){ // checks to see if pointer is empty
  oled_char(*msg); // prints value stored in pointer memory
  msg++; // next increment in memory of pointer
  }    
}

void oled_space(uint8_t spacing){
  for(uint8_t i = 0; i < spacing; i++){
    data_mode(0x00); //spacing
  }
}

void battery(uint8_t spacing, uint8_t percentage){
  uint8_t i = 0;
  percentage /= 10;
  for(i = 0; i < spacing; i++){
    data_mode(0x00); //spacing
  }
  data_mode(0xff);
  for(i = 0; i < percentage; i++){
    data_mode(0xff);
  }
  for(i = 0; i < (10 - percentage); i++){
    data_mode(0b10000001);
  }
  data_mode(0xff);
  data_mode(0x00);
  data_mode(0b00011000);
}

void fill_screen(void){
  row_col(0,0);
  for(uint16_t i = 0; i < (OLED_WIDTH*OLED_HEIGHT); i++){
    data_mode(0xff);
  }
}

void scroll_init(char direction, uint8_t start_page, uint8_t end_page){
  if(direction == 'r'){
    command_mode(RIGHT_HORIZONTAL_SCROLL);
    command_mode(0); // dummy
    command_mode(start_page); // start page address
    command_mode(0x00); // frame frequency 5 selected check datasheet to change it
    command_mode(end_page); // end page address 
    command_mode(0); // dummy
    command_mode(0xFF); // dummy
    command_mode(ACTIVATE_SCROLL);
  }
  else if(direction == 'l'){
    command_mode(LEFT_HORIZONTAL_SCROLL);
    command_mode(0); // dummy
    command_mode(start_page); // start page address
    command_mode(0x00); // frame frequency 5 selected check datasheet to change it
    command_mode(end_page); // end page address 
    command_mode(0); // dummy
    command_mode(0xFF); // dummy
    command_mode(ACTIVATE_SCROLL);
  }  
}
