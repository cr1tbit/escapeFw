
#include <Arduino.h>


#include <WiFi.h>
#include "SPIFFS.h"

#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceID3.h"


const int I2S_SPEAKER_SERIAL_CLOCK = 2; // BCLK
const int I2S_SPEAKER_LEFT_RIGHT_CLOCK = 1; // WSEL,LRC 
const int I2S_SPEAKER_SERIAL_DATA = 3; // DIN

const int PHOTOFRAMES_CPLT_PIN = 5; // or 8
const int DOOR_CLOSED_PIN = 6;


/* * * * * * * * Pin definitions * * * * * * * * 
 *                ___[USB-C]___                     MAX98357A
 *    PF-1    5 )|             |( 5V           -> 5V
 *    HAL     6 )|  ESP32-C3   |( GND          -> GND    
 *            7 )|  Supermini  |( 3V3          xx SD
 *    PF-2    8 )|             |( 4   MAX_GAIN -> GAIN
 *            9 )|             |( 3   MAX_DIN  -> DIN  
 *           10 )|             |( 2   MAX_SCK  -> SCL
 *           20 )|             |( 1   MAX_LRC  -> LRC
 *           21 )| ___________ |( 0   MAX_SD 
 *
 *
 *           PF-1 | PF-2 | 5V | GND
 *            ||    ||    ||   ||
 */ 

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void setup()
{
  Serial.begin(115200);
  Serial.setTxTimeoutMs(0);

  pinMode(PHOTOFRAMES_CPLT_PIN, INPUT_PULLDOWN);
  pinMode(DOOR_CLOSED_PIN, INPUT_PULLDOWN);

  SPIFFS.begin();

  Serial.printf("Sample MP3 playback begins...\n");
  audioLogger = &Serial;

  file = new AudioFileSourceSPIFFS("/spooky.mp3");

  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  out = new AudioOutputI2S();
  out -> SetPinout(
    I2S_SPEAKER_SERIAL_CLOCK,
    I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    I2S_SPEAKER_SERIAL_DATA
  );
  
  out->SetGain(0.5); //set volume

  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
}

bool photoFramesComplete = false;
// void loop()
// {
// //   if (digitalRead(PHOTOFRAMES_CPLT_PIN) == HIGH) {
// //   if (digitalRead(DOOR_CLOSED_PIN) == HIGH) {
// //     if (!photoFramesComplete) {
// //       photoFramesComplete = true;
// //     }
// //   }
  
//   if (mp3->isRunning()) {
//     if (!mp3->loop()) {
//       // delay(1000);
//       ESP.restart();
//     }
//   }
// }


void loop()
{
//   if (digitalRead(PHOTOFRAMES_CPLT_PIN) == HIGH) {
  if (digitalRead(DOOR_CLOSED_PIN) == LOW) {
    if (!photoFramesComplete) {
      photoFramesComplete = true;
    }
  }
  
  
  if (mp3->isRunning()) {
    if (digitalRead(DOOR_CLOSED_PIN) == LOW) {
        if (!mp3->loop()) {
            // delay(1000);
              mp3->begin(id3, out);

        }
    }
    
  }
}
