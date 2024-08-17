#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

#include "alfalog.h"

#include "threadedWS.h"


/* * * Peripherals: * * */

// Contractron pin
// const int CONT_PIN = 5;
const int TURN_ON_DELAY = 1000;

// 6x HAL sensors
const std::vector<int> inputs = {
	5,6,7,8
};

const int PHOTOFRAMES_CPLT_PIN = 2;

// 1x WS2812 LED chain
const int WS2812_PIN = 10;
const int WS2812_LED_COUNT = 6;

// 1x WS2812 LED chain
ThreadedWS ledStrip = ThreadedWS(WS2812_PIN, 8);

// 1x I2S sound card
// const int PIN_CRDR_SS  = 3;
// const int PIN_CRDR_SCLK = 2;
// const int PIN_CRDR_MOSI = 1;
// const int PIN_CRDR_MISO = 0;
// const int PIN_CRDR_RST = 10;

 
/* * * * * * * * Pin definitions * * * * * * * * 
 *                    ___[USB-C]___    
 *  PIN_INPUT_0   5 )|             |( 5V
 *  PIN_INPUT_1   6 )|  ESP32-C3   |( GND                
 *  PIN_INPUT_2   7 )|  Supermini  |( 3V3
 *  PIN_INPUT_3   8 )|             |( 4     
 *                9 )|             |( 3     CLPT_ALT
 *  WS2812_PIN   10 )|             |( 2     CPLT
 *               20 )|             |( 1     
 *               21 )| ___________ |( 0
 */ 

// WiFiMulti wifiMulti;


void uartPrintAlogHandle(const char* str){
    Serial.println(str);
}

SerialLogger serialLogger = SerialLogger(
    uartPrintAlogHandle, LOG_DEBUG, ALOG_FANCY);

InputTrigger inputTrigger = InputTrigger(inputs);

void setup() {
	// Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected
	Serial.begin(115200);

	AlfaLogger.addBackend(&serialLogger);

	ALOGI("Starting setup");
	inputTrigger.init();
	ledStrip.maxBrightness = 0;

	pinMode(PHOTOFRAMES_CPLT_PIN, OUTPUT);
	digitalWrite(PHOTOFRAMES_CPLT_PIN, LOW);

	ledStrip.init([](Adafruit_NeoPixel& strip) {
		for (int i = 0; i<strip.numPixels(); i++){
			strip.setPixelColor(i, 0, 0, random(30,60));
			// strip.setPixelColor(i, 
			// 	strip.ColorHSV(32768, 255, random(120)+135)
			// );
		}
		return random(100);
	});

	delay(1000);
}

bool framesSolved = false;

// int lastTickMs = 0;
// int blinkSpeed = 600;

int counter = 0;

void loop() {
	
	if (inputTrigger.check()) {
		if (framesSolved == false ) {
			framesSolved = true;
			ALOGI("Light quest completed");
			ledStrip.maxBrightness = 255;
			ledStrip.turnOn(TURN_ON_DELAY);
			digitalWrite(PHOTOFRAMES_CPLT_PIN, HIGH);
		} 
	} else {
		printf("%4d check: | ", counter++);
		for (int i = 0; i < inputs.size(); i++) {
			printf("%d | ", digitalRead(inputs[i]));		
		}	
		printf("\n\r");
	}
	delay(300);
}