
#include <Arduino.h>


#include <WiFi.h>
#include "SPIFFS.h"

#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define I2S_SPEAKER_SERIAL_CLOCK 2 // BCLK
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 3 // WSEL,LRC 
#define I2S_SPEAKER_SERIAL_DATA 1 // DIN


AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

/*
void spiffsls() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();
 
  while(file){
 
    Serial.print("FILE: ");
    Serial.println(file.name());

    file = root.openNextFile();
  }
}*/

void setup()
{
  Serial.setTxTimeoutMs(0);
  Serial.begin(115200);
  delay(1000);

  SPIFFS.begin();

  Serial.printf("Sample MP3 playback begins...\n");

  audioLogger = &Serial;

  file = new AudioFileSourceSPIFFS("/glonky.mp3");

  out = new AudioOutputI2S();
  out -> SetPinout(
    I2S_SPEAKER_SERIAL_CLOCK,
    I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    I2S_SPEAKER_SERIAL_DATA
  );
  out -> SetGain(0.2);
  // out -> SetOutputModeMono(true);
  // out -> SetBitsPerSample(16);

  mp3 = new AudioGeneratorMP3();
}

void loop()
{
  if (!(mp3->begin(file, out))) {
    delay(2000);
  }

  while (mp3->isRunning()) {
    if (!mp3->loop()) {
      delay(1000);
      break;
    }
  }
  
}