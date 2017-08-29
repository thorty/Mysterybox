#include <Wire.h> //Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h>//Vorher hinzugefügte LiquidCrystal_I2C //Bibliothek einbinden

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //Hier wird das Display benannt. In unserem //Fall „lcd“. Die I²C Adresse (Erläuterung und I²C Adressen Scanner in folgender Anleitung: Link zur Anleitung „2 I²C Displays gleichzeitig“) 0x27 wird auch angegeben.

uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
int i=0;
void setup()

{

lcd.begin(20,4); //Das Display wird gestartet und die Größe des Displays wird angegeben(Jeweils 20 Zeichen in 4 Zeilen)
lcd.backlight();//Beleuchtung des Displays einschalten

lcd.createChar(0, heart);
lcd.home();

lcd.setCursor(0,0); //Text soll beim ersten Zeichen in der ersten Reihe beginnen..

while(i<20){
  lcd.write((byte)0);
  i++;
}
i=0;
lcd.setCursor(0,1);
lcd.print("Bring mich zum Ort"); //In der ersten Zeile soll der Text „Test Zeile 1“ angezeigt werden
lcd.setCursor(0,2); //Genauso geht es bei den weiteren drei Zeilen weiter
lcd.print("der ersten Begegnung");
lcd.setCursor(0,3);

while(i<20){
  lcd.write((byte)0);
  i++;
}



}

void loop()

{


}


