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
const int pinLight = 4;
const int pinUV = 5;

/* * * * * * * * Pin definitions * * * * * * * * 
 *                ___[USB-C]___    
 *            5 )|             |( 5V
 *            6 )|  ESP32-C3   |( GND                
 *            7 )|  Supermini  |( 3V3
 *            8 )|             |( 4    pinUV
 *            9 )|             |( 3    pinLight
 *           10 )|             |( 2    inputUV
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

	ALOGI("Starting setup");
	// connectWifi();
	lightTrigger.init();	
	UVTrigger.init();

	pinMode(pinLight, OUTPUT);
	pinMode(pinUV, OUTPUT);
	
}

void loop() {
	
	if (lightTrigger.check()) {
		ALOGI("Light quest completed");
		ledStrip.enabled = true;
	}
	
	delay(100);
}