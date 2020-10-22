#ifndef __LCD_HD44780_H__
#define  __LCD_HD44780_H__

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "jalf_dro.h"

//lcd
#define LCD_TOP             0
#define LCD_BOTTOM          1

#define LCD_RS              10
#define LCD_ENABLE1         9
#define LCD_ENABLE2         8
#define LCD_D4              4
#define LCD_D5              5
#define LCD_D6              6
#define LCD_D7              7



void display_init();
void display_update(dro* dro_ptr,LiquidCrystal** lcd);

#endif /*#ifndef __LCD_HD44780_H__*/
