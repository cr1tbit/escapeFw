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
// 2x HAL sensor
const std::vector<int> inputLight = {
	1
};

const std::vector<int> inputUV = {
	2
};

InputTrigger lightTrigger = InputTrigger(inputLight);
InputTrigger UVTrigger = InputTrigger(inputUV);

// 2x light GPIO
const int pinLight = 10;
const int pinUV = 9;

const int pinOnboardLed = 8;

/* * * * * * * * Pin definitions * * * * * * * * 
 *                ___[USB-C]___    
 *            5 )|             |( 5V
 *            6 )|  ESP32-C3   |( GND                
 *            7 )|  Supermini  |( 3V3  LED/HAL pwr
 *            8 )| <-led       |( 4    (legacy) pinUV
 *  pinUv     9 )|             |( 3    (legacy) pinLight
 *  pinLight 10 )|             |( 2    inputUV
 *           20 )|             |( 1    inputLight
 *           21 )| ___________ |( 0    
 */ 

void uartPrintAlogHandle(const char* str){
    Serial.println(str);
}

SerialLogger serialLogger = SerialLogger(
    uartPrintAlogHandle, LOG_DEBUG, ALOG_FANCY);

void setup() {
	// Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected
	Serial.begin(115200);

	AlfaLogger.addBackend(&serialLogger);

	pinMode(pinOnboardLed, OUTPUT);
	digitalWrite(pinOnboardLed, HIGH);

	pinMode(pinLight, OUTPUT);
	digitalWrite(pinLight, HIGH);

	ALOGI("Starting setup");
	// connectWifi();
	lightTrigger.init();	
	UVTrigger.init();

	pinMode(pinLight, OUTPUT);
	pinMode(pinUV, OUTPUT);
}

int loopCounter = 0;
bool completed = false;
void loop() {
	
	if (lightTrigger.check() && !completed) {
		ALOGI("Light quest completed");
		completed = true;
		// ledStrip.enabled = true;
		digitalWrite(pinLight, LOW);
	}
	
	
	//toggle LED every 1sec
	if (loopCounter % 10 == 0) {
		digitalWrite(pinOnboardLed, !digitalRead(pinOnboardLed));
	}
	loopCounter++;

	delay(100);
}