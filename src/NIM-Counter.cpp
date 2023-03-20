/* Copyright (C) 2022 Leonardo Lisa, Alluringbits */
#include "NIM-Counter.h"

void setup() {
	pinMode(SELF_TEST_PIN, OUTPUT);
	digitalWrite(SELF_TEST_PIN, LOW);

	beginCounter();
	Serial.begin(9600);

#ifdef COUNTER_DISPLAY
	u8g2.begin();
	u8g2.enableUTF8Print();
#endif

    // randomSeed(analogRead(0));  
}

void loop() {
  /* uint8_t cmd{getCommand()}; */
  
	//TO DO: change protocol so that it sends back the received command and then sends th value, and update in accordance libNIM
	switch(getCommand()) {
		// Reset all
		case RSTA:
			// Serial.println("RSTA");
			resetCounter1();
			resetCounter2();
			Serial.write(1);
			Serial.write('\n');
			break;

		// Reset counter 1
		case RST1:
			// Serial.println("RST1");
			resetCounter1();
			Serial.write(1);
			Serial.write('\n');
			break;

		// Reset counter 2
		case RST2:
			//Serial.println("RST2");
			resetCounter2();
			Serial.write(1);
			Serial.write('\n');
			break;

		// Read counter 1
		case XET1:
#ifndef COUNTER_DEBUG_COMM
			readCounter1();
#else
			counter1 = 12345;
#endif
			/* for(uint8_t i{3}; i>=0; --i) Serial.write(static_cast<uint8_t>((counter1 >> (i*8)) & 255)); */ //Somehow doesn't work?
			Serial.write(counter1 >> 24);
			Serial.write((counter1 >> 16 ) & 255);
			Serial.write((counter1 >> 8 ) & 255);
			Serial.write(counter1 & 255);
			Serial.write('\n');
			break;

		// Read counter 2
		case XET2:
#ifndef COUNTER_DEBUG_COMM
			readCounter2();
#else
			counter2 = 54321;
#endif
			/* for(uint8_t i{3}; i>=0; --i) Serial.write((counter2 >> (i*8)) & 255); */
			Serial.write(counter2 >> 24);
			Serial.write((counter2 >> 16 ) & 255);
			Serial.write((counter2 >> 8 ) & 255);
			Serial.write(counter2 & 255);
			Serial.write('\n');
			break;
		case GET1:
#ifndef COUNTER_DEBUG_COMM
			readCounter1();
#else
			counter1 = 12345;
#endif
			Serial.println(counter1);
			break;
		case GET2:
#ifndef COUNTER_DEBUG_COMM
			readCounter2();
#else
			counter2 = 56789;
#endif
			Serial.println(counter2);
			break;
		// Remote control counter 1
		case EN_1:
			Serial.write(1);
			Serial.write('\n');
			break;

		// Remote control counter 2
		case EN_2:
			Serial.write(1);
			Serial.write('\n');
			break;
		case SRLN:
			/* for(uint8_t i{7}; i >=0; --i) Serial.write(0); */			
			for(uint8_t i{}; i<7; ++i) Serial.write(0);
			Serial.write(12);
			Serial.write('\n');
			break;
		case RSRT:
			break;
		case COMMAND_ERR:
			USART_Hard_Flush();
			Serial.println("ERR!");
			break;

		case NO_COMMAND:
		default:
			break;
	}
  
  //USART_Hard_Flush();

#ifdef COUNTER_DISPLAY
	readCounter();
	u8g2.firstPage();
	do {
		/* all graphics commands have to appear within the loop body. */
		u8g2.setFont(u8g2_font_crox2hb_tr);
		u8g2.setCursor(0, 15);
		u8g2.print(F("Counter 1"));
		u8g2.setCursor(0, 30);
		u8g2.print(counter1);
		u8g2.setCursor(0, 45);
		u8g2.print(F("Counter 2"));
		u8g2.setCursor(0, 60);
		u8g2.print(counter2);
		u8g2.drawFrame(0, 0, 128, 64);
	} while ( u8g2.nextPage() );
#endif

  // Debug
  // counter1 += random(10);
  // counter2 += random(50);
  // readCounter();
  // Serial.println(counter1);
  // Serial.println(counter2);
  // delay(1000);
  
  /*
  for(int i = 0; i < 10; i++) {
    digitalWrite(SELF_TEST_PIN, HIGH);
    digitalWrite(SELF_TEST_PIN, LOW);
  }
  */
}

void beginCounter() {
	DDRK = 0x00;  // Set port K direction as input.
	DDRA = 0xff;  // Set port A direction as output.
	DDRC |= 0x0f; // Set port C direction as output.

	PORTK = 0x00; // Set port K with NO_PULL-UP resistor.
	PORTA = 0xff; // Set all port A pins to 5V.
	digitalWrite(_CLR1, HIGH); // Active low.
	digitalWrite(_CLR2, HIGH); // Active low.
	digitalWrite(_LATCH1, LOW);
	digitalWrite(_LATCH2, LOW);

	resetCounter1();
	resetCounter2();
}

void resetCounter1() {
	digitalWrite(_CLR1, LOW);
	delayMicroseconds(10);
	digitalWrite(_CLR1, HIGH);
}

void resetCounter2() {
	digitalWrite(_CLR2, LOW);
	delayMicroseconds(10);
	digitalWrite(_CLR2, HIGH);
}

void readCounter(){
	/* Latch counter1 data in internal register */
	digitalWrite(_LATCH1, HIGH);
	//delayMicroseconds(10);
	digitalWrite(_LATCH1, LOW);

	/* Latch counter2 data in internal register */
	digitalWrite(_LATCH2, HIGH);
	//delayMicroseconds(10);
	digitalWrite(_LATCH2, LOW);


	/* Read counter1*/
	PORTA = ~(1 << (_GAL_1));
	counter1 = static_cast<uint32_t>(PINK);

	PORTA = ~(1 << (_GAU_1));
	counter1 += (static_cast<uint32_t>(PINK) << 8);

	PORTA = ~(1 << (_GBL_1));
	counter1 += (static_cast<uint32_t>(PINK) << 16);

	PORTA = ~(1 << (_GBU_1));
	counter1 += (static_cast<uint32_t>(PINK) << 24);



	/* Read counter2*/
	PORTA = ~(1 << (_GAL_2));
	counter2 = static_cast<uint32_t>(PINK);

	PORTA = ~(1 << (_GAU_2));
	counter2 += (static_cast<uint32_t>(PINK) << 8);

	PORTA = ~(1 << (_GBL_2));
	counter2 += (static_cast<uint32_t>(PINK) << 16);

	PORTA = ~(1 << (_GBU_2));
	counter2 += (static_cast<uint32_t>(PINK) << 24);


}

void readCounter1(){
	/* Latch counter1 data in internal register */
	digitalWrite(_LATCH1, HIGH);
	//delayMicroseconds(10);
	digitalWrite(_LATCH1, LOW);

	/* Read counter1*/
	PORTA = ~(1 << (_GAL_1));
	counter1 = static_cast<uint32_t>(PINK);

	PORTA = ~(1 << (_GAU_1));
	counter1 += (static_cast<uint32_t>(PINK) << 8);

	PORTA = ~(1 << (_GBL_1));
	counter1 += (static_cast<uint32_t>(PINK) << 16);

	PORTA = ~(1 << (_GBU_1));
	counter1 += (static_cast<uint32_t>(PINK) << 24);
}

void readCounter2() {

	/* Latch counter2 data in internal register */
	digitalWrite(_LATCH2, HIGH);
	//delayMicroseconds(10);
	digitalWrite(_LATCH2, LOW);


	/* Read counter2*/
	PORTA = ~(1 << (_GAL_2));
	counter2 = static_cast<uint32_t>(PINK);

	PORTA = ~(1 << (_GAU_2));
	counter2 += (static_cast<uint32_t>(PINK) << 8);

	PORTA = ~(1 << (_GBL_2));
	counter2 += (static_cast<uint32_t>(PINK) << 16);

	PORTA = ~(1 << (_GBU_2));
	counter2 += (static_cast<uint32_t>(PINK) << 24);
}

command getCommand() {
	char _buffer[COMMAND_LENGTH]{};

	if(Serial.available() >= COMMAND_LENGTH) {
		for(uint8_t i{}; i < COMMAND_LENGTH; ++i) {
			_buffer[i] = Serial.read();
		}
		USART_Hard_Flush();
	}
	else{
		return NO_COMMAND;
	}

	for(uint8_t j{}; j < COMMANDS_COUNT; ++j) {
		// Is a valid command.
		if(!(strncmp(_buffer, commands[j], COMMAND_LENGTH))) {
			return j;
		}
	}

	// Is an invalid command
	return COMMAND_ERR;
}

void USART_Flush() {
	volatile unsigned char dummy{};
	long lastMillis{millis()};

	while(UCSR0A & (1<<RXC0)) {
		dummy = UDR0;
		if((lastMillis - millis()) > USART_TIMEOUT) {
			return;  
		}
	}
}

void USART_Hard_Flush() {
	long lastMillis{};

	Serial.flush(); // Flush serial output buffer.
	USART_Flush();  // Flush serial input buffer register.

	// Flush serial input buffer.
	lastMillis = millis();
	while(Serial.available() != 0) {
		Serial.read();
		if((lastMillis - millis()) > USART_TIMEOUT) {
			return;  
		}
	}
}


