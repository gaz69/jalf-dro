#include <Arduino.h>
#include "jalf_dro.h"
#include <stdio.h>
#include <LiquidCrystal.h>
#include "lcd_hd44780.h"

dro dro_array[2];
LiquidCrystal* lcd[2];


void setup(){
  Serial.begin(9600);
  init_display();
  
  dro* dro_ptr;

  //init dros
  dro_ptr = &dro_array[0];
  memset(dro_ptr,0,sizeof(dro));
  dro_ptr->scale_type = 2;
  dro_ptr->label = "X";
  dro_ptr->clk_pin = 3;
  dro_ptr->data_pin = 2;
  dro_ptr->flags_1 |= (1 << FLAG_CLK_IDLE);
    
  dro_ptr = &dro_array[1];
  memset(dro_ptr,0,sizeof(dro));
  dro_ptr->scale_type = 2;
  dro_ptr->label = "Y";
  dro_ptr->clk_pin = 15;
  dro_ptr->data_pin = 14;
  dro_ptr->flags_1 |= (1 << FLAG_CLK_IDLE);
  
  cli();                    // disable interrupts

  //init timer registers
  TCNT2 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;

  TCCR2A |= (1 << WGM21);   // CTC mode
  TCCR2B |= (1 << CS21);    // prescaler 1/8
  OCR2A = 50;               // 25us (50 * 0.5us)
  TIMSK2 |= (1 << OCIE2A);  // enable output compare interrupt for OCR2A

  sei();                    // enable interrupts
}

void loop(){

  char s[50];
  for (int i = 0; i < 2; i++ ){
    dro* dro_ptr = &dro_array[i];
    if (dro_ptr->flags_2 & (1 << FLAG_DATA_READY)){
      dro_update_millimeters(&dro_array[i]);
      display_update(dro_ptr,lcd);
      dro_ptr->flags_2 &= ~(1 << FLAG_DATA_READY);
    }
  }
}

void dro_update_millimeters(dro* dro_ptr){
  bool from_inches = false;

  switch (dro_ptr->scale_type){
    case (2):
      if (dro_ptr->flags_1 & (1 << FLAG_INCHES)){
        from_inches = true;
      }else if(dro_ptr->flags_2 & (1 << FLAG_INCH_ONLY)){
        from_inches = true;
      }
      break;
    case (1):
      // always convert from inches
      from_inches = true;
    default:
      break;
  }
  
  if (from_inches){
    dro_ptr->inches = ((dro_ptr->last_good_value >> 1)*10 + 5*(dro_ptr->last_good_value & 1)); 
    dro_ptr->millimeters = dro_ptr->inches * .254;
    if (dro_ptr->flags_1 & (1 << FLAG_MINUS_SIGN)){
      dro_ptr->inches *= -1;
      dro_ptr->millimeters *= -1;
    }
  }else{
    dro_ptr->millimeters = dro_ptr->last_good_value;
    if (dro_ptr->flags_1 & (1 << FLAG_MINUS_SIGN)){dro_ptr->millimeters *= -1;}
    dro_ptr->inches = round(dro_ptr->millimeters / .254);
  }
}



//timer2 interrupt: this is the polling interrupt
ISR(TIMER2_COMPA_vect){
  //return;
  dro* dro_ptr;
  for (int i = 0; i < 2; i++){
    dro_ptr = &dro_array[i];
    if (CHK_IDLE(dro_ptr)){                                 // when the clock is idle we are only looking for a HI-->LO transition       
      if (digitalRead(dro_ptr->clk_pin) == LOW){            // .. found it
        dro_ptr->flags_1 = 0;                               // clear flags    
        dro_ptr->flags_2 &= ~(1 << FLAG_DATA_READY);        // data is not ready 
        
        dro_ptr->buffer = 0;                                // clear input buffer
        dro_ptr->high_bits_count = 0;                       // no high bits
        dro_ptr->current_bit = 0;                           // next bit to read
        
        dro_ptr->flags_1 |= (1 << FLAG_LAST_READ_LOW);      // remember that the last read was a LOW
      }
    }else{                                                  // clock is not idle and we are in a data burst 
                                                            // check a LO-->HI transition to read the data
                                                            // or in a HI-->LO transition to move to the next bit
      if (CHK_LAST_READ_LOW(dro_ptr)){                      // last read was a LOW and we have a 
        if (digitalRead(dro_ptr->clk_pin) == HIGH){         // .. LO-->HI transition
          if (digitalRead(dro_ptr->data_pin) == HIGH){      // check data line
            if (dro_ptr->current_bit < 20){                 // data belongs to value
              dro_ptr->buffer |= (1 << dro_ptr->current_bit); // update buffer 
            }else{                                          // data belongs to flags
              dro_ptr->flags_1 |= (1 << (dro_ptr->current_bit - 20)); // update flags
            }
          }
          dro_ptr->flags_1 &= ~(1 << FLAG_LAST_READ_LOW);   // remember that the last read was a HIGH
          dro_ptr->high_bits_count++;                       // .. count it

        }
        //ignore LO-->LO 
      }else{                                                // last read was a HIGH and we have a               
        if (digitalRead(dro_ptr->clk_pin) == LOW){          // .. HI-->LO transition
          dro_ptr->flags_1 |= (1 << FLAG_LAST_READ_LOW);    // remember that the last read was a LOW
          dro_ptr->high_bits_count = 0;                     // reset the counter for the high bits
          dro_ptr->current_bit++;                           // move to the next bit
        }else{                                              // HI-->HI cannot be ignored when in data burst
          dro_ptr->high_bits_count++;                       // count the high bit
          if (dro_ptr->high_bits_count > 30){               // clock has been high for 750us continously  
            dro_ptr->flags_1 |= (1 << FLAG_CLK_IDLE);       // .. it means end of transmission 
                                                            // .. change state to idle
            if (dro_ptr->current_bit == 23){                // check if all data bits were read
              dro_ptr->flags_1 |= (1 << FLAG_DATA_OK);      // .. if so set the status of the data to ok
              dro_ptr->last_good_value = dro_ptr->buffer;   // ..  save the new data
              dro_ptr->last_good_flags = dro_ptr->flags_1;  // ..  and the flags
              dro_ptr->flags_2 |= (1 << FLAG_DATA_READY);
            }  
          }
        }
      }
    }
  }
}

void init_display(){
  lcd[LCD_TOP] = &LiquidCrystal(LCD_RS,LCD_ENABLE1,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
  lcd[LCD_BOTTOM] = &LiquidCrystal(LCD_RS,LCD_ENABLE2,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
  lcd[LCD_TOP]->begin(27,2);
  lcd[LCD_BOTTOM]->begin(27,2);

  lcd[LCD_TOP]->print("X");
  lcd[LCD_TOP]->setCursor(0,1);
  lcd[LCD_TOP]->print("Y");
  
//  lcd[LCD_BOTTOM]->print("2abcdefghijklmnopqrstuvwxyz");
  lcd[LCD_BOTTOM]->setCursor(0,1);
  lcd[LCD_BOTTOM]->print("AR-DRO V1.0 JALF 2020.10.14");

}
