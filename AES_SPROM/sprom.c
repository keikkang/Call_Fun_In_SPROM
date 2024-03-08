/**************************************************************************//**
 * @file     sprom.c
 * @version  V1.00
 * @brief    Collect of sub-routines running on SPROM.
 *
 *
 * @copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#include "NuMicro.h"

__attribute__((at(0x200FFF))) static const uint8_t Security_Mode  = 0x33;

static const uint32_t au32MyAESKey[8] =
{
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f
};
uint32_t au32MyAESIV[4] =
{
    0x00000000, 0x00000000, 0x00000000, 0x00000000
};
static volatile int  g_AES_done;

void CRYPTO_IRQHandler()
{
    if (AES_GET_INT_FLAG(CRPT))
    {
        g_AES_done = 1;
        AES_CLR_INT_FLAG(CRPT);
    }
}

/*
 *  Put a character to UART0 transmitter
 */
void sprom_putc(int ch)
{
    while (UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk) ;
    UART0->DAT = ch;
    if (ch == '\n')
    {
        while(UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
        UART0->DAT = '\r';
    }
}

/*
 *  Poll until received a character from UART0 receiver
 */
char sprom_getc(void)
{
    while(1)
    {
        if ((UART0->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0)
        {
            return (UART0->DAT);
        }
    }
}

/*
 *  print out a string
 */
void sprom_put_string(char *str)
{
    while (*str)
    {
        sprom_putc(*str++);
    }
}

void sprom_aes_encrypt(uint8_t *inp, int length, uint8_t *outp)
{
    /*---------------------------------------
     *  AES-128 ECB mode encrypt
     *---------------------------------------*/
    AES_Open(CRPT, 0, 1, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    AES_SetKey(CRPT, 0, (uint32_t*)au32MyAESKey, AES_KEY_SIZE_128);
    AES_SetInitVect(CRPT, 0, au32MyAESIV);
    AES_SetDMATransfer(CRPT, 0, (uint32_t)inp, (uint32_t)outp, length);
    g_AES_done = 0;
    AES_Start(CRPT, 0, CRYPTO_DMA_ONE_SHOT);
    while (!g_AES_done);

    sprom_put_string("AES encrypt done.\n");
}

void sprom_aes_decrypt(uint8_t *inp, int length, uint8_t *outp)
{
    /*---------------------------------------
     *  AES-128 ECB mode decrypt
     *---------------------------------------*/
    AES_Open(CRPT, 0, 0, AES_MODE_ECB, AES_KEY_SIZE_128, AES_IN_OUT_SWAP);
    AES_SetKey(CRPT, 0, (uint32_t *)au32MyAESKey, AES_KEY_SIZE_128);
    AES_SetInitVect(CRPT, 0, au32MyAESIV);
    AES_SetDMATransfer(CRPT, 0, (uint32_t)inp, (uint32_t)outp, length);
    g_AES_done = 0;
    AES_Start(CRPT, 0, CRYPTO_DMA_ONE_SHOT);
    while (!g_AES_done);

    sprom_put_string("AES decrypt done.\n");
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
