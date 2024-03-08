/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * @brief    This sample shows how to make an application running on APROM
 *           but with a sub-routine on SPROM, which can be secured.
 *
 * @copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"


void sprom_aes_encrypt(uint8_t *inp, int length, uint8_t *outp);
void sprom_aes_decrypt(uint8_t *inp, int length, uint8_t *outp);

uint8_t au8InputData[] __attribute__((aligned (4))) =
{
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};
uint8_t au8OutputData[256] __attribute__((aligned (4)));

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable HXT clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Wait for HXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_CLKDIV0_HCLK(1));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_PCLK0DIV2 | CLK_PCLKDIV_PCLK1DIV2;

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source as HXT and UART module clock divider as 1 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_CLKDIV0_UART0(1));

    /* Enable CRYPTO module clock */
    CLK_EnableModuleClock(CRPT_MODULE);
    
    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

void UART0_Init(void)
{
    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

void  dump_buff_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02x ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}

int main()
{
    char     chr;                      /* user input character                            */

    SYS_Init();                        /* Init System, IP clock and multi-function I/O    */

    UART0_Init();                      /* Initialize UART0                                */

    SYS_UnlockReg();                   /* Unlock protected registers                      */

    FMC_Open();                        /* Enable FMC ISP function                         */

    SYS_UnlockReg();                   /* Unlock protected registers                      */
    NVIC_EnableIRQ(CRPT_IRQn);
    AES_ENABLE_INT(CRPT);
    
    while (1)
    {
        printf("\n");
        printf("+--------------------------------------+\n");
        printf("| M487 AES in SPROM Test Code          |\n");
        printf("| [1] AES encrypt                      |\n");
        printf("| [2] AES decrypt                      |\n");
        printf("+--------------------------------------+\n");

       if (FMC->ISPSTS & FMC_ISPSTS_SCODE_Msk)
            printf("ISPSTS=0x%x, SPROM is secured.\n", FMC->ISPSTS);
       else
            printf("ISPSTS=0x%x, SPROM is not secured.\n", FMC->ISPSTS);
			 
			 //if conportlab want lock OR unlock sprom do this function. 
			 

        chr = getchar();

        switch (chr)
        {
        case '1':
						printf("The Function Address of sprom_aes_encrypt Function is: %p\n", (void*)&sprom_aes_encrypt);
            sprom_aes_encrypt(au8InputData, sizeof(au8InputData), au8OutputData);
            dump_buff_hex(au8OutputData, sizeof(au8InputData));
            break;
        
        case '2':
						printf("The Function Address of sprom_aes_decrypt Function is: %p\n", (void*)&sprom_aes_decrypt);
            sprom_aes_decrypt(au8OutputData, sizeof(au8InputData), au8InputData);
            dump_buff_hex(au8InputData, sizeof(au8InputData));
            break;           

        }
    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
