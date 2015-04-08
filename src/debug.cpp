/****************************************************************************
*  Copyright (c) 2011 by Michael Blandford. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may
*     be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
****************************************************************************/


#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifdef PCBSKY
 #ifndef PCBDUE
  #include "AT91SAM3S4.h"
 #else
	#include "sam3x8e.h"
 #endif
#endif

#ifdef PCBX9D
#include "x9d/stm32f2xx.h"
#include "x9d/stm32f2xx_gpio.h"
#include "X9D/i2c_ee.h"
#include "X9D/hal.h"
#endif

#include "core_cm3.h"

#include "ersky9x.h"
#include "sound.h"
#include "lcd.h"
#include "myeeprom.h"
#include "drivers.h"
#include "debug.h"
#include "file.h"
#include "ff.h"
#include "audio.h"
#include "timers.h"


#ifndef SIMU
#include "CoOS.h"
#endif



void uputs( register char *string ) ;
void crlf( void ) ;
void p8hex( uint32_t value ) ;
void p4hex( uint16_t value ) ;
void p2hex( unsigned char c ) ;
void hex_digit_send( unsigned char c ) ;

uint8_t Dbg_Spi_tx_buf[32] ;
uint8_t Dbg_Spi_rx_buf[32] ;

uint8_t I2CwriteValue ;
uint8_t I2CreadValue ;
uint8_t LedWriteValue ;

void uputs( register char *string )
{
	while ( *string )
	{
		txmit( *string++ ) ;		
	}	
}


// Send a <cr><lf> combination to the serial port
void crlf()
{
	txmit( 13 ) ;
	txmit( 10 ) ;
}

// Send the 32 bit value to the RS232 port as 8 hex digits
void p8hex( uint32_t value )
{
	p4hex( value >> 16 ) ;
	p4hex( value ) ;
}

// Send the 16 bit value to the RS232 port as 4 hex digits
void p4hex( uint16_t value )
{
	p2hex( value >> 8 ) ;
	p2hex( value ) ;
}

// Send the 8 bit value to the RS232 port as 2 hex digits
void p2hex( unsigned char c )
{
//	asm("swap %c") ;
	hex_digit_send( c >> 4 ) ;
//	asm("swap %c") ;
	hex_digit_send( c ) ;
}

// Send a single 4 bit value to the RS232 port as a hex digit
void hex_digit_send( unsigned char c )
{
	c &= 0x0F ;
	if ( c > 9 )
	{
		c += 7 ;
	}
	c += '0' ;
	txmit( c ) ;
}

extern "C" void utxmit( uint8_t c ) ;

extern "C" void utxmit( uint8_t c )
{
	txmit( c ) ;
}

extern "C" void up8hex( uint32_t value ) ;

extern "C" void up8hex( uint32_t value )
{
	p8hex( value ) ;
}

//extern void read_volume( void ) ;
//extern uint8_t Volume_read ;

#define YMODEM 				0
#define VOICE_TEST		0

//#include "s9xsplash.lbm"

//extern uint32_t Per10ms_action ;
//extern uint32_t Permenu_action ;
void disp_256( uint32_t address, uint32_t lines ) ;
extern void dispw_256( register uint32_t address, register uint32_t lines ) ;
//extern uint8_t eeprom[] ;

#if YMODEM
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)
#define PACKET_TRAILER          (2)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  (0x61)  /* 'a' == 0x61, abort by user */

#define NAK_TIMEOUT             (100)	// Units of 2mS 
#define PACKET_TIMEOUT          (25)		// Units of 2mS 
#define MAX_ERRORS              (5)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int32_t Ymodem_Receive (uint8_t *p ) ;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//uint8_t file_name[FILE_NAME_LENGTH];
//uint32_t FlashDestination = ApplicationAddress; /* Flash user program offset */
//uint16_t PageSize = PAGE_SIZE;
//uint32_t EraseCounter = 0x0;
//uint32_t NbrOfPage = 0;
//FLASH_Status FLASHStatus = FLASH_COMPLETE;
uint32_t RamSource;
extern uint8_t tab_1024[1024];
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Receive_Byte
* Description    : Receive byte from sender
* Input          : - c: Character
*                  - timeout: Timeout
* Output         : None
* Return         : 0: Byte received
*                  -1: Timeout
*******************************************************************************/
extern struct t_fifo32 Console_fifo ;

struct t_fifo64 HexStreamFifo ;

extern uint16_t Sine_values[] ;
uint32_t PlayVoice ;



FILINFO FileInfo ;


#ifdef	DEBUG
uint32_t Mem_address ;
uint32_t Next_mem_address ;

uint32_t Memaddmode ;
uint32_t SoundCheck ;

#if VOICE_TEST
uint8_t Sdcard_data[1024] ;
#endif

#if YMODEM
uint8_t Ymbuffer[10] ;
uint8_t file_name[FILE_NAME_LENGTH];
#endif

#if VOICE_TEST
FIL T_file ;
FATFS g_FATFS_Obj ;

DIR Dir ;
#endif

//extern uint16_t Captures[256] ;
extern uint32_t Cap_index ;
extern uint8_t Activated ;

uint8_t EEdata[256] ;
uint32_t Tcap_index ;

void handle_serial(void* pdata)
{
	uint16_t rxchar ;

	while ( Activated == 0 )
	{
		CoTickDelay(10) ;					// 20mS
	}

#if VOICE_TEST
	static uint32_t SdAddress = 0 ;
#endif
	for(;;)
	{
#ifndef PCBDUE
		while ( g_model.frskyComPort )		// Leave the port alone!
		{
			CoTickDelay(50) ;					// 100mS for now
		}
#endif
		
#if PCBSKY		
#ifndef PCBDUE
		if ( SoundCheck )
		{
			if ( queueTone( 610, 200, 30, 0 ) )
			{
				SoundCheck = 0 ;
			}			
		}
#endif
#endif

		while ( ( rxchar = rxuart() ) == 0xFFFF )
		{
			CoTickDelay(5) ;					// 10mS for now
#ifndef PCBDUE
			if ( g_model.frskyComPort )		// Leave the port alone!
			{
				break ;
			}
#endif
		}
#ifndef PCBDUE
		if ( g_model.frskyComPort )		// Leave the port alone!
		{
			continue ;
		}
#endif
		// Got a char, what to do with it?

		if ( Memaddmode )
		{
			rxchar = toupper( rxchar ) ;
			if ( ( ( rxchar >= '0' ) && ( rxchar <= '9' ) ) || ( ( rxchar >= 'A' ) && ( rxchar <= 'F' ) ) )
			{
				txmit( rxchar ) ;
				rxchar -= '0' ;
				if ( rxchar > 9 )
				{
					rxchar -= 7 ;				
				}
				Mem_address <<= 4 ;
				Mem_address |= rxchar ;			
			}
			else if ( rxchar == 13 )
			{
				crlf() ;
				if ( Mem_address == 0 )
				{
					Mem_address = Next_mem_address ;
				}
				dispw_256( Mem_address, 4 ) ;
				Next_mem_address = Mem_address + 64 ;
				Memaddmode = 0 ;				
			}
			else if ( rxchar == 8 )
			{
				txmit( rxchar ) ;
				txmit( rxchar ) ;
				txmit( rxchar ) ;
				Mem_address >>= 4 ;			
			}
			else if ( rxchar == 27 )
			{
				crlf() ;
				Memaddmode = 0 ;				
			}		

		}

//extern void usbMassStorage( void ) ;
//extern uint32_t UsbTimer ;

//		if ( rxchar == 'm' )
//		{
//			txmit( 'm' ) ;
//			if ( UsbTimer < 5000 )		// 10 Seconds
//			{
//				UsbTimer = 5001 ;
//			}
//			for(;;)
//			{
//				CoSchedLock() ;
//				if ( Voice.VoiceLock == 0 )
//				{
//					break ;
//				}
//  		  CoSchedUnlock() ;
//				CoTickDelay(1) ;					// 2mS
//			}
//			Voice.VoiceLock = 1 ;
//  		CoSchedUnlock() ;
//			txmit( 's' ) ;
//			while ( rxchar != 0x1B )
//			{
//      	usbMassStorage();
//      	CoTickDelay(1);  // 5*2ms for now
//				rxchar = rxuart() ;
//			}
//			Voice.VoiceLock = 0 ;
//			crlf() ;
//		}

#if VOICE_TEST
		if ( ( rxchar == 'M' ) || ( rxchar == 'N' ) )
		{
			FRESULT fr ;
			UINT nread ;
			uint32_t x ;
			uint32_t w8or16 ;
			
			txmit( rxchar ) ;
			txmit( ' ' ) ;
  		fr = f_mount(0, &g_FATFS_Obj);
			p2hex( fr ) ;
			txmit( ' ' ) ;
  		if ( fr == FR_OK)
			{
				fr = f_stat( (rxchar == 'M') ? "0000.wav" : "0001.wav", &FileInfo ) ;
				p2hex( fr ) ;
				txmit( ' ' ) ;
				p8hex( FileInfo.fsize ) ;
				fr = f_open( &T_file, (rxchar == 'M') ? "0000.wav" : "0001.wav", FA_READ ) ;
				p2hex( fr ) ;
				txmit( ' ' ) ;
				if ( fr == FR_OK )
				{
					fr = f_read( &T_file, Sdcard_data, 1024, &nread ) ;
					p2hex( fr ) ;
					txmit( ' ' ) ;
					p4hex( nread ) ;
					txmit( ' ' ) ;
					x = Sdcard_data[34] + ( Sdcard_data[35] << 8 ) ;		// sample size
					p4hex( x ) ;
					w8or16 = x ;
					txmit( ' ' ) ;
					x = Sdcard_data[24] + ( Sdcard_data[25] << 8 ) ;		// sample rate
					p4hex( x ) ;
					crlf() ;
					txmit( '+' ) ;
					// 8 bit unsigned 11025 Hz

					if ( w8or16 == 8 )
					{
						wavU8Convert( &Sdcard_data[44], VoiceBuffer[0].data, 512-44 ) ;
						VoiceBuffer[0].count = 512-44 ;
					}
					else if ( w8or16 == 16 )
					{
						wavU16Convert( (uint16_t*)&Sdcard_data[44], VoiceBuffer[0].data, 512-44/2 ) ;
						VoiceBuffer[0].count = 512-44/2 ;
					}
					else
					{
						break ;		// can't convert
					}
					VoiceBuffer[0].frequency = x ;		// sample rate

					if ( w8or16 == 8 )
					{
						wavU8Convert( &Sdcard_data[512], VoiceBuffer[1].data, 512 ) ;
					}
					else
					{
						fr = f_read( &T_file, (uint8_t *)Sdcard_data, 1024, &nread ) ;
						p2hex( fr ) ;
						txmit( ' ' ) ;
						p4hex( nread ) ;
						txmit( ' ' ) ;
						wavU16Convert( (uint16_t*)&Sdcard_data[0], VoiceBuffer[1].data, 512 ) ;
					}
					VoiceBuffer[1].count = 512 ;
					VoiceBuffer[1].frequency = 0 ;
					
					fr = f_read( &T_file, (uint8_t *)Sdcard_data, (w8or16 == 8) ? 512 : 1024, &nread ) ;		// Read next buffer
					if ( w8or16 == 8 )
					{
						wavU8Convert( &Sdcard_data[0], VoiceBuffer[2].data, 512 ) ;
					}
					else
					{
						wavU16Convert( (uint16_t*)&Sdcard_data[0], VoiceBuffer[2].data, 512 ) ;
					}
					VoiceBuffer[2].count = 512 ;
					VoiceBuffer[2].frequency = 0 ;
					startVoice( 3 ) ;
//							rxchar = 0xFFFF ;
					for(x = 0;;)
					{
						fr = f_read( &T_file, (uint8_t *)Sdcard_data, (w8or16 == 8) ? 512 : 1024, &nread ) ;		// Read next buffer
						txmit( ' ' ) ;
						p4hex( nread ) ;
						if ( nread == 0 )
						{
							break ;
						}
	  				while ( ( VoiceBuffer[x].flags & VF_SENT ) == 0 )
						{
							CoTickDelay(1) ;					// 2mS for now
//								if ( ( rxchar = rxuart() ) != 0xFFFF )
//								{
//									break ;
//								}
						}
//								if ( rxchar != 0xFFFF )
//								{
//									break ;
//								}
						if ( w8or16 == 8 )
						{
							wavU8Convert( &Sdcard_data[0], VoiceBuffer[x].data, nread ) ;
						}
						else
						{
							nread /= 2 ;
							wavU16Convert( (uint16_t*)&Sdcard_data[0], VoiceBuffer[x].data, nread ) ;
						}
						VoiceBuffer[x].count = nread ;
						VoiceBuffer[x].frequency = 0 ;
						appendVoice( x ) ;					// index of next buffer
						x += 1 ;
						if ( x > 2 )
						{
							x = 0 ;							
						}
					}
					fr = f_close( &T_file ) ;
					p2hex( fr ) ;
				}
			}
		}

		if ( rxchar == 'J' )
		{
			// Test Sd card
			FRESULT fr ;
			UINT nread ;

			txmit( 'J' ) ;
			txmit( ' ' ) ;
			
  		fr = f_mount(0, &g_FATFS_Obj);
			p2hex( fr ) ;
			txmit( ' ' ) ;
  		if ( fr == FR_OK)
			{
				fr = f_open( &T_file, "Tfile", FA_READ ) ;
				p2hex( fr ) ;
				txmit( ' ' ) ;
				if ( fr == FR_OK )
				{
					fr = f_read( &T_file, (uint8_t *)Sdcard_data, 512, &nread ) ;
					p2hex( fr ) ;
					txmit( ' ' ) ;
					p4hex( nread ) ;
					fr = f_close( &T_file ) ;
					p2hex( fr ) ;
				}
			}
			crlf() ;
		}
#endif
	 
#ifdef PCBX9D

extern void initWatchdog( void ) ;

		if ( rxchar == 'w' )
		{
			// Watchdog test
			txmit( 'w' ) ;
			initWatchdog() ;
		}

		if ( rxchar == 'z' )
		{
			// Watchdog test
			txmit( 'z' ) ;
			wdt_reset() ;
		}

		if ( rxchar == 'y' )
		{
			// Watchdog test
			txmit( 'y' ) ;
			__disable_irq() ;
			for(;;)
			{
				// Cause a watchdog reset.
			}
		}
		 
		if ( rxchar == 'P' )
		{
			txmit( 'p' ) ;
			if ( g_model.protocol == PROTO_PPM )
			{
				g_model.protocol = PROTO_PXX ;
				txmit( 'x' ) ;
			}
			else
			{
				g_model.protocol = PROTO_PPM ;
				txmit( 'm' ) ;
			}
		}

		static uint32_t bk_address = 0 ;
		if ( rxchar == 'X' )
		{
			txmit( 'X' ) ;
			bk_address = 0 ;
			crlf() ;
			p8hex( bk_address ) ;
			crlf() ;
			uint32_t i ;
			for ( i = 0 ; i < 128 ; i += 1 )
			{
				EEdata[i] = 0x55 ;
			}
			disp_256( (uint32_t)EEdata, 8 ) ;
			I2C_EE_BufferRead( EEdata, 0, 128 ) ;
			txmit( 'y' ) ;
			disp_256( (uint32_t)EEdata, 8 ) ;
		}

		if ( rxchar == 'x' )
		{
			txmit( 'x' ) ;
			crlf() ;
			p8hex( bk_address ) ;
			crlf() ;
			uint32_t i ;
			for ( i = 0 ; i < 128 ; i += 1 )
			{
				EEdata[i] = 0x55 ;
			}
			disp_256( (uint32_t)EEdata, 8 ) ;
			I2C_EE_BufferRead( EEdata, bk_address, 128 ) ;
			txmit( 'y' ) ;
			disp_256( (uint32_t)EEdata, 8 ) ;
			bk_address += 128 ;
		}

		if ( rxchar == 'g' )
		{
void generalDefault() ;
			generalDefault() ;			
		}

		if ( rxchar == 'W' )
		{
			txmit( 'w' ) ;
			uint32_t i ;
			for ( i = 0 ; i < 128 ; i += 1 )
			{
				EEdata[i] = i ;
			}
			disp_256( (uint32_t)EEdata, 128 ) ;
			WP_L ;
			I2C_EE_BufferWrite( EEdata, 0, 128 ) ;
			txmit( 'z' ) ;
		}	
		
		if ( rxchar == 'Q' )
		{
			txmit( 'q' ) ;
			uint32_t i ;
			for ( i = 0 ; i < 128 ; i += 1 )
			{
				EEdata[i] = 0xFF ;
			}
			disp_256( (uint32_t)EEdata, 8 ) ;
			WP_L ;
			I2C_EE_BufferWrite( EEdata, 0, 128 ) ;
			txmit( 'z' ) ;
		}	

		if ( rxchar == 'R' )
		{
			txmit( 'R' ) ;
			PlayVoice = 1 ;			
		}

		if ( rxchar == 'S' )
		{
			txmit( 'S' ) ;
			eeReadAll() ;
		}

#ifdef REV9E
//extern void ht1621WrData( uint8_t data, uint8_t count ) ;
//extern void ht1621WrAllData( uint8_t *pData, uint8_t chip  ) ;
extern void initTopLcd( void ) ;
static uint8_t Ht1621Data[16] = {	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff } ;
		if ( rxchar == 'a' )
		{
			txmit( 'a' ) ;
			initTopLcd() ;			
		}

		if ( rxchar == 'b' )
		{
			txmit( 'b' ) ;
//			ht1621WrAllData( Ht1621Data, 0 ) ;
//			ht1621WrAllData( Ht1621Data, 1 ) ;
			uint32_t i ;
			uint8_t j = 0 ;
			uint8_t k = 0 ;
			for ( i = 0 ; i < 16 ; i += 1 )
			{
				if ( j )
				{
					Ht1621Data[i] = j ;
					j = 0 ;
				}
				else
				{
					if ( Ht1621Data[i] == 1 )
					{
						j = 0x80 ;
					}
					else
					{
						j = 0 ;
					}
					Ht1621Data[i] >>= 1 ;
				}
				k |= Ht1621Data[i] ;
			}
			if ( ( k == 0 ) || ( k == 0xFF ) )
			{
				Ht1621Data[0] = 0x80 ;
			}
			p2hex( Ht1621Data[0] ) ;
			p2hex( Ht1621Data[1] ) ;
			p2hex( Ht1621Data[2] ) ;
		}
#endif

//extern int8_t EeFsck() ;
//extern bool eeLoadGeneral() ;
//extern bool EeFsOpen() ;

//		if ( rxchar == 't' )
//		{
//  		EeFsOpen() ;
//		}
//		if ( rxchar == 'u' )
//		{
//			EeFsck() ;
//			txmit( '<' ) ;
//			disp_256( (uint32_t)&g_model, 8 ) ;
//		}
//		if ( rxchar == 'v' )
//		{
//      eeLoadGeneral() ;
//		}


//		if ( rxchar == 'L' )
//		{
//			lcd_init() ;		
//		}

#endif

		if ( rxchar == '?' )
		{
			Memaddmode = 1 ;
			Mem_address = 0 ;
			txmit( '>' ) ;
		}

		if ( rxchar == 'V' )
		{
			uputs( (char *)VERSION ) ;
			crlf() ;
		}
		
#ifdef PCBX9D
		if ( rxchar == '+' )
		{
			uint8_t volume ;
			volume = I2C_read_volume() ;
			if ( volume < 127 )
			{
				I2C_set_volume( ++volume ) ;				
			}
		}


		if ( rxchar == '-' )
		{
			uint8_t volume ;
			volume = I2C_read_volume() ;
			if ( volume > 0 )
			{
				I2C_set_volume( --volume ) ;				
			}
		}
#endif

#ifdef ASSAN
		if ( rxchar == 'H' )
		{
			uint32_t x ;
			int32_t y ;
			x = 0 ;
			crlf() ;
			for(;;)
			{
				while ( (y = get_fifo64(&HexStreamFifo)) != -1 )
				{
					p2hex( y ) ;
					txmit( ' ' ) ;
					x += 1 ;
					if ( x > 15 )
					{
						crlf() ;
						x = 0 ;
					}
				}
				CoTickDelay(1) ;					// 2mS

				if ( ( rxchar = rxuart() ) != 0xFFFF )
				{
					if ( rxchar == 'X' )
					{
						break ;
					}
				}
			}
		}
#endif

//#ifdef ASSAN
//extern uint8_t ProtocolDebug[16384] ;
//extern uint32_t ProtocolCount ;
//extern uint32_t ProtocolIn ;


//		if ( rxchar == 'A' )
//		{
//			uint32_t start ;
//			int32_t count ;
//			uint32_t i ;
//			uint32_t hold ;

//			hold = ProtocolCount ;
//			ProtocolCount = 0xFFFFFFFF ;

//			start = 0 ;
//			if ( hold >= 16384 )
//			{
//				start = ProtocolIn ;
//			}
//			count = hold ;
//			start &= 0xFFFFFFF8 ;
//			count &= 0xFFFFFFF8 ;
//			p8hex( start ) ;
//			crlf() ;
//			p8hex( count ) ;
//			crlf() ;
//			p8hex( ProtocolIn ) ;
//			crlf() ;
//			p8hex( hold ) ;
//			crlf() ;

//			while ( count > 0 )
//			{
//				txmit( ProtocolDebug[start++] ) ;
//				for ( i = 0 ; i < 7 ; i += 1 )
//				{
//					txmit( ' ' ) ;
//					p2hex( ProtocolDebug[start++] ) ;
//				}
//				crlf() ;
//				start &= 16383 ;
//				count -= 8 ;
//			}
//			ProtocolCount = hold ;
			
			
//		}
//#endif
//		if ( rxchar == 'B' )
//		{
//			register Adc *padc ;

//			padc = ADC ;
//			p8hex( padc->ADC_CDR4 ) ;
//			crlf() ;
//			read_9_adc() ;
//			DACC->DACC_CDR = padc->ADC_CDR4 ;		// Battery 
//		}
	
//		if ( rxchar == 'R' )
//		{
//			register const volatile uint32_t *pword ;
//			register uint32_t i ;

//			pword = &ADC->ADC_CDR0 ;
//			txmit( 'R' ) ;
//			crlf() ;
//			for ( i = 0 ; i < 16 ; i += 1 )
//			{
//				p8hex( *pword++ ) ;
//				crlf() ;
//			}
//		}
	
//		if ( rxchar == 'K' )
//		{
//			txmit( 'K' ) ;
//			p8hex( read_keys() ) ;
//			crlf() ;
//		}
	
//		if ( rxchar == 'T' )
//		{
//			txmit( 'T' ) ;
//			p4hex( read_trims() ) ;
//			crlf() ;
//		}

//		if ( rxchar == 'S' )
//		{
//			txmit( 'E' ) ;
//			p2hex( keyState( SW_ElevDR ) ) ;
//			crlf() ;
//			txmit( 'A' ) ;
//			p2hex( keyState( SW_AileDR ) ) ;
//			crlf() ;
//			txmit( 'R' ) ;
//			p2hex( keyState( SW_RuddDR ) ) ;
//			crlf() ;
//			txmit( 'G' ) ;
//			p2hex( keyState( SW_Gear ) ) ;
//			crlf() ;
//			txmit( 'C' ) ;
//			p2hex( keyState( SW_ThrCt ) ) ;
//			crlf() ;
//			txmit( 'T' ) ;
//			p2hex( keyState( SW_Trainer ) ) ;
//			crlf() ;
//			txmit( '0' ) ;
//			txmit( ' ' ) ;
//			p2hex( keyState( SW_ID0 ) ) ;
//			crlf() ;
//			txmit( '1' ) ;
//			txmit( ' ' ) ;
//			p2hex( keyState( SW_ID1 ) ) ;
//			crlf() ;
//			txmit( '2' ) ;
//			txmit( ' ' ) ;
//			p2hex( keyState( SW_ID2 ) ) ;
//			crlf() ;
//		}

//		if ( rxchar == 'Z' )
//		{
//			txmit( 'A' ) ;
//			txmit( ' ' ) ;
//			p8hex( PIOA->PIO_PDSR ) ;
//			crlf() ;
//			txmit( 'B' ) ;
//			txmit( ' ' ) ;
//			p8hex( PIOB->PIO_PDSR ) ;
//			crlf() ;
//			txmit( 'C' ) ;
//			txmit( ' ' ) ;
//			p8hex( PIOC->PIO_PDSR ) ;
//			crlf() ;
//		}
		 
//		if ( rxchar == 'L' )
//		{
//			lcd_clear() ;
//			lcd_init() ;		
//			lcd_putsn_P( 7*FW, 0, "ERSKY9X", 7 ) ;
//			refreshDisplay() ;
//		}

#if VOICE_TEST
		if ( rxchar == 'D' )
		{ // Directory listing
			txmit( 'D' ) ;
			crlf() ;					
			FRESULT res ;
			char *fn ;

			f_chdir( ".\\VOICE" ) ;

  		res = f_opendir(&Dir, ".") ;        /* Open the directory */
  		if (res == FR_OK)
			{
				for(;;)
				{
					res = f_readdir( &Dir, &FileInfo ) ;
      		if (res != FR_OK || FileInfo.fname[0] == 0)
						break;  /* Break on error or end of dir */
					fn = FileInfo.fname ;
					while ( *fn )
					{
						txmit( *fn++ ) ;						
					}
					crlf() ;					
				}				 

			}	
		}
#endif

#if YMODEM
		if ( rxchar == 'Y' )
		{ // Enter Ymodem mode
			int32_t result ;
int32_t Ymodem_Receive( uint8_t *buf ) ;
			txmit( 'Y' ) ;
			result = Ymodem_Receive( Ymbuffer ) ;
			p8hex( result ) ;
			crlf() ;
			char *fn ;
			fn = (char *)file_name ;
			while ( *fn )
			{
				txmit( *fn++ ) ;						
			}
			crlf() ;
		}
#endif

#if PCBSKY
//		if ( rxchar == 'f' )
//		{
//			uint32_t i ;
//			for ( i = 0 ; i < 21 ; i += 1 )
//			{
//				crlf() ;
//				p8hex( File_system[i].block_no ) ;
//				txmit( ' ' ) ;
//				p8hex( File_system[i].sequence_no ) ;
//				txmit( ' ' ) ;
//				p4hex( File_system[i].size ) ;
//				txmit( ' ' ) ;
//				p4hex( File_system[i].flags ) ;
//				txmit( ' ' ) ;
//				p2hex( ModelNames[i][0] ) ;
//				p2hex( ModelNames[i][1] ) ;
//				p2hex( ModelNames[i][2] ) ;
//				p2hex( ModelNames[i][3] ) ;
//			}
//		}

//		if ( rxchar == 'H' )
//		{
//			txmit( 'H' ) ;
//			hapticOn( 40 ) ;
//		}

//		if ( rxchar == 'I' )
//		{
//			txmit( 'I' ) ;
//			hapticOff() ;
//		}

//	extern void read_volume( void ) ;
//	extern uint8_t Volume_read ;
//	extern void read_coprocessor( void ) ;
//	extern uint8_t Coproc_read ;
//	extern int8_t Coproc_valid ;
	
//		if ( rxchar == ';' )
//		{
//			read_volume() ;
//			txmit( 'W' ) ;
//			txmit( '-' ) ;
//			p2hex( Volume_read ) ;
//			crlf() ;
//		}

//		if ( rxchar == 'Q' )
//		{
//			read_coprocessor() ;
//			txmit( 'Q' ) ;
//			txmit( '-' ) ;
//			p2hex( Coproc_read ) ;
//			txmit( ' ' ) ;
//			p2hex( Coproc_valid ) ;
//			crlf() ;
//		}


	//		*(p+4) = 0 ;
	//		*(p+5) = 0 ;
	//		*(p+19) = 0 ;

	//		p = Spi_tx_buf ;
	//		*p = 3 ;		// Read command
	//		*(p+1) = 0 ;
	//		*(p+2) = 0 ;
	//		*(p+3) = 0 ;		// 3 bytes address
	//		x = spi_operation( p, Spi_rx_buf, 20 ) ;

	//		p8hex( x ) ;
	//		txmit( ' ' ) ;
	//		p = Spi_rx_buf ;
	//		p2hex( *(p+4) ) ;
	//		p2hex( *(p+5) ) ;
	//		p2hex( *(p+18) ) ;
	//		p2hex( *(p+19) ) ;
	//		crlf() ;
	//	}

#if VOICE_TEST
	#define SD_ST_EMPTY		0
	#define SD_ST_IDLE		1
	#define SD_ST_READY		2
	#define SD_ST_IDENT		3
	#define SD_ST_STBY		4
	#define SD_ST_TRAN		5
	#define SD_ST_DATA		6

		if ( rxchar == 'E' )
		{
			uint32_t i ;
	//		static uint32_t card_state = 0 ;

	//		txmit( 'E' ) ;
	//		txmit( '-' ) ;
	//		switch ( card_state )
	//		{
	//			case SD_ST_EMPTY :
	//				i = ( PIOB->PIO_PDSR & PIO_PB7 ) ? 0 : 1 ;
	//				txmit( 'e' ) ;
	//				txmit( ' ' ) ;
	//  			p2hex( i ) ;
	//				if ( i )
	//				{
	//					card_state = SD_ST_IDLE ;
	//				}
	//				crlf() ;
	//			break ;

	//			case SD_ST_IDLE :
	//				i = sd_acmd41() ;
	//				txmit( 'L' ) ;
	//				txmit( ' ' ) ;
	//				p8hex( i ) ;
	//				crlf() ;
	//				if ( i & 0x80000000 )
	//				{
	//					card_state = SD_ST_READY ;
	//				}
	//			break ;

	//			case SD_ST_READY :
	//				i = sd_cmd2() ;
	//				card_state = SD_ST_IDENT ;
	//				txmit( 'R' ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[0] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[1] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[2] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[3] ) ;
	//				crlf() ;
	//			break ;

	//			case SD_ST_IDENT :
	//				i = sd_cmd3() ;
	//				card_state = SD_ST_STBY ;
	//				Sd_rca = i ;
	//				txmit( 'T' ) ;
	//				txmit( ' ' ) ;
	//				p8hex( i ) ;
	//				crlf() ;
	//			break ;

	//			case SD_ST_STBY :
	//				i = sd_cmd9() ;
	//				txmit( 'S' ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[0] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[1] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[2] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[3] ) ;
	//				txmit( '-' ) ;
	//				p8hex(HSMCI->HSMCI_SR) ;
	//				txmit( ' ' ) ;
	//				p8hex(HSMCI->HSMCI_SR) ;
	//				i = sd_cmd7() ;
	//				crlf() ;
	//				card_state = SD_ST_TRAN ;
	//			break ;

//	SdAddress = 0 ;

	//			case SD_ST_TRAN :
	//				txmit( 't' ) ;
	//				txmit( ' ' ) ;
	//				Sd_128_resp[0] = 0 ;
	//				Sd_128_resp[1] = 0 ;
	//				i = sd_acmd51( Sd_128_resp ) ;
	//				p8hex( Sd_128_resp[0] ) ;
	//				txmit( ' ' ) ;
	//				p8hex( Sd_128_resp[1] ) ;
	//				card_state = SD_ST_DATA ;
	//				i = sd_acmd6() ;
	//				txmit( '+' ) ;
	//				p8hex( i ) ;
	//				crlf() ;
	//				SdAddress = 0 ;
	//			break ;
			
	//			case SD_ST_DATA :
	//				txmit( 'D' ) ;
	//				txmit( ' ' ) ;
					SdAddress = 0x39FF ;
					i = sd_read_block( SdAddress, (uint32_t *)Sdcard_data ) ;
					p8hex( SdAddress ) ;
					txmit( ':' ) ;
					p8hex( i ) ;
					crlf() ;
					SdAddress += 1 ;
	//			break ;
	//		}
		}

		if ( rxchar == 'e' )
		{
			uint32_t i ;
			uint32_t j ;
			
//			SdAddress = 0x2000 ;

			for ( j = 0 ; j < 128 ; j += 1 )
			{
				i = sd_read_block( SdAddress, (uint32_t *)Sdcard_data ) ;
				p8hex( SdAddress ) ;
				txmit( ':' ) ;
				p8hex( i ) ;

				if ( * ((uint8_t *)Sdcard_data + 511) == 0xAA )
				{
					txmit( ' ' ) ;
					txmit( 'A' ) ;
				}

				crlf() ;
				SdAddress += 1 ;
			}
		}
#endif

		if ( rxchar == 'i' )
		{
			txmit( 'i' ) ;
			init23008() ;
			crlf() ;
		}

		if ( rxchar == 'j' )
		{
			txmit( 'j' ) ;
			I2CwriteValue += 4 ;
			I2CwriteValue &= 0xFC ;
			write23008( I2CwriteValue ) ;
			crlf() ;
		}

		if ( rxchar == 'k' )
		{
			txmit( 'k' ) ;
			read23008( &I2CreadValue ) ;
			crlf() ;
		}
		
extern uint8_t I2Cdebug ;
		if ( rxchar == 'l' )
		{
			txmit( 'l' ) ;
			p2hex( I2CreadValue & 3 ) ;
			txmit( I2Cdebug ) ;
			crlf() ;
		}

		if ( rxchar == 'p' )
		{
			txmit( 'p' ) ;
			initLed() ;
			txmit( I2Cdebug ) ;
			crlf() ;
		}

		if ( rxchar == 'q' )
		{
			txmit( 'q' ) ;
			LedWriteValue += 4 ;
			writeLed( LedWriteValue ) ;
			txmit( I2Cdebug ) ;
			crlf() ;
		}

		if ( rxchar == 'r' )
		{
			txmit( 'r' ) ;
			readLed( &I2CreadValue ) ;
			p2hex( I2CreadValue ) ;
			txmit( I2Cdebug ) ;
			crlf() ;
		}

//extern uint16_t SerTimes[] ;
//extern uint16_t SerValues[] ;
//extern uint16_t SerBitStates[] ;
//extern uint16_t SerBitCounts[] ;
//extern uint16_t SerBitInputs[] ;
//extern uint32_t SerIndex ;

//		if ( rxchar == 't' )
//		{
//			txmit( 'r' ) ;
//			crlf() ;
//			uint32_t i ;
//			uint32_t j ;
//			j = SerIndex ;
//			for ( i = 0 ; i < 64 ; i += 1 )
//			{
//				p4hex( SerTimes[j] ) ;
//				txmit( ' ' ) ;
//				p2hex( SerValues[j] ) ;
//				txmit( ' ' ) ;
//				p2hex( SerBitStates[j] ) ;
//				txmit( ' ' ) ;
//				p2hex( SerBitCounts[j] ) ;
//				txmit( ' ' ) ;
//				p2hex( SerBitInputs[j] ) ;
//				crlf() ;
//				j += 1 ;
//				if ( j > 63 )
//				{
//					j = 0 ;
//				}
//			}
//		}

//		if ( rxchar == 'm' )
//		{
//			register uint8_t *p ;
//			register uint32_t x ;
			
//			txmit( 'm' ) ;
//			p = Dbg_Spi_rx_buf ;
//			*(p) = 1 ;
//			*(p+1) = 2 ;
//			*(p+2) = 3 ;
//			*(p+3) = 4 ;

//			eeprom_write_enable() ;

//			p = Dbg_Spi_tx_buf ;
//			*p = 2 ;		// Write command
//			*(p+1) = 0 ;
//			*(p+2) = 0 ;
//			*(p+3) = 0 ;

//			spi_PDC_action( p, Dbg_Spi_rx_buf, 0, 4, 4 ) ;

//			for ( x = 0 ; x < 100000 ; x += 1  )
//			{
////				if ( Spi_complete )
////				{
////					break ;				
////				}
//				asm("") ;
//			}

			
//		}

//		if ( rxchar == 'o' )
//		{
//			register uint8_t *p ;
//			register uint32_t x ;

//			txmit( 'o' ) ;
//			p = Dbg_Spi_rx_buf ;
//			*(p) = 0 ;
//			*(p+1) = 0 ;
//			*(p+2) = 0 ;
//			*(p+3) = 0 ;

//			p = Dbg_Spi_tx_buf ;
//			*p = 3 ;		// Read command
//			*(p+1) = 0 ;
//			*(p+2) = 0 ;
//			*(p+3) = 0 ;
//			spi_PDC_action( p, 0, Dbg_Spi_rx_buf, 4, 24 ) ;
//			for ( x = 0 ; x < 100000 ; x += 1  )
//			{
////				if ( Spi_complete )
////				{
////					break ;				
////				}
//				asm("") ;
//			}

//			p8hex( x ) ;
//			txmit( ' ' ) ;
//			p = Dbg_Spi_rx_buf ;
//			p2hex( *p ) ;
//			p2hex( *(p+1) ) ;
//			p2hex( *(p+2) ) ;
//			p2hex( *(p+3) ) ;
//			crlf() ;
//		}

//		if ( rxchar == 'F' )
//		{
//			txmit( 'F' ) ;
//			txmit( '-' ) ;
//  		p2hex( ( PIOB->PIO_PDSR & PIO_PB7 ) ? 0 : 1 ) ;
//			crlf() ;
//		}	
	
//		if ( rxchar == 'U' )
//		{
//			uint32_t xfer ;
////			uint32_t done = 0 ;

//			txmit( 'U' ) ;
//			crlf() ;
//			for(;;)
//			{
//				xfer = 0 ;
//				if ( ( rxchar = rxuart() ) != 0xFFFF )
//				{
//					if ( rxchar == 27 )		// ESCAPE
//					{
//						break ;						
//					}
//					txmitBt( rxchar ) ;
//					xfer = 1 ;
//				}
//				if ( ( rxchar = rxBtuart() ) != 0xFFFF )
//				{
//					if ( rxchar == 27 )		// ESCAPE
//					{
//						break ;						
//					}
//					txmit( rxchar ) ;
//					xfer = 1 ;
//				}
//				if ( xfer == 0 )
//				{
//					CoTickDelay(1) ;					// 2mS
//				}				 
//	    }
//	  }
	
	//	if ( rxchar == 'F' )
	//	{
	//		register uint8_t *p ;
	//		register uint32_t x ;

	//		txmit( 'F' ) ;

	//		eeprom_write_enable() ;
		
	//		p = Spi_tx_buf ;
	//		*p = 1 ;		// Write status register command
	//		*(p+1) = 0 ;

	//		x = spi_operation( p, Spi_rx_buf, 2 ) ;
	//		p8hex( x ) ;
	//		crlf() ;
	//	}
	
	//		register uint8_t *p ;
	//		register uint32_t x ;
		
	//		txmit( 'X' ) ;
	//		eeprom_write_enable() ;
		
	//		p = Spi_tx_buf ;
	//		*p = 0x20 ;		// Block Erase command
	//		*(p+1) = 0 ;
	//		*(p+2) = 0 << 4 ;		// Erase block 0
	//		*(p+3) = 0 ;		// 3 bytes address
		
	//		x = spi_operation( p, Spi_rx_buf, 4 ) ;
	//		p8hex( x ) ;
	//		crlf() ;
	//	}
	
	//	if ( rxchar == 'C' )
	//	{
	//		register uint8_t *p ;
	//		register uint32_t x ;

	//		txmit( 'C' ) ;

	//		eeprom_write_enable() ;
		
	//		p = Spi_tx_buf ;
	//		*p = 0x39 ;		// Unprotect sector command
	//		*(p+1) = 0 ;
	//		*(p+2) = 0 ;
	//		*(p+3) = 0 ;		// 3 bytes address

	//		x = spi_operation( p, Spi_rx_buf, 4 ) ;

	//		p8hex( x ) ;
	//		crlf() ;
	//	}
	 
	//	if ( rxchar == 'U' )
	//	{
	//		register uint8_t *p ;
	//		register uint32_t x ;

	//		txmit( 'U' ) ;

	//		eeprom_write_enable() ;
	//		p = Spi_tx_buf ;
		
	//		*p = 2 ;		// Write command
	//		*(p+1) = 0 ;
	//		*(p+2) = 0 ;
	//		*(p+3) = 0 ;		// 3 bytes address
	//		*(p+4) = 1 ;
	//		*(p+5) = 2 ;
	//		*(p+18) = 0x0F ;
	//		*(p+19) = 0x10 ;

	//		x = spi_operation( p, Spi_rx_buf, 20 ) ;
	//		p8hex( x ) ;
	//		crlf() ;
	//	}

//		if ( rxchar == '+' )
//		if ( rxchar == '-' )
//		{
//			register uint32_t x ;
		
//			x = PWM->PWM_CH_NUM[0].PWM_CDTY ;				// Duty (current)
//			if ( x < 100 )
//			{
//				x += 1 ;
//				PWM->PWM_CH_NUM[0].PWM_CDTYUPD = x ;	// Duty update
//				g_eeGeneral.bright = x ;
//			}
//		}

//		if ( rxchar == '+' )
//		{
//			register uint32_t x ;
		
//			x = PWM->PWM_CH_NUM[0].PWM_CDTY ;				// Duty (current)
//			if ( x > 0 )
//			{
//				x -= 1 ;
//				PWM->PWM_CH_NUM[0].PWM_CDTYUPD = x ;	// Duty update
//				g_eeGeneral.bright = x ;
//			}
//		}

//		if ( rxchar == '/' )
//		{
//			if ( g_eeGeneral.volume > 0 )
//			{
//				setVolume( --g_eeGeneral.volume ) ;
//			}
//		}

//		if ( rxchar == '*' )
//		{
//			if ( g_eeGeneral.volume < NUM_VOL_LEVELS )
//			{
//				setVolume( ++ g_eeGeneral.volume ) ;
//			}
//		}

//		if ( rxchar == '(' )
//		{
//			set_frequency( 500 ) ;
//		}
	
//		if ( rxchar == '=' )
//		{
//			set_frequency( 1000 ) ;
//		}
	
//		if ( rxchar == ')' )
//		{
//			set_frequency( 3000 ) ;
//		}
	
//		if ( rxchar == ' ' )
//		{
//			playTone( 1000, 50 ) ;
//	//		tone_start( 50 ) ;
//		}

//		if ( rxchar == '!' )
//		{
//			playTone( 1000, 20000 ) ;
//	//		tone_start( 0 ) ;
//		}

//		if ( rxchar == '.' )
//		{
//			playTone( 0, 0 ) ;
//	//		tone_stop() ;
//		}

#endif

		// Display Ram version of EEPROM
	//	if ( ( rxchar >= '0' ) && ( rxchar <= '7' ) )
	//	{
	//		disp_256( ( uint32_t)eeprom + (rxchar - '0') * 256, 16 ) ;
	//	}

	//	if ( rxchar == '@' )
	//	{
	//  	strncpy_P(g_eeGeneral.ownerName,PSTR("MIKE      "), 10) ;
	//		STORE_GENERALVARS ;
	//	}
	}


}

//void disp_mem( register uint32_t address )
//{
//	p8hex( address ) ;
//	txmit('=') ;
//	p8hex( *( (uint32_t *)address ) ) ;
//	crlf() ;
//}

void disp_256( register uint32_t address, register uint32_t lines )
{
	register uint32_t i ;
	register uint32_t j ;
	for ( i = 0 ; i < lines ; i += 1 )
	{
		p8hex( address ) ;
		for ( j = 0 ; j < 16 ; j += 1 )
		{
			txmit(' ') ;
			p2hex( *( (uint8_t *)address++ ) ) ;
		}
		crlf() ;
	}
}


void dispw_256( register uint32_t address, register uint32_t lines )
{
	register uint32_t i ;
	register uint32_t j ;
	for ( i = 0 ; i < lines ; i += 1 )
	{
		p8hex( address ) ;
		for ( j = 0 ; j < 4 ; j += 1 )
		{
			txmit(' ') ;
			p8hex( *( (uint32_t *)address ) ) ;
			address += 4 ;
		}
		crlf() ;
	}
}




#if YMODEM

static int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
	uint16_t rxchar ;

	while ( ( rxchar = rxuart() ) == 0xFFFF )
	{
		CoTickDelay(1) ;					// 2mS
		timeout -= 1 ;
		if ( timeout == 0 )
		{
			break ;			
		}
	}
	if ( rxchar == 0xFFFF )
	{
    return -1;
	}
	*c = rxchar ;
	return 0 ;
}

/*******************************************************************************
* Function Name  : Send_Byte
* Description    : Send a byte
* Input          : - c: Character
* Output         : None
* Return         : 0: Byte sent
*******************************************************************************/
static uint32_t Send_Byte (uint8_t c)
{
  txmit(c) ;
  return 0 ;
}

/*******************************************************************************
* Function Name  : Receive_Packet
* Description    : Receive a packet from sender
* Input 1        : - data
* Input 2        : - length
* Input 3        : - timeout
* Output         : *length:
*                  0: end of transmission
*                  -1: abort by sender
*                  >0: packet length
* Return         : 0: normally return
*                  -1: timeout or packet error
*                  1: abort by user
*******************************************************************************/
static int32_t Receive_Packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
  uint16_t i, packet_size;
  uint8_t c;
  *length = 0;

  if (Receive_Byte(&c, timeout) != 0)
  {
    return -1;
  }
	txmitBt( '1' ) ;
	{
		uint8_t d ;
		
		d = c >> 4 ;
		d &= 0x0F ;
		if ( d > 9 )
		{
			d += 7 ;
		}
		d += '0' ;
		txmitBt( d ) ;
		d = c & 0x0F ;
		if ( d > 9 )
		{
			d += 7 ;
		}
		d += '0' ;
		txmitBt( d ) ;
	}

  switch (c)
  {
    case SOH:
      packet_size = PACKET_SIZE;
			txmitBt( '2' ) ;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
			txmitBt( '3' ) ;
      break;
    case EOT:
			txmitBt( '4' ) ;
      return 0;
    case CA:
      if ((Receive_Byte(&c, timeout) == 0) && (c == CA))
      {
        *length = -1;
				txmitBt( '5' ) ;
        return 0;
      }
      else
      {
				txmitBt( '6' ) ;
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
  }
  *data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
  {
    if (Receive_Byte(data + i, PACKET_TIMEOUT) != 0)
    {
			txmitBt( '7' ) ;
      return -1;
    }
  }
  if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
  {
		txmitBt( '8' ) ;
    return -1;
  }
  *length = packet_size;
	txmitBt( '9' ) ;
  return 0;
}

uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD] ;

uint8_t packet_0[PACKET_1K_SIZE + PACKET_OVERHEAD] ;
uint8_t packet_1[PACKET_1K_SIZE + PACKET_OVERHEAD] ;
uint8_t packet_2[PACKET_1K_SIZE + PACKET_OVERHEAD] ;

int32_t size = 0 ;

/*******************************************************************************
* Function Name  : Ymodem_Receive
* Description    : Receive a file using the ymodem protocol
* Input          : Address of the first byte
* Output         : None
* Return         : The size of the file
*******************************************************************************/

// Send debug using void txmitBt( uint8_t c ) for testing
FIL Tfile ;

int32_t Ymodem_Receive (uint8_t *buf)
{
  uint8_t *file_ptr; //, *buf_ptr;
  int32_t i, packet_length, session_done, file_done, packets_received, errors, session_begin ;
	FRESULT fr ;
	uint32_t written ;
  /* Initialize FlashDestination variable */
//  FlashDestination = ApplicationAddress;

  
	for(;;)
	{
		CoSchedLock() ;
		if ( Voice.VoiceLock == 0 )
		{
			break ;
		}
    CoSchedUnlock() ;
		CoTickDelay(1) ;					// 2mS
	}
	Voice.VoiceLock = 1 ;
  CoSchedUnlock() ;

	size = 0 ;
  file_name[0] = 0 ;

  for (session_done = 0, errors = 0, session_begin = 0; ;)
  {
//		txmitBt( 'A' ) ;
    for (packets_received = 0, file_done = 0 /*, buf_ptr = buf*/; ;)
    {
//			txmitBt( 'B' ) ;
      switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))
      {
        case 0:
          errors = 0;
          switch (packet_length)
          {
              /* Abort by sender */
            case - 1:
//							txmitBt( 'C' ) ;
              Send_Byte(ACK);
              return 0;
              /* End of transmission */
            case 0:
//							txmitBt( 'D' ) ;
							fr = f_close( &Tfile ) ;
              Send_Byte(ACK);
              file_done = 1;
              break;
              /* Normal packet */
            default:
//							txmitBt( 'E' ) ;
              if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))//
              {
//								txmitBt( 'F' ) ;
                Send_Byte(NAK);
              }
              else
              {
                if (packets_received == 0)
                {/* Filename packet */
//									txmitBt( 'G' ) ;
                  if (packet_data[PACKET_HEADER] != 0)
                  {/* Filename packet has valid data */
                    for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
                    {
                      file_name[i++] = *file_ptr++;
                    }
                    file_name[i++] = '\0';
                    for (i = 0, file_ptr += 1; (*file_ptr) && (i < FILE_SIZE_LENGTH);)
                    {
											size *= 10 ;
											size += *file_ptr++ - '0' ;
                    }
											f_unlink ( "Ymodtemp" ) ;					/* Delete any existing temp file */
											fr = f_open( &Tfile, "Ymodtemp", FA_WRITE | FA_CREATE_ALWAYS ) ;

		  // Check fr value here

                    /* Test the size of the image to be sent */
                    /* Image size is greater than Flash size */
//                    if (size > (FLASH_SIZE - 1))
//                    {
                      /* End session */
//                      Send_Byte(CA);
//                      Send_Byte(CA);
//                      return -1;
//                    }
                    /* Erase the needed pages where the user application will be loaded */
                    /* Define the number of page to be erased */
//                    NbrOfPage = FLASH_PagesMask(size);
                    /* Erase the FLASH pages */
//                    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
//                    {
//                      FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
//                    }
                    Send_Byte(ACK);
                    Send_Byte(CRC16);
                  }
                  /* Filename packet is empty, end session */
                  else
                  {
                    Send_Byte(ACK);
                    file_done = 1;
                    session_done = 1;
                    break;
                  }
                }
                /* Data packet */
                else
                {
//									txmitBt( 'H' ) ;
//                  memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
//                  RamSource = (uint32_t)buf;
//                  for (j = 0;(j < packet_length) && (FlashDestination <  ApplicationAddress + size);j += 4)
//                  {
                    /* Program the data received into STM32F10x Flash */
//                    FLASH_ProgramWord(FlashDestination, *(u32*)RamSource);
//                    if (*(u32*)FlashDestination != *(u32*)RamSource)
//                    {
//                      /* End session */
//                      Send_Byte(CA);
//                      Send_Byte(CA);
//                      return -2;
//                    }
//                    FlashDestination += 4;
//                    RamSource += 4;
//                  }
                  Send_Byte(ACK);
                }
								// Temp, copy and save first three buffers
								if ( packets_received < 3 )
								{
									uint8_t *ptr ;
									ptr = packet_0 ;
									if ( packets_received == 1 ) ptr = packet_1 ;
									if ( packets_received == 2 ) ptr = packet_2 ;
									for ( i = 0 ; i < PACKET_1K_SIZE + PACKET_OVERHEAD ; i += 1 )
									{
										*ptr++ = packet_data[i] ;
									}
								}
								if ( packets_received == 1 )
								{
									fr = f_write( &Tfile, &packet_data[3], 128, (UINT *)&written ) ;
//									txmitBt( '^' ) ;
//									if ( fr == FR_OK )
//									{
//										txmitBt( 'o' ) ;
//									}
//									else
//									{
//										txmitBt( 'e' ) ;
//										txmitBt( fr+'@' ) ;
//									}
//									if ( written == 128 )
//									{
//										txmitBt( 'w' ) ;
//									}
//									else
//									{
//										txmitBt( 'x' ) ;
//									}
								}
                packets_received ++;
                session_begin = 1;
              }
							(void)fr ;
          }
          break;
        case 1:
//					txmitBt( 'I' ) ;
          Send_Byte(CA);
          Send_Byte(CA);
          return -3;
        default:
          if (session_begin > 0)
          {
//						txmitBt( 'J' ) ;
            errors ++;
          }
          if (errors > MAX_ERRORS)
          {
//						txmitBt( 'K' ) ;
            Send_Byte(CA);
            Send_Byte(CA);
            return 0;
          }
//					txmitBt( 'L' ) ;
          Send_Byte(CRC16);
          break;
      }
      if (file_done != 0)
      {
//				txmitBt( 'M' ) ;
        break;
      }
    }
    if (session_done != 0)
    {
//			txmitBt( 'N' ) ;
      break;
    }
  }
//	txmitBt( 'P' ) ;


//FRESULT f_unlink (const TCHAR*);					/* Delete an existing file or directory */
//FRESULT f_rename (const TCHAR*, const TCHAR*);		/* Rename/Move a file or directory */

	Voice.VoiceLock = 0 ;
  return (int32_t)size ;
}

#endif

/* Perform a CRC16 computation over `buf'. This method was derived from
 * an algorithm (C) 1986 by Gary S. Brown, and was checked against an
 * implementation (C) 2000 by Compaq Computer Corporation, authored by
 * George France.
 */
//unsigned short crc16_buf(unsigned char *buf, unsigned int length)
//{
//  unsigned short crc = 0;

//  while(length-- > 0)
//    crc = crc16_table[(crc >> 8) & 0xff] ^ (crc << 8) ^ *buf++;

//  return crc;
//}

///* Generate the table of constants used in executing the CRC32 algorithm: */
//int crc16_init(void)
//{
//  int i, j;
//  unsigned short crc;

//  if(crc16_table == NULL){

//    /* This table is currently _not_ freed: */
//    if((crc16_table = 
//	(unsigned short *)mmalloc(CRC_TABLE_SIZE *
//				  sizeof(unsigned short))) == NULL)
//      return -1;

//    for(i = 0; i < CRC_TABLE_SIZE; ++i){

//      crc = i << 8;

//      for(j = 8; j > 0; --j){

//	if(crc & 0x8000)
//	  crc = (crc << 1) ^ CRC16_POLYNOMIAL;
//	else
//	  crc <<= 1;

//      }

//      crc16_table[i] = crc;

//    }

//  }

//  return 0;
//}

#endif

//#ifdef PCBX9D
//void voice_task(void* pdata)
//{
//	uint32_t v_index ;
////	FRESULT fr ;
////	UINT nread ;
//	uint32_t x ;
////	uint32_t w8or16 ;
////	uint32_t mounted = 0 ;
//	uint32_t size ;

//	for(;;)
//	{
//		while ( PlayVoice == 0 )
//		{
//			CoTickDelay(3) ;					// 6mS for now
//		}

//		PlayVoice = 0 ;
//		txmit( '1' ) ;

//		if ( 1 )
//		{
//			uint32_t i ;
//			uint32_t j ;
//			i = 0 ;
//			for ( j = 0 ; j < 400 ; j += 1 )
//			{
//				VoiceBuffer[0].data[j] = Sine_values[i] ;
//				i += 1 ;
//				if ( i >= 100 )
//				{
//					i = 0 ;					
//				}
//			}
//			VoiceBuffer[0].count = 400 ;

//			i = 0 ;
//			for ( j = 0 ; j < 400 ; j += 1 )
//			{
//				VoiceBuffer[1].data[j] = Sine_values[i] ;
//				i += 1 ;
//				if ( i >= 100 )
//				{
//					i = 0 ;					
//				}
//			}
//			VoiceBuffer[1].count = 400 ;

//			i = 0 ;
//			for ( j = 0 ; j < 400 ; j += 1 )
//			{
//				VoiceBuffer[2].data[j] = Sine_values[i] ;
//				i += 1 ;
//				if ( i >= 100 )
//				{
//					i = 0 ;					
//				}
//			}
//			VoiceBuffer[2].count = 400 ;


//				VoiceBuffer[0].frequency = 25000 ;		// sample rate

//				startVoice( 3 ) ;
//		txmit( '2' ) ;
//				for(x = 0, size = 0 ; size < 25 ; size += 1 )
//				{
//	  			while ( ( VoiceBuffer[x].flags & VF_SENT ) == 0 )
//					{
//						CoTickDelay(1) ;					// 2mS for now
//					}
//					VoiceBuffer[x].count = 400 ;
//					VoiceBuffer[x].frequency = 0 ;
//					appendVoice( x ) ;					// index of next buffer

//extern uint8_t VoiceCount ;

//		txmit( VoiceCount + '0' ) ;
//					v_index = x ;		// Last buffer sent
//					x += 1 ;
//					if ( x > 2 )
//					{
//						x = 0 ;							
//					}
//				}
//			// Now wait for last buffer to have been sent
//			x = 100 ;
//		txmit( '4' ) ;
// 			while ( ( VoiceBuffer[v_index].flags & VF_SENT ) == 0 )
//			{
//				CoTickDelay(1) ;					// 2mS for now
//				if ( --x == 0 )
//				{
//					break ;		// Timeout, 200 mS
//				}
//			}
//		}
//		CoTickDelay(1) ;					// 2mS for now
//	} // for(;;)
//}
//#endif


