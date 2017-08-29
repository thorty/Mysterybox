#include <Wire.h>
#include <LiquidCrystal_I2C.h>

uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
uint8_t note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
uint8_t duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
uint8_t check[8] = {0x0, 0x1 , 0x3, 0x16, 0x1c, 0x8, 0x0};
uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
uint8_t retarrow[8] = {  0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

byte complete[8] = {
  B11111, B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte empty[8] = {
  B00000, B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte half_o[8] = {
  B11111, B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000
};
byte half_u[8] = {
  B00000, B00000,
  B00000,
  B00000,
  B11111, B11111,
  B11111,
  B11111
};



byte smiley_s[8] = {
  B00000, B01010,
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};


//kit mode
byte smiley_n1[8] = {
  B00000, B01010,
  B00000,
  B00000,
  B01111,
  B10000,
  B00000,
  B00000
};
byte smiley_n2[8] = {
  B00000, B01010,
  B00000,
  B00000,
  B00111,
  B11000,
  B00000,
  B00000
};
byte smiley_n3[8] = {
  B00000, B01010,
  B00000,
  B00000,
  B00011,
  B11100,
  B00000,
  B00000
};
byte smiley_n4[8] = {
  B00000, B01010,
  B00000,
  B00000,
  B00001,
  B11110,
  B00000,
  B00000
};
byte smiley_n5[8] = {
  B00000, B01010,
  B00000,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
};
//ENDE KIT MODE


byte heart_h[8] = {
  B00000,
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100, B00000
};
byte heart_f[8] = {
  B00000,
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100, B00000
};


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  lcd.begin(20, 4);
  lcd.backlight();
  lcd.setCursor(0, 0);

  lcd.begin(20, 4);
  lcd.backlight();

  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);
  lcd.createChar(7, retarrow);
  lcd.home();

  lcd.print("Hello world...");
  lcd.setCursor(0, 1);
  lcd.print(" i ");
  lcd.write(3);
  lcd.print(" arduinos!");
  delay(5000);
  displaySmilie();
}

// display all keycodes
void displayKeyCodes(void) {
  uint8_t i = 0;

  while (1) {
    lcd.clear();
    lcd.print("Codes 0x");
    lcd.print(i, HEX);
    lcd.print("-0x");
    lcd.print(i + 16, HEX);
    lcd.setCursor(0, 1);

    for (int j = 0; j < 16; j++) {
      lcd.write(i + j);
    }
    i += 16;

    delay(4000);
  }
}

void displaySmilie() {

  lcd.clear(); //clear the screen
  lcd.backlight(); // turn on back light
  lcd.setCursor(0, 0); //set cursor to upper left corner
  lcd.createChar (0, smiley_s); // load smiley to memory 0
  lcd.createChar (1, smiley_n1); // load smiley to memory 0
  lcd.createChar (2, smiley_n2);
  lcd.createChar (3, smiley_n3);
  lcd.createChar (4, smiley_n4);
  lcd.createChar (5, smiley_n5);
  lcd.createChar (6, complete);
  lcd.createChar (7, half_o);
  lcd.createChar (8, half_u);
  lcd.setCursor(1, 2);
  lcd.write(byte(0));

  lcd.write(byte(1));
  lcd.write(byte(3));
  lcd.write(byte(4));
  lcd.write(byte(5));



}
int z = 0;
void loop()
{
  /*
    z= z +1;
    lcd.setCursor(0,0); //set cursor to bottom left second line
    lcd.print(char(z));
    if (z==5)z=0;
  */
  writeBigSmilyCenter();
  delay(1000);
  while (z >0){
    speakingSmily();
  } 
  /*
    if (z % 2){
    writeBigSmilySpeakingOneCenter();
    delay(500);
    writeBigSmilySpeakingTwoCenter();
    } else {
    writeBigSmilySpeakingThreeCenter();
    }
  */
  z= z +1;
  // Do nothing here...
}

void writeBigSmilyEyes() {
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(10, 0);
  lcd.write(byte(6));
}

void writeBigSmilyCenter() {
  //Big Smily
  lcd.clear();
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(10, 0);
  lcd.write(byte(6));

  lcd.setCursor(7, 2);
  lcd.write(byte(8));
  lcd.setCursor(11, 2);
  lcd.write(byte(8));
  lcd.setCursor(8, 3);
  lcd.write(byte(7));
  lcd.write(byte(7));
  lcd.write(byte(7));
}

void speakingSmily(){
  lcd.clear();
  writeBigSmilySpeakingOneCenter();
  delay(800);
  lcd.clear();
  writeBigSmilySpeakingTwoCenter();  
  delay(800); 
}

void writeBigSmilySpeakingOneCenter() {  
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(10, 0);
  lcd.write(byte(6));

  lcd.setCursor(8, 2); 
  lcd.write(byte(8));   
  lcd.write(byte(8));  
  lcd.write(byte(8));
}

void writeBigSmilySpeakingTwoCenter() {
  lcd.clear();
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(10, 0);
  lcd.write(byte(6));

  lcd.setCursor(9, 3);
  lcd.write(byte(7));
  lcd.setCursor(9, 2);
  lcd.write(byte(7));
  lcd.setCursor(8, 2);
  lcd.write(byte(8));
  lcd.setCursor(10, 2);
  lcd.write(byte(8));
}




