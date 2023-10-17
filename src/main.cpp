#include <SPI.h>
#include <MFRC522.h>
#include <vector>

#include "pinDefs.h"

MFRC522 mfrc522(PIN_CRDR_SS, PIN_CRDR_RST);  // Create MFRC522 instance

std::vector<int> inputs = {
	PIN_INPUT_0, PIN_INPUT_1, PIN_INPUT_2, 
	PIN_INPUT_3, PIN_INPUT_4, PIN_INPUT_5
};

void initCardReader(){
	SPI.begin(
		PIN_CRDR_SCLK,
		PIN_CRDR_MISO,
		PIN_CRDR_MOSI,
		-1
	);
	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();	
}

void setup() {
	Serial.setTxTimeoutMs(0); // prevent logger slowdown when no usb connected
	Serial.begin(115200);

	initCardReader();

	for (auto i: inputs) {
		pinMode(i, INPUT_PULLUP);		
	}
}

bool areAllInputsHigh(std::vector<int> pins) {
  	for (int i = 0; i < pins.size(); i++) {
		if (digitalRead(pins[i]) == LOW) {
		  return false;
		}
	}
  return true;
}

bool isCardDetected(){
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	// if ( ! mfrc522.PICC_IsNewCardPresent()) {
	// 	return;
	// }

	// // Select one of the cards
	// if ( ! mfrc522.PICC_ReadCardSerial()) {
	// 	return;
	// }

	// // Dump debug info about the card; PICC_HaltA() is automatically called
	// // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  // //print the UID
  // Serial.print("UID tag :");
  // Serial.println(mfrc522.uid.uidByte[0]);
}


void loop() {
	if (areAllInputsHigh(inputs)) {
		Serial.println("HAL quest completed");
		digitalWrite(PIN_LED_OUTPUT, HIGH);
	}

	if (isCardDetected()) {
		Serial.println("NFC quest completed");
		digitalWrite(PIN_LED_OUTPUT, HIGH);
	}

	delay(100);
}