/*
 * pinMapping.h
 * 
 * Version: 1.00
 * Date: 26/02/2023
 * Author : Leonardo Lisa
 * leonardo.lisa@studenti.unimi.it
 * 
 */
 
#ifndef _PIN_MAPPING_
  /* Display pin definition */
  #define _RES_1 44   //res PL5 
  #define _DC_1 45    //dc PL4 
  #define _D1_1 46    //d1 PL3 
  #define _D0_1 47    //d0 PL2 
  #define _CS_1 48    //cs PL1
  
  #define _RES_2 35   //res PL0 
  #define _DC_2 50    //dc PB3 MISO
  #define _D1_2 51    //d1 PB2 MOSI
  #define _D0_2 52    //d0 PB1 SCK
  #define _CS_2 53    //cs PB0
  
  #define MISO 50    //dc PB3 MISO
  #define MOSI 51    //d1 PB2 MOSI
  #define SCK 52     //d0 PB1 SCK
  #define RST1 44    //res PL5 
  #define RST2 45    //dc PL4 
  #define CS1 48     //cs PL1
  #define CS2 47    //d0 PL2 
  
  /* Counter pin definition */
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
  
  /* Front panel IO pin definition */
  #define LOCAL1  A1 //PF0
  #define REMOTE1 A0 //PF1
  #define LOCAL2  A3 //PF2
  #define REMOTE2 A2 //PF3
  #define _LOCAL1  1 //PF0
  #define _REMOTE1 0 //PF1
  #define _LOCAL2  3 //PF2
  #define _REMOTE2 2 //PF3
  
  #define GATE1 2 //PE4 INT4
  #define GATE2 3 //PE5 INT5
  
  #define EN_CH1 4
  #define EN_CH2 5
  
  #define C_RST1 18 //PD3 INT3
  #define P_RST1 19 //PD2 INT2
  #define C_RST2 20 //PD1 INT1
  #define P_RST2 21 //PD0 INT0
#endif
