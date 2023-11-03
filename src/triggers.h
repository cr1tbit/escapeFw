
#include <MFRC522.h>
#include <Arduino.h>
#include <map>
#include <vector>

class Trigger {
	public:
	Trigger(){};

	virtual bool check() = 0;
};

typedef struct{
	int8_t ss;
	int8_t sclk;
	int8_t mosi;
	int8_t miso;
	int8_t rst;
} CardReaderPins_t;

class CardReaderTrigger : public Trigger {
	public:
	MFRC522 reader;

	CardReaderTrigger(CardReaderPins_t& pins) : reader(pins.ss, pins.rst) {
		_pins = pins;
	}
	
	void init() {
		SPI.begin(
			_pins.sclk,
			_pins.miso,
			_pins.mosi,
			-1
		);
		reader.PCD_Init();
		delay(4);
		// reader.PCD_DumpVersionToSerial();
	}

	bool check() override {
		if ( ! reader.PICC_IsNewCardPresent()) {
			return false;
		}

		// Select one of the cards
		if ( ! reader.PICC_ReadCardSerial()) {
			return false;
		}

		// reader.PICC_DumpToSerial(&(reader.uid));
		//print the UID
		printUid(reader.uid);
		if (cards.find(reader.uid) == cards.end()) {
			Serial.println("Card not found");
			// cards.insert(std::pair<MFRC522::Uid, bool>(reader.uid, true));
		} else {
			cards[reader.uid]();
		}
		return true;
	}

	void addCard(std::vector<uint8_t> uid, std::function<void(void)> callback) {
		cards.insert(std::pair<MFRC522::Uid, std::function<void(void)>>(uidFromBytes(uid), callback));
	}

	struct UidComparator {
		bool operator()(const MFRC522::Uid& uid1, const MFRC522::Uid& uid2) const {
			if (uid1.size != uid2.size) {
				return uid1.size < uid2.size;
			}
			for (int i = 0; i < uid1.size; i++) {
				if (uid1.uidByte[i] != uid2.uidByte[i]) {
					return uid1.uidByte[i] < uid2.uidByte[i];
				}
			}
			return false;
		}
	};

	void printUid(MFRC522::Uid& uid) {
		String out = "UID tag[" + String(uid.size) + "]: {";
		for (int i = 0; i < uid.size; i++) {
			out += uid.uidByte[i];
			if (i < uid.size - 1){
				out += ", ";
			}
		}
		out += "}";
		Serial.println(out);
	}

	MFRC522::Uid uidFromBytes(std::vector<uint8_t> bytes) {
		MFRC522::Uid uid;
		uid.size = bytes.size();

		for (int i = 0; i < bytes.size(); i++) {
			uid.uidByte[i] = bytes[i];
		}
		printUid(uid);
		return uid;
	}

	std::map<MFRC522::Uid, std::function<void(void)>, UidComparator> cards;
	CardReaderPins_t _pins;
};


class InputTrigger : public Trigger {
	public:
	int pin;
	InputTrigger(std::vector<int> pins){
		pins = pins;
	}

	void init() {
		for (auto i: pins) {
			pinMode(i, INPUT);		
		}
	}

	bool check() override {
		for (int i = 0; i < pins.size(); i++) {
			if (digitalRead(pins[i]) == LOW) {
			  return false;
			}
		}
		return true;
	}

	std::vector<int> pins;
};