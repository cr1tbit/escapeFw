#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

#include "alfalog.h"

/* * * Peripherals: * * */
// 2x HAL sensor
const std::vector<int> inputLight = {
	PIN_INPUT_0
};

const std::vector<int> inputUV = {
	PIN_INPUT_1
};

// 2x light GPIO
const int pinLight = 4;
const int pinUV = 5;
 
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

void uartPrintAlogHandle(const char* str){
    Serial.println(str);
}

SerialLogger serialLogger = SerialLogger(
    uartPrintAlogHandle, LOG_DEBUG, ALOG_FANCY);

CardReaderTrigger cardReaderTrigger = CardReaderTrigger(pins);
// InputTrigger inputTrigger = InputTrigger(inputs);

void connectWifi(){
	WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
    wifiMulti.run();
    if(WiFi.status() != WL_CONNECTED){
        WiFi.disconnect(true);
        wifiMulti.run();
    }
	Serial.println("Wifi connect");
}

void setup() {
	// Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected
	Serial.begin(115200);

	AlfaLogger.addBackend(&serialLogger);

	ALOGI("Starting setup");
	// connectWifi();
	
	cardReaderTrigger.init();
	cardReaderTrigger.addCard(
		{83, 91, 65, 221},
		[](){
			ALOGI("Card 0 detected");
		}
	);
	cardReaderTrigger.addCard(
		{4, 251, 3, 186, 184, 92, 132},
		[](){
			ALOGI("Card 0 detected");
		}
	);
}

void loop() {
	cardReaderTrigger.check();

	// if (areAllInputsHigh(inputs)) {
	// 	Serial.println("HAL quest completed");
	// 	digitalWrite(PIN_LED_OUTPUT, HIGH);
	// }

	// if (isCardDetected()) {
	// 	Serial.println("NFC quest completed");
	// 	digitalWrite(PIN_LED_OUTPUT, HIGH);
	// }

	delay(100);
}