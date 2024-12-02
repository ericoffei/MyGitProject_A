/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "adcf4xx.h"

//========================================================================================================================================
//Initialize the DMA
/************** STEPS TO FOLLOW *****************
 1. Enable DMA clock
 2. Configure the Configuration Register
************************************************/

/* DMA_Init() */
void dman_init(DMAMN *siop)
{
 __IO uint32_t tmpreg;
   // Enable the DMA2 Clock
    *siop -> ahbenr |= siop -> ahbenroffset;   // DMA2EN
    tmpreg = *siop -> ahbenr & siop -> ahbenroffset;
    UNUSED(tmpreg);    /* To avoid gcc/g++ warnings */

	// Configure DMA Stream
	siop -> stream->CR &= ~DMA_SxCR_EN; // Disable DMA
    // Reset DMA2 Stream 0 and wait until the stream is disabled
    DMA2_Stream0->CR = 0x00;
    while(DMA2_Stream0->CR & DMA_SxCR_EN) {;}


   // Select Circular mode
   if (siop -> mode == 0)
       siop -> stream-> CR &= ~(DMA_SxCR_CIRC);  // CIRC disabled
   else
       siop -> stream-> CR |=  (DMA_SxCR_CIRC);  // CIRC enabled


   // Enable Memory Address Increment
   if (siop -> minc == 0)
       siop -> stream-> CR &= ~(DMA_SxCR_MINC);  // MINC = 0; no increment
   else
       siop -> stream-> CR |=  (DMA_SxCR_MINC);  // MINC = 1; MEM address increment

   // Set the size for memory data: 0 (8-bit), 1 (16-bit), 2 (32-bit)
   siop -> stream->CR &= ~(3 << DMA_SxCR_MSIZE_Pos);  // 8-bit
   switch (siop ->msize)
   {
      case  0:
          break;
      case 1:
    	  siop -> stream->CR |= (1 << DMA_SxCR_MSIZE_Pos);
          break;
      case 2:
    	  siop -> stream->CR |= (2 << DMA_SxCR_MSIZE_Pos);
          break;
      default: ;
   }

   // Enable Peripheral Address Increment
   if (siop -> pinc == 0)
       siop -> stream-> CR &= ~ (1 << DMA_SxCR_PINC_Pos);  // no increment
   else
       siop -> stream-> CR |=  ( 1 << DMA_SxCR_PINC_Pos);  // increment

   // Set the size for peripheral data: 0 (8-bit), 1 (16-bit), 2 (32-bit)
   siop -> stream->CR &= ~(3 << DMA_SxCR_PSIZE_Pos);  // 8-bit
   switch (siop ->psize)
   {
      case  0:
          break;
      case 1:
    	  siop -> stream->CR |= (1 << DMA_SxCR_PSIZE_Pos);
          break;
      case 2:
    	  siop -> stream->CR |= (2 << DMA_SxCR_PSIZE_Pos);
          break;
      default: ;
   }

   // Select channel for the stream
   siop -> stream-> CR &= ~( 7 << DMA_SxCR_CHSEL_Pos) ;  // Channel 0 selected
   if ((siop ->cha >= 1) && (siop ->cha <= 7) )
   {
	   siop -> stream-> CR |= ( siop ->cha << DMA_SxCR_CHSEL_Pos);  // Channel 1..7 selected	     }
   }
}

/* DMA_Init() */
void dman_init2(DMAMN *siop)
{
	uint32_t tmp = 0U;
 __IO uint32_t tmpreg;
   // Enable the DMA2 Clock
    *siop -> ahbenr |= siop -> ahbenroffset;   // DMA2EN
    tmpreg = *siop -> ahbenr & siop -> ahbenroffset;
    UNUSED(tmpreg);    /* To avoid gcc/g++ warnings */

	// Configure DMA Stream
	siop -> stream->CR &= ~DMA_SxCR_EN; // Disable DMA
    // Reset DMA2 Stream 0 and wait until the stream is disabled
    while(siop -> stream->CR & DMA_SxCR_EN) {;}

    /* Get the CR register value */
    tmp = siop -> stream->CR;

    /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
    tmp &= ((uint32_t)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST | \
                        DMA_SxCR_PL    | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  | \
                        DMA_SxCR_MINC  | DMA_SxCR_PINC   | DMA_SxCR_CIRC   | \
 						DMA_SxCR_TCIE  | DMA_SxCR_HTIE   | DMA_SxCR_TEIE   | DMA_SxCR_DMEIE | \
						DMA_SxCR_DIR   | DMA_SxCR_CT     | DMA_SxCR_DBM));

   // Select Circular mode
   if (siop -> mode)
   {
	   tmp |=  (DMA_SxCR_CIRC);  // CIRC enabled
   }

   // Enable Memory Address Increment
   if (siop -> minc)
   {
	   tmp |=  (DMA_SxCR_MINC);  // MINC = 1; MEM address increment
   }

   // Set the size for memory data: 0 (8-bit), 1 (16-bit), 2 (32-bit)
   switch (siop ->msize)
   {
      case  0:           // 0 (8-bit)
          break;
      case 1:            // 1 (16-bit)
    	  tmp |= DMA_SxCR_MSIZE_0;
          break;
      case 2:            //  (32-bit)
    	  tmp |= DMA_SxCR_MSIZE_1;
          break;
      default: ;
   }

   // Enable Peripheral Address Increment
   if (siop -> pinc)
   {
	   tmp |=  DMA_SxCR_PINC;  // increment
   }

   // Set the size for peripheral data: 0 (8-bit), 1 (16-bit), 2 (32-bit)
    switch (siop ->psize)
   {
      case  0:           // 0 (8-bit)
          break;
      case 1:            // 1 (16-bit)
    	  tmp |= DMA_SxCR_PSIZE_0;
          break;
      case 2:            //  (32-bit)
    	  tmp |= DMA_SxCR_PSIZE_1;
          break;
      default: ;
   }

   // Select channel for the stream
   if ((siop ->cha >= 1) && (siop ->cha <= 7) )
   {
    	tmp |= (siop ->cha << DMA_SxCR_CHSEL_Pos);  // Channel 1..7 selected	     }
   }

   siop -> stream->CR = tmp;

   // Enable DMA interrupts.
   if (siop ->ien)
   {
       NVIC_SetPriority(siop ->irqn , siop ->prio); // DMA IRQ.
       NVIC_EnableIRQ(siop ->irqn);
       //DMA interrupt requests
       siop -> stream->CR |= DMA_SxCR_TCIE;   // Transfer complete interrupt enable
       //siop -> stream->CR |= DMA_SxCR_HTIE;   // Half transfer interrupt enable
       siop -> stream->CR |= DMA_SxCR_TEIE;   // Transfer error interrupt enable
   }

}

// CONFIGURE THE DMA
/************** STEPS TO FOLLOW *****************
1. Set the Data Size in the NDTR Register
2. Set the Peripheral Address and the Memory Address
3. Enable the DMA Stream
************************************************/

void dman_config(DMAMN *siop, uint32_t *src, uint32_t *dst, uint32_t size )
{
	if (!siop)
		return;

	// Configure DMA Stream
	if ((siop -> stream->CR & DMA_SxCR_EN) == DMA_SxCR_EN ) // Disable DMA
	{
		siop -> stream->CR &= ~DMA_SxCR_EN;
		while(siop -> stream->CR & DMA_SxCR_EN); // Wait for DMA to be disabled
	}

   //  Set the Data Size in the NDTR Register
	siop -> stream->NDTR = size;   // Set the size of the transfer

   // Set the Peripheral Address and the Memory Address
	   //  0) Peripheral-to memory,
	   //  1 (memory-Peripheral),
	   //  2 (memory-memory)
   siop -> stream->CR &= ~(DMA_SxCR_DIR);  // Peripheral to memory
   switch (siop ->dir)
   {
	  case  0:                                          // Peripheral to memory
	   	 siop -> stream->PAR =  (uint32_t)src;  // Source address is peripheral address
    	 siop -> stream->M0AR = (uint32_t)dst;  // Destination Address is memory address
    	 break;
	  case 1:
    	 siop -> stream->CR |=  DMA_SxCR_DIR_0;      // Memory to peripheral
         siop -> stream->M0AR = (uint32_t)src;  // Source Address is memory address
	     siop -> stream->PAR =  (uint32_t)dst;  // Destination Address is peripheral address
         break;
	  case  2:                                    // Memory to memory
    	 siop -> stream->CR |=  DMA_SxCR_DIR_1;
	   	 siop -> stream->PAR =  (uint32_t)src;  // Source address is peripheral address
    	 siop -> stream->M0AR = (uint32_t)dst;  // Destination Address is memory address
    	 break;
      default: ;
   }


   //  Enable DMA transfer
   siop -> stream->CR |= DMA_SxCR_EN;  // EN =1; Start DMA transfer
}

///////////////////////////////////////////////////////////////////////////////////////////////
void mem_to_mem(DMAMN *siop, uint32_t *src, uint32_t *dst, uint32_t size )
{
	uint32_t tmp = siop -> dir;

	// clear all interrupt flags in the DMA_LIFCR/DMA_HIFCR
	if (siop ->stream >= DMA1_Stream0 && siop ->stream <= DMA1_Stream7)
	{
        siop ->dma -> LIFCR |= siop ->ifcrmsk;
	}
	else if (siop ->stream >= DMA2_Stream0 && siop ->stream <= DMA2_Stream7)
	{
	    siop ->dma -> HIFCR |= siop ->ifcrmsk;
	}

	siop -> dir = 2; // memory-to-memory
	dman_config(siop, src, dst, size);
	siop -> dir = tmp; // restore
}

void periph_to_mem(DMAMN *siop, uint32_t *src, uint32_t *dst, uint32_t size )
{
	uint32_t tmp = siop -> dir;

	// clear all interrupt flags in the DMA_LIFCR/DMA_HIFCR
	if (siop ->stream >= DMA1_Stream0 && siop ->stream <= DMA1_Stream7)
	{
        siop ->dma -> LIFCR |= siop ->ifcrmsk;
	}
	else if (siop ->stream >= DMA2_Stream0 && siop ->stream <= DMA2_Stream7)
	{
	    siop ->dma -> HIFCR |= siop ->ifcrmsk;
	}

	siop -> dir = 0; //  // Peripheral to memory
	dman_config(siop, src, dst, size);
	siop -> dir = tmp; // restore
}

void mem_to_periph(DMAMN *siop, uint32_t *src, uint32_t *dst, uint32_t size )
{
	uint32_t tmp = siop -> dir;

	// clear all interrupt flags in the DMA_LIFCR/DMA_HIFCR
	if (siop ->stream >= DMA1_Stream0 && siop ->stream <= DMA1_Stream7)
	{
        siop ->dma -> LIFCR |= siop ->ifcrmsk;
	}
	else if (siop ->stream >= DMA2_Stream0 && siop ->stream <= DMA2_Stream7)
	{
	    siop ->dma -> HIFCR |= siop ->ifcrmsk;
	}

	siop -> dir = 1;  // Memory to peripheral
	dman_config(siop, src, dst, size);
	siop -> dir = tmp; // restore
}

