/*
 * NIM-Counter firmware for the Open-NIM Counter module.
 * Copyright (C) 2022 Leonardo Lisa, Alluringbits 
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
 * APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
 * HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
 * IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
 * ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */
#pragma once

#include <avr/io.h>
#include <Arduino.h>
#include <string.h>

#define U8G2_WITHOUT_FONT_ROTATION
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


/* #define COMMAND_LENGTH 4 */
/* #define COMMANDS_COUNT 7 */
/* #define USART_TIMEOUT 100 */

#define DISPLAY

enum constants : uint8_t {COMMAND_LENGTH = 4, COMMANDS_COUNT = 8, USART_TIMEOUT = 100};
enum command : uint8_t {
	RSTA, //Reset all counters
	RST1, //Reset the first counter
	RST2, //Reset the second counter
	GET1, //Get the value of the first counter
	GET2, //Get the value of the second counter
	EN_1, //Check whether the first counter is in remote mode
	EN_2, //Check whether the second counter is in remote mode
	SRLN, //Return the serial number of the module
	NO_COMMAND, COMMAND_ERR
};

//Underscores before Defines and variable names are bad. User defined global variables, defines and enum values are supposed to be without underscore.
enum pins : uint8_t{
	_GAL_1,    //PA0
	_GAU_1,    //PA1
	_GBL_1,    //PA2
	_GBU_1,    //PA3
	_GAL_2,    //PA4
	_GAU_2,    //PA5
	_GBL_2,    //PA6
	_GBU_2,    //PA7
	_CLR2 = 34,//PC3
	_LATCH2,   //PC2
	_CLR1,     //PC1
	_LATCH1,   //PC0
	_RES = 44, //res PL5
	_DC,       //dc PL4 
	_D1,       //d1 PL3 
	_D0,       //d0 PL2 
	_CS,       //cs PL1 
	SELF_TEST_PIN,  
	_Y7 = 82,  //PK7 
	_Y6,       //PK6
	_Y5,       //PK5
	_Y4,       //PK4
	_Y3,       //PK3
	_Y2,       //PK2
	_Y1,       //PK1
	_Y0        //PK0
};

/* #define _RES 44   //res PL5 */ 
/* #define _DC 45    //dc PL4 */ 
/* #define _D1 46    //d1 PL3 */ 
/* #define _D0 47    //d0 PL2 */ 
/* #define _CS 48    //cs PL1 */ 

/* #define _Y7 82  //PK7 */
/* #define _Y6 83  //PK6 */
/* #define _Y5 84  //PK5 */
/* #define _Y4 85  //PK4 */
/* #define _Y3 86  //PK3 */
/* #define _Y2 87  //PK2 */
/* #define _Y1 88  //PK1 */
/* #define _Y0 89  //PK0 */

/* #define _GBU_2 7 //29  //PA7 */
/* #define _GBL_2 6 //28  //PA6 */
/* #define _GAU_2 5 //27  //PA5 */
/* #define _GAL_2 4 //26  //PA4 */
/* #define _GBU_1 3 //25  //PA3 */
/* #define _GBL_1 2 //24  //PA2 */
/* #define _GAU_1 1 //23  //PA1 */
/* #define _GAL_1 0 //22  //PA0 */

/* #define _CLR2   34  //PC3 */
/* #define _LATCH2 35  //PC2 */
/* #define _CLR1   36  //PC1 */
/* #define _LATCH1 37  //PC0 */

/* #define SELF_TEST_PIN 49 */

#ifdef DISPLAY
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ _D0, /* data=*/ _D1, /* cs=*/ _CS, /* dc=*/ _DC, /* reset=*/ _RES);
#endif

constexpr char commands[COMMANDS_COUNT][COMMAND_LENGTH]{
	"RSTA",
	"RST1",
	"RST2",
	"GET1",
	"GET2",
	"EN_1",
	"EN_2",
	"SRLN" //,
	// "TEST"
};

volatile uint32_t counter1{}; //uint64_t
volatile uint32_t counter2{};

void beginCounter();
void resetCounter1();
void resetCounter2();
/* void readCounter(); */ //deleted because below
//Separated the two read counter functions, when reading one counter it's not necessary to load and prepare everything to read the other too.
void readCounter1();
void readCounter2();

command getCommand(); 
void USART_Flush();
void USART_Hard_Flush();

