#include <Keypad.h>
#include <Password.h>

/**
 * 
 * Arduino Pin Belegung 
 *  GND am Keypad (links PIN 28)
 *  22, 24, 26, 28 (von rechts)
 *  LED 52 (100 Ohm an GND)
 */


//PWD SETUOP
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
byte colPins[COLS] = {22, 24, 26, 28}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//LED SETUP
int LED=52; 



void setup() {
 Serial.begin(9600);
 pinMode(LED, OUTPUT);
 digitalWrite(LED, LOW); // Schalte die LED aus.
}

void loop() {
   char key = keypad.getKey();
   if (key != NO_KEY){
      delay(60);       
        processNumberKey(key);                
      }
   }


void processNumberKey(char key) {
   Serial.print(key);
   currentPasswordLength++;
   password.append(key);
   if (currentPasswordLength == maxPasswordLength) {
      checkPassword();
   } 
}

void checkPassword() {
   if (password.evaluate()){
      Serial.println(" OK.");      
      digitalWrite(LED, HIGH); // Schalte die LED an.
   } else {
      digitalWrite(LED, LOW); // Schalte die LED aus.
      Serial.println(" Wrong passwowrd!");
   } 
   resetPassword();
}

void resetPassword() {
   password.reset(); 
   currentPasswordLength = 0; 
}


