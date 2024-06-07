#include <Wire.h>;
#include <LiquidCrystal_I2C.h>;

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

int sda_pin = D2; // pin to connect to SDA pin on LCD (can be any data pin)
int scl_pin = D1; // pin to connect to SCL pin on LCD (can be any data pin)

float temperature = 130.7;

float humidity = 138.1;

LiquidCrystal_I2C lcd(0x27,20,4); // start up LCD as 20 characters X 4 lines

uint8_t heart[8] = {0x0, // 0000000 Create a 5X7 bit heart character using pattern
                   0xa,  // 0001010
                   0x1f, // 0011111
                   0x1f, // 0011111
                   0xe,  // 0001110
                   0x4,  // 0000100
                   0x0}; // 0000000



void setup() {
  Wire.begin(2, 0);
  lcd.begin();
  lcd.backlight();
  lcd.createChar(1, heart);   //Store the heart character in byte[1] in the LCD
}

void loop() {
  lcd.home();
  lcd.setCursor(0, 0);
  lcd.print("Node: 01");
  lcd.setCursor(13, 0);
  lcd.print("Mode: 1");
  lcd.setCursor(0, 1);               // set cursor to 2nd position on line 2
//  lcd.write(byte(1));                // write heart character stored in byte[1]
  lcd.print("Temperature:");
  lcd.print(temperature,1);
  lcd.setCursor(17,1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0, 2);               // set cursor to 0th position on line 3
  lcd.print("Humidity:   ");
  lcd.print(humidity,1);
  lcd.setCursor(17, 2);               
  lcd.print("%");
  lcd.setCursor(0, 3);               // set cursor to 6th position on line 4
  lcd.print("ppm C0: ");
  lcd.setCursor(17, 3);               // set cursor to 6th position on line 4
  lcd.print("ppm");
  delay(1000);
}
