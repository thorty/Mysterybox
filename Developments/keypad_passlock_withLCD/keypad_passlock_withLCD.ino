#include <Keypad.h>
#include <Password.h>
#include <Wire.h> //Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h>//Vorher hinzugefügte LiquidCrystal_I2C //Bibliothek einbinden

/**
 * 
 * Arduino Pin Belegung 
 *  GND am Keypad (links PIN 30)
 *  22, 24, 26, 28 (von rechts)
 *  LED 52 (100 Ohm an GND)
 */

//LCD SETUP
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//PWD SETUP
Password password = Password( "1234" );
int currentPasswordLength = 0;
int maxPasswordLength = 4;

//NUMPAD SETUP
const byte ROWS = 1; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
    {'3','4','1','2'}
};
byte rowPins[ROWS] = {30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//LED SETUP
//int LED=52; 

//LOGIC
boolean passOK = false;


void setup() {
  Serial.begin(9600);
  initLCD();
   
 
 //pinMode(LED, OUTPUT);
 //digitalWrite(LED, LOW); // Schalte die LED aus.
 
}

void loop() {
  if (!passOK){      
   char key = keypad.getKey();
   if (key != NO_KEY){
      delay(60);               
        writePINInputOnLCD();
        passOK = processNumberKey(key);                        
      } 
  } else {
    writeUnlock();
  }
}

void initLCD(){
  lcd.begin(20,4); //Das Display wird gestartet und die Größe des Displays wird angegeben(Jeweils 20 Zeichen in 4 Zeilen)
  lcd.backlight();//Beleuchtung des Displays einschalten    
  lcd.setCursor(0,0);
}


boolean processNumberKey(char key) {
    boolean passOk = false;
   //Serial.print(key);
   currentPasswordLength++;
   password.append(key);
   if (currentPasswordLength == maxPasswordLength) {
      passOk = checkPassword();
   } 
   return passOk;
}

boolean checkPassword() {
  boolean passOk = false;
   if (password.evaluate()){
      lcd.clear();      
      //digitalWrite(LED, HIGH); // Schalte die LED an.
      passOk = true;
   } else {
//      digitalWrite(LED, LOW); // Schalte die LED aus.
      lcd.clear();            
      writeOnLCDCenter("Leider falsch",1); 
      writeOnLCDCenter("Nicht aufgeben!",2);                  
      delay(2000);
      lcd.clear();
      Serial.println(" Wrong passwowrd!");
   } 
   resetPassword();
   return passOk;
}

void resetPassword() {
   password.reset(); 
   currentPasswordLength = 0; 
}

void writeUnlock(){
  int i =0;  
  uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
  lcd.createChar(0, heart);    
  lcd.home();
    while(i<80){
      lcd.write((byte)0);
      i++;
    }  
  delay(400);
  lcd.noDisplay();
  delay(400);
  // Turn on the display:
  lcd.display();
  
}

void writeOnLCDCenter(String lcdString, int row){
  lcd.setCursor(getCenterCursorPosition(lcdString),row);
  lcd.print(lcdString);
}

void writePINInputOnLCD(){
  lcd.setCursor(10 - (maxPasswordLength -currentPasswordLength)+2,1);
  lcd.print("*");
}

void writeOnLCDCenter(String lcdString){
  lcd.setCursor(getCenterCursorPosition(lcdString),1);
  lcd.print(lcdString);
}

int getCenterCursorPosition(String lcdString){
  int centerPosition = 10 - (lcdString.length()/2);
  return centerPosition;
}




