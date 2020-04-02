/*
 * main.h
 *
 *  Created on: 3 дек. 2018 г.
 *      Author: admin
 */

#ifndef APPLICATION_INC_MAIN_H_
#define APPLICATION_INC_MAIN_H_


#include "stm32f4xx.h"

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_M      4
#define PLL_N      168

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      8

void clockConfig(void);
void initFSMC(void);

void clockConfig(void)
{
	RCC->CR |= (uint32_t)RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){};

    /* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /* HCLK = SYSCLK / 1*/
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK / 1*/
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* PCLK1 = HCLK / 2*/
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* Configure the main PLL */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0){}

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

    /* Select the main PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL){};
}



void initFSMC(void)
{


	RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
	//RCC->AHB3RSTR |= RCC_AHB3RSTR_FSMCRST;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIODEN;

	//GPIOE FSMC
	GPIOE->MODER |= GPIO_MODER_MODE15_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE13_1 | GPIO_MODER_MODE12_1 |
			GPIO_MODER_MODE11_1 | GPIO_MODER_MODE10_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1 | GPIO_MODER_MODE7_1; //Alternate function mode
	//GPIOE->OSPEEDR = 0xFFFFC000;
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED15 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED13 | GPIO_OSPEEDR_OSPEED12 |
			GPIO_OSPEEDR_OSPEED11 | GPIO_OSPEEDR_OSPEED10 | GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED7;
	//GPIOE->PUPDR = 0x00000000;
	GPIOE->AFR[1] = 0xCCCCCCCC;
	GPIOE->AFR[0] = 0xC0000000;

	//GPIOD FSMC
	GPIOD->MODER |= GPIO_MODER_MODE15_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE13_1 | GPIO_MODER_MODE10_1 |
			GPIO_MODER_MODE9_1 | GPIO_MODER_MODE8_1 | GPIO_MODER_MODE7_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE1_1 | GPIO_MODER_MODE0_1; //Alternate function mode
	//GPIOD->OSPEEDR = 0xFC3FCF0F;
	GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED15 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED13 | GPIO_OSPEEDR_OSPEED10 |
			GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED0;
	//GPIOD->PUPDR = 0x00000000;
	GPIOD->AFR[1] = 0xCCC00CCC;
	GPIOD->AFR[0] = 0xC0CC00CC;

	//NOR/PSRAM chip-select control register(BCR)
	FSMC_Bank1->BTCR[0] |= FSMC_BCR1_MBKEN | FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN;
	/*
	FSMC_Bank1->BTCR[0] |= FSMC_BCR1_MBKEN; //Memory bank enable bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_MUXEN; //Address/data multiplexing enable bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_MTYP_1 | FSMC_BCR1_MTYP_0; //MTYP[1:0] bits (Memory type)
	FSMC_Bank1->BTCR[0] |= FSMC_BCR1_MWID_0; //MWID[1:0] bits (Memory data bus width)
	FSMC_Bank1->BTCR[0] |= FSMC_BCR1_FACCEN; //Flash access enable
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_BURSTEN; //Burst enable bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_WAITPOL; //Wait signal polarity bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_WRAPMOD; //Wrapped burst mode support
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_WAITCFG; //Wait timing configuration
	FSMC_Bank1->BTCR[0] |= FSMC_BCR1_WREN; //Write enable bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_WAITEN; //Wait enable bit
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_EXTMOD; //Extended mode enable
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_ASYNCWAIT; //Asynchronous wait
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_CPSIZE_2 | FSMC_BCR1_CPSIZE_1 | FSMC_BCR1_CPSIZE_0; //CRAM page size
	FSMC_Bank1->BTCR[0] &=~ FSMC_BCR1_CBURSTRW; //Write burst enable
	*/


	//chip-select timing register(BTR)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_ADDSET_1 | FSMC_BTR1_DATAST_1;
	/*
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_ADDSET_3 | FSMC_BTR1_ADDSET_2 | FSMC_BTR1_ADDSET_1 | FSMC_BTR1_ADDSET_0; //ADDSET[3:0] bits (Address setup phase duration)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_ADDHLD_3 | FSMC_BTR1_ADDHLD_2 | FSMC_BTR1_ADDHLD_1 | FSMC_BTR1_ADDHLD_0; //ADDHLD[3:0] bits (Address-hold phase duration)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_DATAST_7 | FSMC_BTR1_DATAST_6 | FSMC_BTR1_DATAST_5 | FSMC_BTR1_DATAST_4 |
			FSMC_BTR1_DATAST_3 | FSMC_BTR1_DATAST_2 | FSMC_BTR1_DATAST_1 | FSMC_BTR1_DATAST_0; //DATAST [7:0] bits (Data-phase duration)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_BUSTURN_3 | FSMC_BTR1_BUSTURN_2 | FSMC_BTR1_BUSTURN_1 | FSMC_BTR1_BUSTURN_0; //BUSTURN[3:0] bits (Bus turnaround phase duration)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_CLKDIV_3 | FSMC_BTR1_CLKDIV_2 | FSMC_BTR1_CLKDIV_1 | FSMC_BTR1_CLKDIV_0; //CLKDIV[3:0] bits (Clock divide ratio)
	FSMC_Bank1->BTCR[1] |= FSMC_BTR1_DATLAT_3 | FSMC_BTR1_DATLAT_2 | FSMC_BTR1_DATLAT_1 | FSMC_BTR1_DATLAT_0; //DATLA[3:0] bits (Data latency)
	FSMC_Bank1->BTCR[1] &=~ FSMC_BTR1_ACCMOD_1 | FSMC_BTR1_ACCMOD_0; //ACCMOD[1:0] bits (Access mode)
	*/


	/** FSMC GPIO Configuration
	PE7   ------> FSMC_D4
	PE8   ------> FSMC_D5
	PE9   ------> FSMC_D6
	PE10   ------> FSMC_D7
	PE11   ------> FSMC_D8
	PE12   ------> FSMC_D9
	PE13   ------> FSMC_D10
	PE14   ------> FSMC_D11
	PE15   ------> FSMC_D12
	PD8   ------> FSMC_D13
	PD9   ------> FSMC_D14
	PD10   ------> FSMC_D15
	PD13   ------> FSMC_A18
	PD14   ------> FSMC_D0
	PD15   ------> FSMC_D1
	PD0   ------> FSMC_D2
	PD1   ------> FSMC_D3
	PD4   ------> FSMC_NOE
	PD5   ------> FSMC_NWE
	PD7   ------> FSMC_NE1
	*/


}

#endif /* APPLICATION_INC_MAIN_H_ */
