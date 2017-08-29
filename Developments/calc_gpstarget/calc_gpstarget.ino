

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("CalcTest fpor GPS!");
  isInScope();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void isInScope( ){
 float targetLatitudeDegrees = 22.34515762;
 float targetLongitudeDegrees =5.15875339;                              
 float testLatitudeDegrees = 22.3455000;
 float testLongitudeDegrees =5.1589533;

 float diffLatitude = targetLatitudeDegrees-testLatitudeDegrees;
 float diffLongitude = targetLongitudeDegrees - testLongitudeDegrees;

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
      }
 
}



