#include <Keypad.h> 
#include <Password.h>
#include <Wire.h> //Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> //to play wav file
#include <TMRpcm.h> //to play wav file
#include <SPI.h>
#include <SD.h> //to read SD
#include <Servo.h> 
#include <Adafruit_GPS.h>

/**

   Arduino Pin Belegung
    GND am Keypad (links PIN 30)
    22, 24, 26, 28 (von rechts)
    LED 52 (100 Ohm an GND)
*/

//LCD SETUP
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//PWD SETUP
Password password = Password( "2413" );
int currentPasswordLength = 0;
int maxPasswordLength = 4;

//NUMPAD SETUP
const byte ROWS = 1;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'3', '4', '1', '2'}
};
byte rowPins[ROWS] = {30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//LED SETUP
int LED1 = 8;
int LED2 = 49;

//SDCARD AND SPEAKER
#define CHIP_SELECT_PIN  53
#define SPEAKER_PIN      11
TMRpcm tmrpcm;

//IRM Odul
int trigger = 7;
int echo = 6;

//GPS
HardwareSerial mySerial = Serial1;
Adafruit_GPS GPS(&Serial1);
uint32_t timer = millis();
boolean usingInterrupt = false;
#define GPSECHO  false

//SERVO
Servo servoblau;


//LOGIC
boolean passOK = false;
int status = 0;
int gpsstatus = 1;

void setup() {  
  Serial.begin(9600);
  initLCD();  
  initIR();
  initGPS();
  initSdAndSpeaker();
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);
  delay(3000);

}

void loop() {

  if (status > 0 && status < 5) {    
    if (checkRestartCode()) {
      status = 1;
      gpsstatus=1;
      servoblau.attach(5);
      servoblau.write(120);      
      delay(1000);
      servoblau.detach();
      delay(1000);     
    }
  }

  if (status == 0) {
    //Check SD CARD
    if ( !SD.begin( CHIP_SELECT_PIN ) )
      Serial.println( "SD fail" );
    else {      
      status = 1;
      Serial.println( "SD OK" );
      digitalWrite(LED1, HIGH);
      delay( 1000 );
      digitalWrite(LED1, LOW);
    }
  }
  //SD OK, PLAY FILE
  if (status == 1) {
      servoblau.attach(5);
       delay(1000);            
      servoblau.write(60);
       delay(1000);
      servoblau.detach();
    
    writeBigSmilyCenter();
    digitalWrite(LED1, HIGH);
    delay( 1000 );
    digitalWrite(LED1, LOW);
    tmrpcm.play( "start.wav" );  
    lcd.clear();
    while (tmrpcm.isPlaying() == 1){
      speakingSmily();
      } 
    delay(1000);//laenge von audio
    lcd.clear();
    writeOnLCDCenter("Ist da jemand ?", 2);
    status = 2;
  }
  //IR Test
  if (status == 2) {    
    if (checkEntfernung()) {
      Serial.println( "IR Test fertig" );
      writeBigSmilyCenter();
      tmrpcm.play( "resolved.wav" );
      delay(2000);
      digitalWrite(LED1, HIGH);
      delay( 50 );
      digitalWrite(LED1, LOW);
      tmrpcm.play( "intro.wav" );
      //delay(32000);
      lcd.clear();
      while (tmrpcm.isPlaying() == 1){
        speakingSmily();
      }       
      status = 3;
    } else {
      Serial.println( "IR Test geht weiter" );
      delay(1000);
    }
  }
  //GPS Test1
  if (status == 3) {
    digitalWrite(LED1, HIGH);
    delay(1000);
    digitalWrite(LED1, LOW);
    delay(2000);

    // in case you are not using the interrupt above, you'll
    // need to 'hand query' the GPS, not suggested :(
    if (! usingInterrupt) {
      // read data from the GPS in the 'main loop'
      char c = GPS.read();
      // if you want to debug, this is a good time to do it!
      if (GPSECHO)
        if (c) Serial.print(c);
    }

    // if a sentence is received, we can check the checksum, parse it...
    if (GPS.newNMEAreceived()) {
      // a tricky thing here is if we print the NMEA sentence, or data
      // we end up not listening and catching other sentences!
      // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
      //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

      if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
        return;  // we can fail to parse a sentence in which case we should just wait for another
    }



    Serial.println("Status 3");

    if ( GPS.fix) {
      Serial.println("GPS FIX");
      if (gpsstatus == 1) {
        lcd.clear();
        tmrpcm.play( "ort1.wav" );
        //delay(18000);//laenge von audio        
        while (tmrpcm.isPlaying() == 1){
          speakingSmily();
        }
        lcd.clear();
        writeOnLCDCenter("Die erste Begegnung!", 2);   
        delay(1000);//Todo remove
        gpsstatus = 2;
      }
      if (gpsstatus == 2) {
        if (isAtLocationOne()) {
          lcd.clear();
          tmrpcm.play( "resolved.wav" ); 
          writeBigSmilyCenter();
          delay(60000);//laengeere pause (ca 1 minute) um zum stehen zu kommen mim auto
          digitalWrite(LED1, HIGH);                 
          delay(2000);
          digitalWrite(LED1, LOW);
          gpsstatus = 3;
        }
      }
      if (gpsstatus == 3) {
        tmrpcm.play( "ort2.wav" );
        lcd.clear();
        while (tmrpcm.isPlaying() == 1){
          speakingSmily();
        }
        //delay(14558);//laenge von audio
        lcd.clear();
        writeOnLCDCenter("Der erste Kuss!", 2);    
        delay(1000);//Todo remove            
        gpsstatus = 4;
      }
        if (gpsstatus == 4) {
          if (isAtLocationTwo()) {
            lcd.clear();
            writeBigSmilyCenter();
            tmrpcm.play( "resolved.wav" );
            delay(60000);//laengeere pause (ca 1 minute) um zum stehen zu kommen mim auto
            status = 4;
            digitalWrite(LED1, HIGH);
            delay(2000);//laenge von audio
            digitalWrite(LED1, LOW);
          }
        }
    } else {
      Serial.println("GPS NO FIX");
      delay(2000);
      lcd.clear();
      writeOnLCDCenter("Kein Signal", 2);
    }
  }
    //IVR Test
    if (status == 4) {      
      digitalWrite(LED1, HIGH);
      tmrpcm.play( "nummer.wav" );
      lcd.clear();      
      while (tmrpcm.isPlaying() == 1){
            speakingSmily();
      }              
      digitalWrite(LED1, LOW);
      lcd.clear();
      writeOnLCDCenter("080033030247", 1);
      status = 5;
    }

    if (status == 5) {
      digitalWrite(LED1, HIGH);
      if (!passOK) {
        char key = keypad.getKey();
        if (key != NO_KEY) {
          delay(20);
          writePINInputOnLCD();
          passOK = processNumberKey(key);
        }
      } else {
        tmrpcm.play( "unlocked.wav" );
        writeBigSmilyCenter();
        delay(14796);//laenge von audio
        status = 6;
      }
    }

    if (status == 6) {
      digitalWrite(LED1, LOW);
      writeUnlockTwo();
      servoblau.attach(5);
      servoblau.write(120);
      delay(1000);
      servoblau.detach();
      status = 7;
    }

    if (status == 7) {
      digitalWrite(LED1, HIGH);
      delay(50);
      digitalWrite(LED1, LOW);
      writeUnlockTwo();
    }
  }


  void initLCD() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    lcd.begin(20, 4);
    lcd.backlight();
    lcd.setCursor(0, 0);
    initLCDSmily();
  }
  void initSdAndSpeaker() {
    tmrpcm.speakerPin = SPEAKER_PIN;
    tmrpcm.setVolume(5);    
    //pinMode(53, OUTPUT);
  }

  void initIR() {
    pinMode(trigger, OUTPUT);
    pinMode(echo, INPUT);
  }


  //GPS STUFF
  void initGPS() {
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
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);
  }

  // Interrupt is called once a millisecond, looks for any new GPS data, and stores it
  SIGNAL(TIMER0_COMPA_vect) {
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) UDR0 = c;
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
  //GPS STUFF ENDS HERE


  //IRLogik
  boolean checkEntfernung() {
    long dauer = 0;
    long entfernung = 0;
    digitalWrite(trigger, LOW);
    delay(5);
    digitalWrite(trigger, HIGH);
    delay(10);
    digitalWrite(trigger, LOW);
    dauer = pulseIn(echo, HIGH);
    entfernung = (dauer / 2) * 0.03432;
    if (entfernung >= 500 || entfernung <= 0)
    {
      Serial.println("Kein Messwert");
      return false;
    }
    else
    {
      Serial.print(entfernung);
      Serial.println(" cm");
    }
    if (entfernung <= 5)
    {
      Serial.print("true");
      return true;
    }
    else
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

  boolean checkRestartCode() {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      Serial.print("key pressed: " + key);
      if (key == '4') {

        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }


  boolean checkPassword() {
    boolean passOk = false;
    if (password.evaluate()) {
      lcd.clear();
      //digitalWrite(LED, HIGH); // Schalte die LED an.
      passOk = true;
    } else {
      //      digitalWrite(LED, LOW); // Schalte die LED aus.
      lcd.clear();
      writeOnLCDCenter("Leider falsch", 1);
      writeOnLCDCenter("Nicht aufgeben!", 2);
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

  void writeUnlock() {
    int i = 0;
    uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
    lcd.createChar(0, heart);
    lcd.home();
    while (i < 80) {
      lcd.write((byte)0);
      i++;
    }
    delay(400);
    lcd.noDisplay();
    delay(400);
    // Turn on the display:
    lcd.display();
  }



  void writeOnLCDCenter(String lcdString, int row) {
    lcd.setCursor(getCenterCursorPosition(lcdString), row);
    lcd.print(lcdString);
  }

  void writePINInputOnLCD() {
    lcd.setCursor(10 - (maxPasswordLength - currentPasswordLength) + 2, 2);
    lcd.print("*");
  }

  void writeOnLCDCenter(String lcdString) {
    lcd.setCursor(getCenterCursorPosition(lcdString), 1);
    lcd.print(lcdString);
  }

  int getCenterCursorPosition(String lcdString) {
    int centerPosition = 10 - (lcdString.length() / 2);
    return centerPosition;
  }


  boolean isAtLocationOne() {
    if (millis() - timer > 2000) {
      timer = millis(); // reset the timer
      //float targetLatitudeDegrees = 49.34939956;
      //float targetLongitudeDegrees = 8.16590690;
      float targetLatitudeDegrees = 49.34515762;
      float targetLongitudeDegrees = 8.15875339;

      float diffLatitude = targetLatitudeDegrees - GPS.latitudeDegrees;
      float diffLongitude = targetLongitudeDegrees - GPS.longitudeDegrees;

      Serial.println("");
      Serial.print("diffLatitude = ");
      Serial.print(diffLatitude, 8);
      Serial.println("");
      Serial.print("diffLongitude = ");
      Serial.print(diffLongitude, 8);
      if (diffLatitude < 0.0004 && diffLatitude > -0.0004 &&
          diffLongitude < 0.0004 && diffLongitude > -0.0004) {
        Serial.println("");
        Serial.println("On Target");
        return true;
      } else {
        return false;
        //return true; //Debug
      }
    }
  }

  boolean isAtLocationTwo() {
    if (millis() - timer > 2000) {
      timer = millis(); // reset the timer

      //float targetLatitudeDegrees = 49.34939956;
      //float targetLongitudeDegrees = 8.16590690;
      float targetLatitudeDegrees = 49.36341094;
      //float targetLatitudeDegrees = 49.36360000;
      float targetLongitudeDegrees =  8.25144386;
      //float targetLongitudeDegrees = 8.25105600;

      float diffLatitude = targetLatitudeDegrees - GPS.latitudeDegrees;
      float diffLongitude = targetLongitudeDegrees - GPS.longitudeDegrees;

      Serial.println("");
      Serial.print("Latitude = ");Serial.print(GPS.latitudeDegrees, 8);
      Serial.println("");
      Serial.print("Longitude = ");Serial.print(GPS.longitudeDegrees,8);
      Serial.println("");
      Serial.print("diffLatitude = ");
      Serial.print(diffLatitude, 8);
      Serial.println("");
      Serial.print("diffLongitude = ");
      Serial.print(diffLongitude, 8);
      if (diffLatitude < 0.0004 && diffLatitude > -0.0004 &&
          diffLongitude < 0.0004 && diffLongitude > -0.0004) {
        Serial.println("");
        Serial.println("On Target");
        return true;
      } else {
        return false;
      }
    }
  }

  void writeUnlockTwo() {
    int i = 0;
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);

    while (i < 80) {
      lcd.print(char(1));
      i++;
    }
    delay(500);
    while (i > 0) {
      lcd.print(char(2));
      i--;
    }
    delay(500);
  }

/** SMILY ACTION **/
void initLCDSmily(){
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
  lcd.createChar (6, complete);
  lcd.createChar (7, half_o);
  lcd.createChar (8, half_u);
  lcd.createChar (9, empty);
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
//  lcd.clear();
  writeBigSmilySpeakingTwoCenter();    
  delay(700);
  //lcd.clear();
  writeBigSmilySpeakingOneCenter();
  delay(700); 
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
  //clear
  lcd.setCursor(9, 3);
  lcd.write(byte(9));  
}

void writeBigSmilySpeakingTwoCenter() {
  lcd.setCursor(8, 0);
  lcd.write(byte(6));
  lcd.setCursor(10, 0);
  lcd.write(byte(6));
  //clear
  lcd.setCursor(9, 2); 
  lcd.write(byte(9));  
  //clear ende
  lcd.setCursor(9, 3);
  lcd.write(byte(7));
  lcd.setCursor(9, 2);
  lcd.write(byte(7));
  lcd.setCursor(8, 2);
  lcd.write(byte(8));
  lcd.setCursor(10, 2);
  lcd.write(byte(8));
}



