#include <SPI.h>
#include <vector>

#include <Arduino.h>
#include "WiFiMulti.h"

#include "triggers.h"

#include "pinDefs.h"
#include "secrets.h"

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

CardReaderTrigger cardReaderTrigger = CardReaderTrigger(pins);
// InputTrigger inputTrigger = InputTrigger(inputs);

void setup() {
	Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected
	Serial.begin(115200);

	Serial.println("Starting setup");

	// WiFi.mode(WIFI_STA);
    // wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
    // wifiMulti.run();
    // if(WiFi.status() != WL_CONNECTED){
    //     WiFi.disconnect(true);
    //     wifiMulti.run();
    // }

	// Serial.println("Wifi connect");

	cardReaderTrigger.init();
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