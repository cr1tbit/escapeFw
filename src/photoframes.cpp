#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

#include "alfalog.h"

WiFiMulti wifiMulti;

// std::vector<int> inputs = {
// 	PIN_INPUT_0, PIN_INPUT_1, PIN_INPUT_2
// };

CardReaderPins_t pins = {
	.ss   = PIN_CRDR_SS,
	.sclk = PIN_CRDR_SCLK,
	.mosi = PIN_CRDR_MOSI,
	.miso = PIN_CRDR_MISO,
	.rst  = PIN_CRDR_RST,
};

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