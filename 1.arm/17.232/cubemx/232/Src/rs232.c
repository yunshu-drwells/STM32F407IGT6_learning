/**
 ****************************************************************************************************
 * @file        rs232.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 7* @date        2021-10-27
 * @brief       RS232 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 F407电机开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 * 修改说明
 * V1.0 20211027
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "rs232.h"

UART_HandleTypeDef g_rs232_handler;     /* RS232控制句柄(串口) */

#ifdef RS232_EN_RX /* 如果使能了接收 */

uint8_t g_RS232_rx_buf[RS232_REC_LEN]; /* 接收缓冲, 最大 RS232_REC_LEN 个字节. */
uint8_t g_RS232_rx_cnt = 0;            /* 接收到的数据长度 */

void RS232_UX_IRQHandler(void)
{
    uint8_t res;

    if ((__HAL_UART_GET_FLAG(&g_rs232_handler, UART_FLAG_RXNE) != RESET)) /* 接收到数据 */
    {
        HAL_UART_Receive(&g_rs232_handler, &res, 1, 1000);

        if (g_RS232_rx_cnt < RS232_REC_LEN)         /* 缓冲区未满 */
        {
            g_RS232_rx_buf[g_RS232_rx_cnt] = res;   /* 记录接收到的值 */
            g_RS232_rx_cnt++;                       /* 接收数据增加1 */
        }
    }
}

#endif

/**
 * @brief       RS232初始化函数
 *   @note      该函数主要是初始化串口
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void rs232_init(uint32_t baudrate)
{

    /* IO 及 时钟配置 */
    RS232_TX_GPIO_CLK_ENABLE(); /* 使能 串口TX脚 时钟 */
    RS232_RX_GPIO_CLK_ENABLE(); /* 使能 串口RX脚 时钟 */
    RS232_UX_CLK_ENABLE();      /* 使能 串口 时钟 */

    GPIO_InitTypeDef gpio_init_struct;
    
    gpio_init_struct.Pin = RS232_TX_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = RS232_AF_UART5;                /* 复用为串口5 */
    HAL_GPIO_Init(RS232_TX_GPIO_PORT, &gpio_init_struct);       /* 串口TX 脚 模式设置 */

    gpio_init_struct.Pin = RS232_RX_GPIO_PIN;
    HAL_GPIO_Init(RS232_RX_GPIO_PORT, &gpio_init_struct);       /* 串口RX 脚 必须设置成输入模式 */

    /* USART 初始化设置 */
    g_rs232_handler.Instance = RS232_UX;                    /* 选择232对应的串口 */
    g_rs232_handler.Init.BaudRate = baudrate;               /* 波特率 */
    g_rs232_handler.Init.WordLength = UART_WORDLENGTH_8B;   /* 字长为8位数据格式 */
    g_rs232_handler.Init.StopBits = UART_STOPBITS_1;        /* 一个停止位 */
    g_rs232_handler.Init.Parity = UART_PARITY_NONE;         /* 无奇偶校验位 */
    g_rs232_handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   /* 无硬件流控 */
    g_rs232_handler.Init.Mode = UART_MODE_TX_RX;            /* 收发模式 */
    HAL_UART_Init(&g_rs232_handler);                        /* 使能对应的串口 */
    __HAL_UART_DISABLE_IT(&g_rs232_handler, UART_IT_TC);

#if RS232_EN_RX /* 如果使能了接收 */
    __HAL_UART_ENABLE_IT(&g_rs232_handler, UART_IT_RXNE);   /* 开启接收中断 */
    HAL_NVIC_EnableIRQ(RS232_UX_IRQn);                      /* 使能RS232中断 */
    HAL_NVIC_SetPriority(RS232_UX_IRQn, 3, 3);              /* 抢占优先级3, 子优先级3 */
#endif

}

/**
 * @brief       RS232发送len个字节
 * @param       buf     : 发送区首地址
 * @param       len     : 发送的字节数(为了和本代码的接收匹配,这里建议不要超过 RS232_REC_LEN 个字节)
 * @retval      无
 */
void rs232_send_data(uint8_t *buf, uint8_t len)
{
    HAL_UART_Transmit(&g_rs232_handler, buf, len, 1000); /* RS232发送数据 */
    g_RS232_rx_cnt = 0;
}

/**
 * @brief       RS232查询接收到的数据
 * @param       buf     : 接收缓冲区首地址
 * @param       len     : 接收到的数据长度
 *   @arg               0   , 表示没有接收到任何数据
 *   @arg               其他, 表示接收到的数据长度
 * @retval      无
 */
void rs232_receive_data(uint8_t *buf, uint8_t *len)
{
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
    uint8_t rxlen = g_RS232_rx_cnt;
    uint8_t i = 0;
    *len = 0;     /* 默认为0 */
    HAL_Delay(10); /* 等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束 */

    if (rxlen == g_RS232_rx_cnt && rxlen) /* 接收到了数据,且接收完成了 */
    {
        for (i = 0; i < rxlen; i++)
        {
            buf[i] = g_RS232_rx_buf[i];
        }

        *len = g_RS232_rx_cnt; /* 记录本次数据长度 */
        g_RS232_rx_cnt = 0;    /* 清零 */
    }
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
}
