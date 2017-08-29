#include <Keypad.h>
#include <Password.h>
#include <Wire.h> //Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h>//Vorher hinzugefügte LiquidCrystal_I2C //Bibliothek einbinden
#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>

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

//SDCARD AND SPEAKER
#define CHIP_SELECT_PIN  53
#define SPEAKER_PIN      11
TMRpcm tmrpcm;

//IRM Odul
int trigger=7; 
int echo=6; 

//LOGIC
boolean passOK = false;
int status =0;

void setup() {
  Serial.begin(9600);
  initLCD();
 initSdAndSpeaker();
 initIR();
 //pinMode(LED, OUTPUT);
 //digitalWrite(LED, LOW); // Schalte die LED aus.
 
}

void loop() {
if (status == 0){
  //Play Audio
   //Check SD CARD   
  if ( !SD.begin( CHIP_SELECT_PIN ) )
    Serial.println( "SD fail" );
  else {
    status =1;
    Serial.println( "SD OK" );
    //tmrpcm.play( "KnowHow.wav" );
    delay( 5 );
  } 
}
if (status ==1){
    //tmrpcm.play( "KnowHow.wav" );
    delay( 5 );
    status = 2;
    delay(5000);//laenge von audio
}
if (status == 2){
  if (checkEntfernung()){
    status =3;
    Serial.println( "IR Test fertig" );
  } else {
    Serial.println( "IR Test geht weiter" );
    delay(2000);  
  }
}
if (status ==3){
  initLCD();  
}
  
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
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);  
  lcd.begin(20,4);
  lcd.backlight();
  lcd.setCursor(0,0);
}
void initSdAndSpeaker(){  
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.volume(7); 
  //pinMode(53, OUTPUT);  
}

void initIR(){
  pinMode(trigger, OUTPUT); 
  pinMode(echo, INPUT); 
}


//IRLogik 
boolean checkEntfernung() {
  long dauer=0; 
  long entfernung=0; 
  digitalWrite(trigger, LOW);
  delay(5);
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer/2) * 0.03432;
  if (entfernung >= 500 || entfernung <= 0)
  {
  Serial.println("Kein Messwert");
  }
  else
  {
  Serial.print(entfernung);
  Serial.println(" cm");
  } 
  if (entfernung <= 10)
  {
    Serial.print("true");
   return true;
  }  
  else  //Und wenn das nicht so ist... 
  {
    Serial.print("false");
   return false;
  }  
  
}

//PASSLogik
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




