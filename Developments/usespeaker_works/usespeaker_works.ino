#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>
#define CHIP_SELECT_PIN  53
#define SPEAKER_PIN      11
TMRpcm tmrpcm;
 
void setup() {

  tmrpcm.speakerPin = SPEAKER_PIN;
  Serial.begin( 9600 );
  tmrpcm.volume(5); 
  //pinMode(53, OUTPUT);
  while ( !Serial ) /*mt*/ ;
  if ( !SD.init( CHIP_SELECT_PIN ) )
    Serial.println( "SD fail" );
  else {
    Serial.println( "PLAY MUSIC" );
    tmrpcm.play( "KnowHow.wav" );
    delay( 5 );
  }
}
 
void loop( ) {
}
