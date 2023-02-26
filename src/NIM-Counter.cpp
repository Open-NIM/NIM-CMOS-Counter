/*
 * NIM-Counter.ino
 * 
 * Version: 1.00
 * Date: 26/02/2023
 * Author : Leonardo Lisa
 * leonardo.lisa@studenti.unimi.it
 * 
 */ 

#define U8G2_WITHOUT_FONT_ROTATION

#include <avr/io.h>
#include <avr/interrupt.h>

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#include "pinMapping.h"


/* Parameters */
#define COMMAND_LENGTH 4
#define COMMANDS_COUNT 12
#define INVALID_COMMAND 12 // Must be = COMMANDS_COUNT
#define PASS COMMANDS_COUNT + 1
#define USART_TIMEOUT 100

#define DEBOUNCING_TIME 150

#define SELECT_TRUE   0
#define SELECT_LOCAL  2
#define SELECT_REMOTE 1

#define SERIAL_NUMBER F("0001")
#define FW_VERSION F("1.00")

/* Global variable */
#ifdef DISPLAY
  //clock=D0, data=D1, cs=CS, reset=RST
  U8G2_SSD1306_128X64_NONAME_F_3W_HW_SPI display1(U8G2_R0, CS1, RST1);
  U8G2_SSD1306_128X64_NONAME_F_3W_HW_SPI display2(U8G2_R0, CS2, RST2);
#endif

const char commands[COMMANDS_COUNT][COMMAND_LENGTH] = {
  "RSTA",
  "RST1",
  "RST2",
  "GET1",
  "GET2",
  "EN_1",
  "EN_2",
  "DE_1",
  "DE_2",
  "SRLN",
  "FW_v",
  "help"
  };

volatile uint32_t counter1 = 0; //uint64_t
volatile uint32_t counter2 = 0;

volatile uint8_t en_status_ch1 = 0;
volatile uint8_t en_status_ch2 = 0;

uint8_t remote_en_ch1 = 0;
uint8_t remote_en_ch2 = 0;

volatile long lastMillis1 = 0;
volatile long lastMillis2 = 0;

/* Global functions */
void beginCounter(void);
void resetCounter1(void);
void resetCounter2(void);
void readCounter(void);

uint8_t getCommand(void); 
bool compareArrays(char arr[], int command);
void USART_Flush(void);
void USART_Hard_Flush(void);

uint32_t readCounter(uint8_t n);


void setup() {
  beginCounter();
  Serial.begin(9600);

  display1.begin();
  display1.enableUTF8Print();

  display2.begin();
  display2.enableUTF8Print();
}

void loop() {
  /* Prevent millis() overflow */
  if(lastMillis1 > millis()) {
    lastMillis1 = 0;
    }
  if(lastMillis2 > millis()) {
    lastMillis2 = 0;
    }
  
  /* Read enable switch */
  en_status_ch1 = (PINF & (1 << _LOCAL1)) | (PINF & (1 << _REMOTE1));
  en_status_ch2 = ((PINF & (1 << _LOCAL2)) | (PINF & (1 << _REMOTE2))) >> 2;
  switch(en_status_ch1) {
    case SELECT_TRUE:
      digitalWrite(EN_CH1, HIGH);
    break;
    
    case SELECT_LOCAL:
      // if GATE1 is HIGH
      if(!!(PINE & (1 << PE4))) {
        digitalWrite(EN_CH1, HIGH);
        }else{
          digitalWrite(EN_CH1, LOW);
          }
    break;
    
    case SELECT_REMOTE:
      if(remote_en_ch1 == 1) {
        digitalWrite(EN_CH1, HIGH);
        }else{
          digitalWrite(EN_CH1, LOW);
          }
    break;
  }
  switch(en_status_ch2) {
    case SELECT_TRUE:
      digitalWrite(EN_CH2, HIGH);
    break;
    
    case SELECT_LOCAL:
      // if GATE2 is HIGH
      if(!!(PINE & (1 << PE5))) {
        digitalWrite(EN_CH2, HIGH);
        }else{
          digitalWrite(EN_CH2, LOW);
          }
    break;
    
    case SELECT_REMOTE:
      if(remote_en_ch2 == 1) {
        digitalWrite(EN_CH2, HIGH);
        }else{
          digitalWrite(EN_CH2, LOW);
          }
    break;
  }

  /*Serial comunication */
  uint8_t cmd = getCommand();

  switch(cmd) {
    // Reset all.
    case 0:
      resetCounter1();
      resetCounter2();
      Serial.println(F("-> Counter1 and Counter2 Reset"));
    break;
    
    // Reset counter 1.
    case 1:
      resetCounter1();
      Serial.println(F("-> Counter1 Reset"));
    break;

    // Reset counter 2.
    case 2:
      Serial.println(F("-> Counter2 Reset"));
      resetCounter2();
    break;
    
    // Read counter 1.
    case 3:
      Serial.print(F("> Counter1: "));
      readCounter();
      Serial.println(counter1);
    break;

    // Read counter 2.
    case 4:
      Serial.print(F("> Counter2: "));
      readCounter();
      Serial.println(counter2);
    break;
    
    // Remote enable counter 1.
    case 5:
      if(en_status_ch1 == SELECT_REMOTE) {
        digitalWrite(EN_CH1, HIGH);
        remote_en_ch1 = 1;
        Serial.println(F("-> Counter1 Enable"));
        }else{
          Serial.println(F("> Error(EN_1): plese select remote"));
          }
    break;

    // Remote enable counter 2.
    case 6:
      if(en_status_ch2 == SELECT_REMOTE) {
        digitalWrite(EN_CH2, HIGH);
        remote_en_ch2 = 1;
        Serial.println(F("-> Counter2 Enable"));
        }else{
          Serial.println(F("> Error(EN_2): plese select remote"));
          }
    break;

    // Remote disable counter 1.
    case 7:
      if(en_status_ch1 == SELECT_REMOTE) {
        digitalWrite(EN_CH1, LOW);
        remote_en_ch1 = 0;
        Serial.println(F("-> Counter1 Disable"));
        }else{
          Serial.println(F("> Error(DE_1): plese select remote"));
          }
    break;

    // Remote disable counter 2.
    case 8:
      if(en_status_ch2 == SELECT_REMOTE) {
        digitalWrite(EN_CH2, LOW);
        remote_en_ch2 = 0;
        Serial.println(F("-> Counter2 Disable"));
        }else{
          Serial.println(F("> Error(DE_2): plese select remote"));
          }
    break;

    // Get serial number.
    case 9:
      Serial.print(F("> Serial number: "));
      Serial.println(SERIAL_NUMBER);
    break;

    // Get firmware version.
    case 10:
      Serial.print(F("> Firmware version is: "));
      Serial.println(FW_VERSION);
    break;

    // Get help.
    case 11:
      Serial.println(F("> Help! Command list:"));
      Serial.println(F("    RSTA: reset all counters"));
      Serial.println(F("    RST1: reset counter1"));
      Serial.println(F("    RST2: reset counter2"));
      Serial.println(F("    GET1: get counter 1 counts"));
      Serial.println(F("    GET2: get counter 2 counts"));
      Serial.println(F("    EN_1: enable counting on counter1"));
      Serial.println(F("    EN_2: enable counting on counter2"));
      Serial.println(F("    DE_1: disable counting on counter1"));
      Serial.println(F("    DE_2: disable counting on counter2"));
      Serial.println(F("    SRLN: get serial number"));
      Serial.println(F("    FW_v: get firmware version"));
      Serial.println(F("    help: you are here ;)"));
    break;

    // Invalid command.
    case INVALID_COMMAND:
      USART_Hard_Flush();
      Serial.println("> Error(Invalid Command)");
    break;

    // No command sent.
    case PASS:
    break;

    default:
      USART_Hard_Flush();
    break;
  }
  
  /* Display */
  readCounter();
  display1.clearBuffer();
  display1.setFont(u8g2_font_crox2hb_tr);
  display1.setCursor(0, 15);
  display1.print(F("EN > "));
  switch(en_status_ch1) {
    case SELECT_TRUE:
      display1.print(F("TRUE"));
    break;
    
    case SELECT_LOCAL:
      display1.print(F("LOCAL"));
    break;
    
    case SELECT_REMOTE:
      display1.print(F("REMOTE"));
    break;
  }
  display1.setCursor(0, 45);
  display1.print(F("COUNTER 1"));
  display1.setCursor(0, 60);
  display1.print(counter1);
  display1.sendBuffer();

  display2.clearBuffer();
  display2.setFont(u8g2_font_crox2hb_tr);
  display2.setCursor(0, 15);
  display2.print(F("EN > "));
  switch(en_status_ch2) {
    case SELECT_TRUE:
      display2.print(F("TRUE"));
    break;
    
    case SELECT_LOCAL:
      display2.print(F("LOCAL"));
    break;
    
    case SELECT_REMOTE:
      display2.print(F("REMOTE"));
    break;
  }
  display2.setCursor(0, 45);
  display2.print(F("COUNTER 2"));
  display2.setCursor(0, 60);
  display2.print(counter2);
  display2.sendBuffer();
}

void beginCounter(void) {
  cli();
  DDRK = 0x00;  // Set port K direction as input.
  DDRA = 0xff;  // Set port A direction as output.
  DDRC |= 0x0f; // Set lower half of port C direction as output.
  DDRF &= 0xf0;  // Set lower half of port F direction as input.
  DDRD &= 0xf0;  // Set lower half of port D direction as input.
  pinMode(GATE1, INPUT);
  pinMode(GATE2, INPUT);
  pinMode(EN_CH1, OUTPUT);
  pinMode(EN_CH2, OUTPUT);

  PORTK = 0x00; // Set port K with NO_PULL-UP resistor.
  PORTA = 0xff; // Set all port A pins to 5V.
  PORTF &= 0xf0; // Set lower half of port F with NO_PULL-UP resistor.
  PORTD &= 0xf0; // Set lower half of port D with NO_PULL-UP resistor.
  digitalWrite(GATE1, LOW); // NO_PULL-UP resistor.
  digitalWrite(GATE2, LOW); // NO_PULL-UP resistor.
  digitalWrite(EN_CH1, LOW);
  digitalWrite(EN_CH2, LOW);
  
  digitalWrite(_CLR1, HIGH); // Active low.
  digitalWrite(_CLR2, HIGH); // Active low.
  digitalWrite(_LATCH1, LOW);
  digitalWrite(_LATCH2, LOW);

  EICRA = 0xff; // INT0, INT1, INT2, INT3 set on rising edge.
  EICRB = (1 << ISC50)|(1 << ISC40); // INT4, INT5 set on any logical chage.
  EIMSK |= (1 << INT5)|(1 << INT4)|(1 << INT3)|(1 << INT2)|(1 << INT1)|(1 << INT0); // enable interrupts.

  sei();
  
  resetCounter1();
  resetCounter2();
}

void resetCounter1(void) {
  digitalWrite(_CLR1, LOW);
  delayMicroseconds(10);
  digitalWrite(_CLR1, HIGH);
}

void resetCounter2(void) {
  digitalWrite(_CLR2, LOW);
  delayMicroseconds(10);
  digitalWrite(_CLR2, HIGH);
}

void readCounter(void) {
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
  counter1 = (uint32_t)(PINK);

  PORTA = ~(1 << (_GAU_1));
  counter1 += ((uint32_t)(PINK) << 8);

  PORTA = ~(1 << (_GBL_1));
  counter1 += ((uint32_t)(PINK) << 16);

  PORTA = ~(1 << (_GBU_1));
  counter1 += ((uint32_t)(PINK) << 24);

  /* Read counter2*/
  PORTA = ~(1 << (_GAL_2));
  counter2 = (uint32_t)(PINK);

  PORTA = ~(1 << (_GAU_2));
  counter2 += ((uint32_t)(PINK) << 8);

  PORTA = ~(1 << (_GBL_2));
  counter2 += ((uint32_t)(PINK) << 16);

  PORTA = ~(1 << (_GBU_2));
  counter2 += ((uint32_t)(PINK) << 24);
}

uint8_t getCommand(void) {
  char _buffer[COMMAND_LENGTH];

  if(Serial.available() == 0) {
    return PASS;
    }
  
  if(Serial.available() >= COMMAND_LENGTH) {
    for(uint8_t i = 0; i < COMMAND_LENGTH; i++) {
      _buffer[i] = Serial.read();
    }
    USART_Hard_Flush();
  }else{
    return INVALID_COMMAND;
    }
  
  for(uint8_t j = 0; j < COMMANDS_COUNT; j++) {
    // Is a valid command.
    if(compareArrays(_buffer, j)) {
      return j;
    }
  }

  // Is an invalid command
  return INVALID_COMMAND;
}

bool compareArrays(char arr[], int command) {
  for(uint8_t i = 0; i < COMMAND_LENGTH; i++) {
    if (commands[command][i] != arr[i]) return false;
  }
  return true;
}

void USART_Flush(void) {
  volatile unsigned char dummy;
  long lastMillis = millis();
  
  while(UCSR0A & (1<<RXC0)) {
    dummy = UDR0;
    if((lastMillis - millis()) > USART_TIMEOUT) {
      return;  
    }
  }
}

void USART_Hard_Flush(void) {
  long _lastMillis;
  
  Serial.flush(); // Flush serial output buffer.
  USART_Flush();  // Flush serial input buffer register.
  
  // Flush serial input buffer.
  _lastMillis = millis();
  while(Serial.available() != 0) {
    Serial.read();
    if((millis() - _lastMillis) > USART_TIMEOUT) {
      return;  
    }
  }
}

/* Push button reset 1 */
ISR(INT2_vect) {
  if((millis() - lastMillis1) > DEBOUNCING_TIME) {
       resetCounter1();
       lastMillis1 = millis();
  }
}

/* Lemo/coaxial reset 1 */
ISR(INT3_vect) {
  resetCounter1();
  }

/* Push button reset 2 */
ISR(INT0_vect) {
  if((millis() - lastMillis2) > DEBOUNCING_TIME) {
       resetCounter2();
       lastMillis2 = millis();
  }
}

/* Lemo/coaxial reset 2 */
ISR(INT1_vect) {
  resetCounter2();
  }

/* Gate1 */
ISR(INT4_vect) {
  if(en_status_ch1 == SELECT_LOCAL) {
    // if GATE1 is HIGH
    if(!!(PINE & (1 << PE4))) {
      digitalWrite(EN_CH1, HIGH);
      }else{
        digitalWrite(EN_CH1, LOW);
        }
    }
  }

/* Gate2 */
ISR(INT5_vect) {
  if(en_status_ch2 == SELECT_LOCAL) {
    // if GATE2 is HIGH
    if(!!(PINE & (1 << PE5))) {
      digitalWrite(EN_CH2, HIGH);
      }else{
        digitalWrite(EN_CH2, LOW);
        }
    }
  }
 
