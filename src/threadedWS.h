#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
// #include <FreeRTOS.h>
#include <functional>

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


class ThreadedWS {
  private:
    Adafruit_NeoPixel strip;
    std::function<int(Adafruit_NeoPixel&)> drawFunc;
    bool enabled = false;

    int turnCommandTimestamp = 0;
    int turnCommandTransientMs = 0;

    const int maxBrightness = 255;
  public:
    ThreadedWS(int pin, int ledCount) : strip(ledCount, pin, NEO_RGB + NEO_KHZ800) {}

    void init(std::function<int(Adafruit_NeoPixel&)> func) {
      strip.begin();
      strip.show(); // Initialize all pixels to 'off'
      drawFunc = func;

      // Create a new task that will execute the drawOnStrip() function
      xTaskCreatePinnedToCore(
        drawOnStrip,   /* Function to implement the task */
        "drawOnStrip", /* Name of the task */
        2500,         /* Stack size in words */
        this,          /* Task input parameter */
        31,            /* Priority of the task */
        NULL,          /* Task handle. */
        1);            /* Core where the task should run */
    }

    void turnOn(int transientMs) {
      if (enabled) {
        return;
      }
      turnCommandTransientMs = transientMs;
      turnCommandTimestamp = millis();
      enabled = true;
    }

    void turnOff(int transientMs) {
      if (!enabled) {
        return;
      }
      turnCommandTransientMs = transientMs;
      turnCommandTimestamp = millis();
      enabled = false;
    }

    static void drawOnStrip(void * parameter) {
      ThreadedWS* stripObj = static_cast<ThreadedWS*>(parameter);
      for(;;) { // Infinite loop
        int targetDelay = stripObj->drawFunc(stripObj->strip);

        //handle transient dimming
        float dimmRatio = 1.0f;
        if ((stripObj->turnCommandTransientMs + stripObj->turnCommandTimestamp) > millis()) {
          dimmRatio = (float)(millis() - stripObj->turnCommandTimestamp)
            / ((float)(stripObj->turnCommandTransientMs));
          ALOGI("penis {}/{} = {}",            
            millis() - stripObj->turnCommandTimestamp,
            stripObj->turnCommandTransientMs,
            dimmRatio
          );
          
        }

        uint8_t briGammaCorr;

        if (stripObj->enabled) {
          briGammaCorr = gamma8[(uint8_t)(stripObj->maxBrightness * dimmRatio)];
          // stripObj->strip.setBrightness(stripObj->maxBrightness * dimmRatio);
        } else {
          briGammaCorr = gamma8[(uint8_t)(stripObj->maxBrightness * (1.0f - dimmRatio))];
          // stripObj->strip.setBrightness(stripObj->maxBrightness * (1.0f - dimmRatio));
        }        
        stripObj->strip.setBrightness(briGammaCorr);
        ALOGI("dupa {} {}",dimmRatio, briGammaCorr);


		    stripObj->strip.show();
        if (targetDelay < 10 ){
          targetDelay = 10;
        }
        vTaskDelay(targetDelay);
      }      
    }

};
