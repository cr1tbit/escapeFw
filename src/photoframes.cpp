#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

#include "alfalog.h"
#include <Adafruit_NeoPixel.h>


/* * * Peripherals: * * */
// 6x HAL sensors
const std::vector<int> inputs = {
	PIN_INPUT_0, PIN_INPUT_1, PIN_INPUT_2,
	PIN_INPUT_3, PIN_INPUT_4, PIN_INPUT_5,
};

// 1x WS2812 LED chain
const int WS2812_PIN = 6;
const int WS2812_LED_COUNT = 6;

// 1x I2S sound card
// const int PIN_CRDR_SS  = 3;
// const int PIN_CRDR_SCLK = 2;
// const int PIN_CRDR_MOSI = 1;
// const int PIN_CRDR_MISO = 0;
// const int PIN_CRDR_RST = 10;

 
/* * * * * * * * Pin definitions * * * * * * * * 
 *                ___[USB-C]___    
 *            5 )|             |( 5V
 *            6 )|  ESP32-C3   |( GND                
 *            7 )|  Supermini  |( 3V3
 *            8 )|             |( 4
 *            9 )|             |( 3
 *           10 )|             |( 2
 *           20 )|             |( 1
 *           21 )| ___________ |( 0
 */ 

// WiFiMulti wifiMulti;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);


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
}

bool framesSolved = false;

void loop() {
	if (framesSolved)
	{
		for (int i = 0; i < WS2812_LED_COUNT; i++)
		{
			int flicker = random(0, 95);
			int r = random(200, 255 - flicker);
			int g = random(100, 150 - flicker);
			int b = 0;
			strip.setPixelColor(i, strip.Color(r, g, b));
		}
		strip.show();
		delay(random(100, 150));
	}
	else
	{
		if (inputTrigger.check())
		{
			Serial.println("Frames put in correct place!");
			framesSolved = true;
		}
		delay(100);
	}
}