#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

#include "alfalog.h"

WiFiMulti wifiMulti;

/* * * Peripherals: * * */
// 1x HAL sensor, powered from GPIO

const int HAL_PIN = 0;
const int HAL_GND = 1;
const int HAL_VDD = 2;

const std::vector<int> inputs = {
	HAL_PIN
};

// 1x relay gpio switch
const int PIN_RELAY = 10;
 
/* * * * * * * * Pin definitions * * * * * * * * 
 *                ___[USB-C]___    
 *            5 )|             |( 5V
 *            6 )|  ESP32-C3   |( GND                
 *            7 )|  Supermini  |( 3V3
 *            8 )|             |( 4
 *            9 )|             |( 3
 *    RELAY  10 )|             |( 2   HAL_PIN
 *           20 )|             |( 1   HAL_GND
 *           21 )| ___________ |( 0   HAL_VDD
 */ 


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

	pinMode(PIN_RELAY, OUTPUT);
	digitalWrite(PIN_RELAY, LOW);

	// HAL is powered from GPIO
	pinMode(HAL_GND, OUTPUT);
	digitalWrite(HAL_GND, LOW);
	pinMode(HAL_VDD, OUTPUT);
	digitalWrite(HAL_VDD, HIGH);

	inputTrigger.init();
}

void loop() {	
	if (inputTrigger.check()) {
		digitalWrite(PIN_RELAY, HIGH);
	}
	delay(100);
}