#include "lcd_hd44780.h"


void display_update(dro* dro_ptr,LiquidCrystal** lcd){
  char s[20];
  char* units;
  volatile uint8_t line;
  volatile uint8_t lcd_pos;

  if (strcmp(dro_ptr->label,"X") == 0){
    line = 0;
    lcd_pos = LCD_TOP; 
  }else if(strcmp(dro_ptr->label,"Y") == 0){
    line = 1;
    lcd_pos = LCD_TOP;
  }

  if(dro_ptr->flags_1 & (1 << FLAG_INCHES)){
    dtostrf(dro_ptr->inches/10000,20,4,s);
    lcd[lcd_pos]->setCursor(22-strlen(s),line);
    units = (char*)&" inch";
  }else{
    dtostrf(dro_ptr->millimeters/100,20,2,s);
    lcd[lcd_pos]->setCursor(24-strlen(s),line);
    units = (char*)&" mm";
  }  

  lcd[lcd_pos]->print(s);
  lcd[lcd_pos]->print(units);
/*
  lcd[lcd_pos]->print(s);
  lcd[lcd_pos]->print(units);
  */
}
