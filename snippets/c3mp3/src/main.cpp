
#include <Arduino.h>


#include <WiFi.h>
#include "SPIFFS.h"

#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceID3.h"

#define I2S_SPEAKER_SERIAL_CLOCK 2 // BCLK
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK 3 // WSEL,LRC 
#define I2S_SPEAKER_SERIAL_DATA 1 // DIN


AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

// void spiffsls() {
//   if (!SPIFFS.begin(true)) {
//     Serial.println("An Error has occurred while mounting SPIFFS");
//     return;
//   }
 
//   File root = SPIFFS.open("/");
 
//   File file = root.openNextFile();
 
//   while(file){
 
//     Serial.print("FILE: ");
//     Serial.println(file.name());

//     file = root.openNextFile();
//   }
// }

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
  Serial.setTxTimeoutMs(0);
  Serial.begin(115200);

  SPIFFS.begin();

  Serial.printf("Sample MP3 playback begins...\n");

  audioLogger = &Serial;

  file = new AudioFileSourceSPIFFS("/glonky.mp3");

  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");

  out = new AudioOutputI2S();
  out -> SetPinout(
    I2S_SPEAKER_SERIAL_CLOCK,
    I2S_SPEAKER_LEFT_RIGHT_CLOCK,
    I2S_SPEAKER_SERIAL_DATA
  );

  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
}

void loop()
{
  if (mp3->isRunning()) {
    if (!mp3->loop()) ESP.restart();
  }
}