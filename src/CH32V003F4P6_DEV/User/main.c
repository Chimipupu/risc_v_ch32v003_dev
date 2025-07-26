/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                     PD6 -- Tx
 *
 */

#include "debug.h"
#include <string.h>
#include <stdint.h>

// UARTデータ構造体
typedef struct {
    uint8_t rx_cnt;
    uint8_t tx_cnt;
    uint8_t *p_rx_buf;
    uint8_t *p_tx_buf;
    uint8_t rx_buf_size;
    uint8_t tx_buf_size;
    uint8_t rx_idx;
    uint8_t tx_idx;
} uart_data_t;

static uart_data_t s_uart_data;

uint8_t g_uart_rx_buf[256]; // UART受信リングバッファ
uint8_t g_uart_tx_buf[256]; // UART送信リングバッファ

static void hw_uart_init(void);

/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief UART初期化
 * 
 */
static void hw_uart_init(void)
{
    memset(&g_uart_rx_buf, 0x00, sizeof(g_uart_rx_buf));
    memset(&g_uart_tx_buf, 0x00, sizeof(g_uart_tx_buf));
    s_uart_data.p_rx_buf = &g_uart_rx_buf[0];
    s_uart_data.p_tx_buf = &g_uart_tx_buf[0];
    s_uart_data.rx_buf_size = (uint8_t)sizeof(g_uart_rx_buf);
    s_uart_data.tx_buf_size = (uint8_t)sizeof(g_uart_tx_buf);
    s_uart_data.rx_cnt = 0;
    s_uart_data.tx_cnt = 0;
    s_uart_data.rx_idx = 0;
    s_uart_data.tx_idx = 0;

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    USARTx_CFG();
}

void app_uart_main(void)
{
    uint8_t tmp;
    uint8_t i;

    // [UART受信]
    // UARTの受信FIFOが空ではなかったらFIFO読み出し
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET) {
        tmp = (USART_ReceiveData(USART1));
        s_uart_data.p_rx_buf[s_uart_data.rx_idx] = tmp;
        s_uart_data.rx_cnt++;
        s_uart_data.rx_idx = (s_uart_data.rx_idx + 1) % s_uart_data.rx_buf_size;
    }

    // [UART送信]
    // UARTの送信FIFOが空ではなかったらFIFOにデータを2Byte書き込み
#if 1
    if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET) {
        if(s_uart_data.rx_cnt > 0) {
            memcpy(&g_uart_tx_buf[0], &g_uart_rx_buf[0], s_uart_data.rx_cnt);
            memset(&g_uart_rx_buf[0], 0x00, s_uart_data.rx_buf_size);
            s_uart_data.tx_cnt = s_uart_data.rx_cnt;
            s_uart_data.rx_cnt = 0;

            // 受信したデータ分同じデータを送り返してローカルエコー
            for(i = 0; i < s_uart_data.tx_cnt; i++)
            {
                USART_SendData(USART1, (uint16_t)s_uart_data.p_tx_buf[s_uart_data.tx_idx]);
                s_uart_data.tx_idx = (s_uart_data.tx_idx + 1) % s_uart_data.tx_buf_size;
            }
            memset(&g_uart_tx_buf[0], 0x00, s_uart_data.tx_buf_size);
            s_uart_data.tx_cnt = 0;
        }
    }
#endif
}

/**
 * @brief メインループ
 * 
 * @return int 
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    hw_uart_init();

    while(1)
    {
        app_uart_main();
    }

    return 0;
}
