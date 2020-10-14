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

//dro.flags_1
#define FLAG_MINUS_SIGN     0
#define FLAG_INCHES         3
#define FLAG_DATA_OK        4
#define FLAG_LAST_READ_LOW  6
#define FLAG_CLK_IDLE       7

//dro.flags_2
#define FLAG_DATA_READY     0     
#define FLAG_INCH_ONLY      1 

#define CHK_IDLE(dro_ptr) (dro_ptr->flags_1 & (1 << FLAG_CLK_IDLE))
#define CHK_LAST_READ_LOW(dro_ptr) (dro_ptr->flags_1 & (1 << FLAG_LAST_READ_LOW))

typedef struct {
  uint8_t scale_type;
  uint8_t high_bits_count;    // the number of consecutive high bits
  uint8_t current_bit;        // the next bit to be read
  uint32_t buffer;            // buffer to hold the value while reading
  uint32_t last_good_value;   // last value known to be good
  uint8_t last_good_flags;    // at least we need the sign 
  char* label;                // the name to display
  uint8_t clk_pin;            // pin for clock line
  uint8_t data_pin;           // pin for data line
  double millimeters;         // last good value in millimeters (signed)
  double inches;              // last good value in inches (signed)
  uint8_t flags_1;
  uint8_t flags_2;
} dro;

void update_dro_values(dro* dro_ptr);

void display_init();
void display_update(dro* dro_ptr);
