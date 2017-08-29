#include <Keypad.h>
#include <Password.h>
#include <Wire.h> //Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h>//Vorher hinzugefügte LiquidCrystal_I2C //Bibliothek einbinden
#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <Adafruit_GPS.h>

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
int LED1=48; 
int LED2=49; 

//SDCARD AND SPEAKER 
#define CHIP_SELECT_PIN  53
#define SPEAKER_PIN      11
TMRpcm tmrpcm;

//IRM Odul
int trigger=7; 
int echo=6; 

//GPS
HardwareSerial mySerial = Serial1;
Adafruit_GPS GPS(&Serial1);

//SERVO
Servo servoblau;


//LOGIC
boolean passOK = false;
int status =0;

void setup() {
 Serial.begin(9600);
 initLCD();
 initSdAndSpeaker();
 initIR();
 initGPS();
 
 pinMode(LED1, OUTPUT);
 digitalWrite(LED1, LOW);
 pinMode(LED2, OUTPUT);
 digitalWrite(LED2, LOW);
}

void loop() {

if (status == 0){
  //Play Audio
   //Check SD CARD   
  if ( !SD.begin( CHIP_SELECT_PIN ) )
    Serial.println( "SD fail" );
  else {
    //Todo: Check if GPS is fix
    status =1;
    Serial.println( "SD OK" );
    //tmrpcm.play( "OOT_Secret.wav" );
    delay( 5 );
  } 
}
if (status ==1){
    digitalWrite(LED1, HIGH);
    //tmrpcm.play( "OOT_Secret_16b.wav" );
    delay( 5 );
    status = 2;
    delay(5000);//laenge von audio
}
if (status == 2){
  if (checkEntfernung()){
    digitalWrite(LED2, HIGH);
    status =3;     
    //tmrpcm.play( "KnowHow2.wav" );
    tmrpcm.play( "6.wav" );
    Serial.println( "IR Test fertig" );
    delay(1000); 
  } else {
    Serial.println( "IR Test geht weiter" );
    delay(1000);  
  }
}
if (status ==3){  
  Serial.println("Status 3");
if ( GPS.fix ){
  Serial.println("GPS FIX");
  //tmrpcm.play( "erklaerung.wav" );
  //delay(1000);//laenge von audio
  //tmrpcm.play( "ort1.wav" );
  //todo check ort 1
  delay(1000);//laenge von audio   
  tmrpcm.play( "6.wav" );      
  if (isAtLocationOne()){
    delay(100);//laenge von audio  
    status = 4;  
    tmrpcm.play( "6.wav" );      
  }  
} else {
  Serial.println("GPS NO FIX"); 
}
}
if (status == 4){  
  //tmrpcm.play( "ort2.wav" );
  //todo check ort 2
  delay(1000);//laenge von audio   
  tmrpcm.play( "6.wav" );  
  //tmrpcm.play( "nummer.wav" );     
  writeOnLCDCenter("080033012345", 2);
  status = 5;
}

if (status == 5) {    
  if (!passOK){      
    char key = keypad.getKey();
    if (key != NO_KEY){
        delay(20);               
          writePINInputOnLCD();
          passOK = processNumberKey(key);                        
        } 
    } else {
     // tmrpcm.play( "OOT_Secret.wav" );
      status =6;
    }
  }

if (status == 6) {
  writeUnlock();  
  servoblau.attach(5);    
  servoblau.write(180);
  delay(3000);
  servoblau.detach();
  status =7;
}  

if (status == 7){
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

void initGPS(){
  GPS.begin(9600);  
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); 
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);
  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);
  delay(1000);
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
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


boolean isAtLocationOne(){
 float targetLatitudeDegrees = 49.34939956;
 float targetLongitudeDegrees = 8.16590690;                               

 float diffLatitude = targetLatitudeDegrees - GPS.latitudeDegrees;
 float diffLongitude = targetLongitudeDegrees - GPS.longitudeDegrees;

 Serial.println("");
 Serial.print("diffLatitude = ");
 Serial.print(diffLatitude, 8);
 Serial.println("");
 Serial.print("diffLongitude = ");
 Serial.print(diffLongitude, 8);
 if (diffLatitude < 0.0004 && diffLatitude > -0.0004 &&
      diffLongitude < 0.0004 && diffLongitude > -0.0004){
        Serial.println("");
        Serial.println("On Target");
        return true;
      } else {
        return false;
      }
 
}




