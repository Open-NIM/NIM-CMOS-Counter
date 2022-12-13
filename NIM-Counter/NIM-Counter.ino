#define U8G2_WITHOUT_FONT_ROTATION

#include <avr/io.h>

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define COMMAND_LENGTH 4
#define COMMANDS_COUNT 7
#define USART_TIMEOUT 100

#define DISPLAY 1

#define _RES 44   //res PL5 
#define _DC 45    //dc PL4 
#define _D1 46    //d1 PL3 
#define _D0 47    //d0 PL2 
#define _CS 48    //cs PL1 

#define _Y7 82  //PK7
#define _Y6 83  //PK6
#define _Y5 84  //PK5
#define _Y4 85  //PK4
#define _Y3 86  //PK3
#define _Y2 87  //PK2
#define _Y1 88  //PK1
#define _Y0 89  //PK0

#define _GBU_2 7 //29  //PA7
#define _GBL_2 6 //28  //PA6
#define _GAU_2 5 //27  //PA5
#define _GAL_2 4 //26  //PA4
#define _GBU_1 3 //25  //PA3
#define _GBL_1 2 //24  //PA2
#define _GAU_1 1 //23  //PA1
#define _GAL_1 0 //22  //PA0

#define _CLR2   34  //PC3
#define _LATCH2 35  //PC2
#define _CLR1   36  //PC1
#define _LATCH1 37  //PC0

#define SELF_TEST_PIN 49

#ifdef DISPLAY
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ _D0, /* data=*/ _D1, /* cs=*/ _CS, /* dc=*/ _DC, /* reset=*/ _RES);
#endif

const char commands[COMMANDS_COUNT][COMMAND_LENGTH] = {
  "RSTA",
  "RST1",
  "RST2",
  "GET1",
  "GET2",
  "EN_1",
  "EN_2" //,
  // "TEST"
  };

volatile uint32_t counter1 = 0; //uint64_t
volatile uint32_t counter2 = 0;

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
  pinMode(SELF_TEST_PIN, OUTPUT);
  digitalWrite(SELF_TEST_PIN, LOW);
  
  beginCounter();
  Serial.begin(9600);

#ifdef DISPLAY
    u8g2.begin();
    u8g2.enableUTF8Print();
#endif
}

void loop() {
  uint8_t cmd = getCommand();
  
  switch(cmd) {
    // Reset all
    case 0:
      // Serial.println("RSTA");
      resetCounter1();
      resetCounter2();
    break;
    
    // Reset counter 1
    case 1:
      // Serial.println("RST1");
      resetCounter1();
    break;

    // Reset counter 2
    case 2:
      //Serial.println("RST2");
      resetCounter2();
    break;
    
    // Read counter 1
    case 3:
      readCounter();
      Serial.println(counter1);
    break;

    // Read counter 2
    case 4:
      readCounter();
      Serial.println(counter2);
    break;
    
    // Remote control counter 1
    case 5:
      Serial.println("EN_1");
    break;

    // Remote control counter 2
    case 6:
      Serial.println("EN_2");
    break;

    case 7:
      USART_Hard_Flush();
      Serial.println("ERR!");
    break;
  }
  //USART_Hard_Flush();

  readCounter();

/* 26ms block execution time*/
#ifdef DISPLAY
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

void beginCounter(void) {
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
  
  if(Serial.available() >= COMMAND_LENGTH) {
    for(uint8_t i = 0; i < COMMAND_LENGTH; i++) {
      _buffer[i] = Serial.read();
    }
    USART_Hard_Flush();
  }else{
    return 8;
    }
  
  for(uint8_t j = 0; j < COMMANDS_COUNT; j++) {
    // Is a valid command.
    if(compareArrays(_buffer, j)) {
      return j;
    }
  }

  // Is an invalid command
  return COMMANDS_COUNT;
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
  long lastMillis;
  
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
