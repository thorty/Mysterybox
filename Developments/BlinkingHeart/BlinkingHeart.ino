#include <Wire.h>
#include <LiquidCrystal_I2C.h>
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
  
  lcd.createChar(1, heart_h);
  lcd.createChar(2, heart_f);  
  lcd.home();
  
  lcd.setCursor(0, 0);
  lcd.print("Hello world...");
  lcd.setCursor(0, 1);
  lcd.print(" i ");
  lcd.write(byte(1));
  lcd.print(" arduinos!");
  delay(5000);
  lcd.clear();
   lcd.setCursor(0, 0);
}




int z = 0;
void loop()
{
  /*
z= z+1;  
lcd.setCursor(0,0);
lcd.print(char(z)); 
if (z==2)z=0;
delay(500);
*/
writeUnlockTwo();

}




void writeUnlockTwo(){
  int i =0;  
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
  lcd.createChar(1, heart_h);
  lcd.createChar(2, heart_f);  
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  
  while(i<80){
      lcd.print(char(1)); 
      i++;
    }  
  delay(500);
  while(i>0){
      lcd.print(char(2)); 
      i--;
    }    
  delay(500);
}

