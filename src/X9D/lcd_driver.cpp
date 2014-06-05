/**
  ******************************************************************************
  * @file    Project/lcd/lcd.c 
  * @author  X9D Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides LCD Init and botom drivers.
  * *
  ******************************************************************************
*/

#include "../ersky9x.h"

#include "x9d\stm32f2xx.h"
#include "x9d\stm32f2xx_gpio.h"
#include "x9d\stm32f2xx_rcc.h"
#include "x9d\hal.h"
#include "x9d\aspi.h"

#include "timers.h"
#include "lcd.h"
#include "logicio.h"
#include "myeeprom.h"

//#define	WriteData(x)	 AspiData(x)
//#define	WriteCommand(x)	 AspiCmd(x)
#define CONTRAST_OFS 12

#define __no_operation     __NOP


extern uint8_t DisplayBuf[] ;

void Set_Address(uint8_t x, uint8_t y)
{
  AspiCmd(x&0x0F);	//Set Column Address LSB CA[3:0]
  AspiCmd((x>>4)|0x10);	//Set Column Address MSB CA[7:4]
  
  AspiCmd((y&0x0F)|0x60);	//Set Row Address LSB RA [3:0]
  AspiCmd(((y>>4)&0x0F)|0x70);    //Set Row Address MSB RA [7:4]
}

#ifdef REVPLUS
void xrefreshDisplay()
{
  for (uint32_t y=0; y<DISPLAY_H; y++)
	{
    uint8_t *p = &DisplayBuf[(y>>3)*DISPLAY_W];
    uint8_t mask = (1 << (y%8));
		
		GPIO_TypeDef *gpiod = GPIOC ;
    
		Set_Address(0, y);
    AspiCmd(0xAF);
    
		gpiod->BSRRL = PIN_LCD_A0 ;			// A0 high
    GPIOA->BSRRH = PIN_LCD_NCS ;		// CS low

		for (uint32_t x=0; x<DISPLAY_W; x+=2)
		{
      uint32_t data ;
			data = 0 ;
			if ( p[x] & mask )
			{
				data = 0xF0 ;
			}
			if (p[x+1] & mask )
			{
				data += 0x0F ;
			}	
			while ( SPI3->SR & SPI_SR_TXE )
			{
				// wait
			}
			SPI3->DR = data ;
		}
		while ( SPI3->SR & SPI_SR_TXE )
		{
			// wait
		}
    GPIOA->BSRRL = PIN_LCD_NCS ;		// CS high
		gpiod->BSRRL = PIN_LCD_A0 ;
  }
	
}
#endif

void refreshDisplay()
{  
  for (uint32_t y=0; y<DISPLAY_H; y++)
	{
    uint8_t *p = &DisplayBuf[(y>>3)*DISPLAY_W];
    uint8_t mask = (1 << (y%8));
		
#ifdef REVPLUS
		GPIO_TypeDef *gpiod = GPIOC ;
#else
		GPIO_TypeDef *gpiod = GPIOD ;
#endif
		uint32_t *bsrr = (uint32_t *)&gpiod->BSRRL ;

		Set_Address(0, y);
    AspiCmd(0xAF);
    
		gpiod->BSRRL = PIN_LCD_CLK ;		// Clock high
		gpiod->BSRRL = PIN_LCD_A0 ;			// A0 high
#ifdef REVPLUS
    GPIOA->BSRRH = PIN_LCD_NCS ;		// CS low
#else
    gpiod->BSRRH = PIN_LCD_NCS ;		// CS low
#endif

		for (uint32_t x=0; x<DISPLAY_W; x+=2)
		{
      uint32_t data ;
			data = 0 ;
			if ( p[x] & mask )
			{
				data = 0xF0 ;
			}
			if (p[x+1] & mask )
			{
				data += 0x0F ;
			}	
			
        if(data&0x80)
        {
					gpiod->BSRRL = PIN_LCD_MOSI ;
        }
				else
				{
					gpiod->BSRRH = PIN_LCD_MOSI ;
				}
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x40)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x20)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x10)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x08)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x04)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x02)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low

        if(data&0x01)
        {
					gpiod->BSRRL = PIN_LCD_MOSI | PIN_LCD_CLK ;
        }
				else
				{
					*bsrr = (PIN_LCD_MOSI<<16) | PIN_LCD_CLK ;
				}
				__no_operation() ;
				gpiod->BSRRH = PIN_LCD_CLK ;		// Clock low
				__no_operation() ;
				gpiod->BSRRL = PIN_LCD_CLK ;		// Clock high

		}
#ifdef REVPLUS
    GPIOA->BSRRL = PIN_LCD_NCS ;		// CS high
#else
    gpiod->BSRRL = PIN_LCD_NCS ;		// CS high
#endif
		gpiod->BSRRL = PIN_LCD_A0 ;
    AspiData(0);
  }
}

uint16_t BacklightBrightness ;

#ifdef REVPLUS
uint16_t BacklightBrightness_white ;

void backlight_w_on()
{
	TIM4->CCR4 = 100 - BacklightBrightness_white ;
}

void backlight_w_off()
{
	TIM4->CCR4 = 0 ;
}

void backlight_on()
{
	TIM4->CCR2 = 100 - BacklightBrightness ;
}

void backlight_off()
{
	TIM4->CCR2 = 0 ;
}

void backlight_set( uint16_t brightness, uint16_t w_or_b )
{
	if ( w_or_b )
	{ // blue
		BacklightBrightness = brightness ;
		TIM4->CCR2 = 100 - BacklightBrightness ;
	}
	else
	{ // white
		BacklightBrightness_white = brightness ;
		TIM4->CCR4 = 100 - BacklightBrightness_white ;
	}
}

/**Init the Backlight GPIO */
static void LCD_BL_Config()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBL, ENABLE);
  
	GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_BL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBL, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BL ,GPIO_AF_TIM4);

  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_BLW;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBLW, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOBLW, GPIO_PinSource_BLW ,GPIO_AF_TIM4);

  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN ;    // Enable clock
	TIM4->ARR = 100 ;
	TIM4->PSC = (Peri1_frequency*Timer_mult2) / 10000 - 1 ;		// 100uS from 30MHz
	TIM4->CCMR1 = TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 ;	// PWM
	TIM4->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 ;	// PWM
	TIM4->CCER = TIM_CCER_CC4E | TIM_CCER_CC2E ;
	
	BacklightBrightness = 40 ;
	TIM4->CCR2 = BacklightBrightness ;
	TIM4->CCR4 = BacklightBrightness_white ;
	TIM4->EGR = 0 ;
	TIM4->CR1 = TIM_CR1_CEN ;				// Counter enable
}

// Init the Haptic
void initHaptic()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOHAPTIC, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_HAPTIC;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOHAPTIC, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOHAPTIC, GPIO_PinSource_HAPTIC ,GPIO_AF_TIM10);

	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN ;		// Enable clock
	TIM10->ARR = 100 ;
	TIM10->PSC = (Peri2_frequency*Timer_mult2) / 10000 - 1 ;		// 100uS from 30MHz
	TIM10->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;	// PWM
	TIM10->CCER = TIM_CCER_CC1E ;
	
	TIM10->CCR1 = 0 ;
	TIM10->EGR = 0 ;
	TIM10->CR1 = TIM_CR1_CEN ;				// Counter enable
}

void hapticOff()
{
	TIM10->CCR1 = 0 ;
}

// pwmPercent 0-100
void hapticOn( uint32_t pwmPercent )
{
	if ( pwmPercent > 100 )
	{
		pwmPercent = 100 ;		
	}
	TIM10->CCR1 = pwmPercent ;
}

#else

void backlight_on()
{
	TIM10->CCR1 = 100 - BacklightBrightness ;
}

void backlight_off()
{
	TIM10->CCR1 = 0 ;
}

void backlight_set( uint16_t brightness )
{
	BacklightBrightness = brightness ;
	TIM10->CCR1 = 100 - BacklightBrightness ;
}


/**Init the Backlight GPIO */
static void LCD_BL_Config()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOBL, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_BL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOBL, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOBL, GPIO_PinSource_BL ,GPIO_AF_TIM10);

	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN ;		// Enable clock
	TIM10->ARR = 100 ;
	TIM10->PSC = (Peri2_frequency*Timer_mult2) / 10000 - 1 ;		// 100uS from 30MHz
	TIM10->CCMR1 = 0x60 ;	// PWM
	TIM10->CCER = 1 ;	
	
	BacklightBrightness = 80 ;
	TIM10->CCR1 = BacklightBrightness ;
	TIM10->EGR = 0 ;
	TIM10->CR1 = 1 ;
}
#endif

/** Init the anolog spi gpio
*/
static void LCD_Hardware_Init()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LCD, ENABLE);
#ifdef REVPLUS
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LCD_RST, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LCD_NCS, ENABLE);
#endif  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*!< Configure lcd CLK\ MOSI\ A0pin in output pushpull mode *************/
  GPIO_InitStructure.GPIO_Pin =PIN_LCD_MOSI | PIN_LCD_CLK | PIN_LCD_A0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_LCD, &GPIO_InitStructure);
  
  /*!< Configure lcd NCS pin in output pushpull mode ,PULLUP *************/
#ifdef REVPLUS
	GPIO_InitStructure.GPIO_Pin = PIN_LCD_NCS ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(GPIO_LCD_NCS, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_LCD_RST ;
  GPIO_Init(GPIO_LCD_RST, &GPIO_InitStructure);

#else  
	GPIO_InitStructure.GPIO_Pin = PIN_LCD_NCS | PIN_LCD_RST ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(GPIO_LCD, &GPIO_InitStructure);
#endif  
}

static void LCD_Init()
{
  LCD_BL_Config() ;
  /*Hardware Reset need delay*/
  /*LCD_RST_LOW();
    Delay(50);    
    LCD_RST_HIGH();*/
    
  AspiCmd(0x25);   //Temperature compensation curve definition: 0x25 = -0.05%/oC
  AspiCmd(0x2b);   //Panel loading set ,Internal VLCD.
  AspiCmd(0xEA);	//set bias=1/10 :Command table NO.27
  AspiCmd(0x81);	//Set Vop
  AspiCmd(25+CONTRAST_OFS);		//0--255
  AspiCmd(0xA6);	//inverse display off
  AspiCmd(0xD1);	//SET RGB:Command table NO.21 .SET RGB or BGR.  D1=RGB
  AspiCmd(0xD5);	//set color mode 4K and 12bits  :Command table NO.22
  AspiCmd(0xA0);	//line rates,25.2 Klps
  AspiCmd(0xC8);	//SET N-LINE INVERSION
  AspiCmd(0x1D);	//Disable NIV
  AspiCmd(0xF1);	//Set CEN
  AspiCmd(0x3F);	// 1/64DUTY
  AspiCmd(0x84);	//Disable Partial Display
  AspiCmd(0xC4);	//MY=1,MX=0
  AspiCmd(0x89);	//WA=1,column (CA) increment (+1) first until CA reaches CA boundary, then RA will increment by (+1).

  AspiCmd(0xF8);	//Set Window Program Enable  ,inside modle
  AspiCmd(0xF4);   //starting column address of RAM program window.
  AspiCmd(0x00);
  AspiCmd(0xF5);   //starting row address of RAM program window.
  AspiCmd(0x60);
  AspiCmd(0xF6);   //ending column address of RAM program window.
  AspiCmd(0x47);
  AspiCmd(0xF7);   //ending row address of RAM program window.
  AspiCmd(0x9F);

  AspiCmd(0xAF);	//dc2=1,IC into exit SLEEP MODE,	 dc3=1  gray=ON 开灰阶	,dc4=1  Green Enhanc mode disabled	  绿色增强模式关
	
}

static void Delay(volatile unsigned int ms)
{
  volatile u8 i;
  while(ms != 0)
  {
    for(i=0;i<250;i++) {}
    for(i=0;i<75;i++) {}
    ms--;
  }
}

void lcd_init()
{
	GPIO_TypeDef *gpiod = GPIOD ;

  LCD_BL_Config();
  LCD_Hardware_Init();
  
	gpiod->BSRRL = PIN_LCD_RST ;		// RST high
  Delay(5);

  gpiod->BSRRH = PIN_LCD_RST ;		// RST low
  Delay(120); //11ms

	gpiod->BSRRL = PIN_LCD_RST ;		// RST high
  Delay(2500);
 
  AspiCmd(0xE2);
  Delay(2500);

  LCD_Init();
  Delay(120);
  AspiCmd(0xAF);	//dc2=1, IC into exit SLEEP MODE, dc3=1 gray=ON, dc4=1 Green Enhanc mode disabled
}

void lcdSetRefVolt(uint8_t val)
{
  AspiCmd(0x81);	//Set Vop
  AspiCmd(val+CONTRAST_OFS);		//0--255
}
