/**
 ****************************************************************************************************
 * @file        ethernet.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       ETHERNET 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 F407电机开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "ethernet.h"
#include "lwip_comm.h"
//#include "./SYSTEM/delay/delay.h"
#include "malloc.h"


ETH_HandleTypeDef g_eth_handler;            /* 以太网句柄 */
ETH_DMADescTypeDef *g_eth_dma_rx_dscr_tab;  /* 以太网DMA接收描述符数据结构体指针 */
ETH_DMADescTypeDef *g_eth_dma_tx_dscr_tab;  /* 以太网DMA发送描述符数据结构体指针 */
uint8_t *g_eth_rx_buf;                      /* 以太网底层驱动接收buffers指针 */
uint8_t *g_eth_tx_buf;                      /* 以太网底层驱动发送buffers指针 */


/**
 * @brief       以太网芯片初始化
 * @param       无
 * @retval      0,成功
 *              1,失败
 */
uint8_t ethernet_init(void)
{
    uint8_t macaddress[6];

    macaddress[0] = g_lwipdev.mac[0];
    macaddress[1] = g_lwipdev.mac[1];
    macaddress[2] = g_lwipdev.mac[2];
    macaddress[3] = g_lwipdev.mac[3];
    macaddress[4] = g_lwipdev.mac[4];
    macaddress[5] = g_lwipdev.mac[5];

    g_eth_handler.Instance = ETH;
    g_eth_handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;    /* 使能自协商模式 */
    g_eth_handler.Init.Speed = ETH_SPEED_100M;                          /* 速度100M,如果开启了自协商模式，此配置就无效 */
    g_eth_handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;                /* 全双工模式，如果开启了自协商模式，此配置就无效 */
    g_eth_handler.Init.PhyAddress = ETHERNET_PHY_ADDRESS;               /* 以太网芯片的地址 */
    g_eth_handler.Init.MACAddr = macaddress;                            /* MAC地址 */
    g_eth_handler.Init.RxMode = ETH_RXINTERRUPT_MODE;                   /* 中断接收模式 */
    g_eth_handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;         /* 硬件帧校验 */
    g_eth_handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;       /* RMII接口 */

    if (HAL_ETH_Init(&g_eth_handler) == HAL_OK)
    {
        return 0;   /* 成功 */
    }
    else
    {
        return 1;  /* 失败 */
    }
}

/**
 * @brief       ETH底层驱动，时钟使能，引脚配置
 *    @note     此函数会被HAL_ETH_Init()调用
 * @param       heth:以太网句柄
 * @retval      无
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef gpio_init_struct;

    ETH_CLK_GPIO_CLK_ENABLE();          /* 开启ETH_CLK时钟 */
    ETH_MDIO_GPIO_CLK_ENABLE();         /* 开启ETH_MDIO时钟 */
    ETH_CRS_GPIO_CLK_ENABLE();          /* 开启ETH_CRS时钟 */
    ETH_MDC_GPIO_CLK_ENABLE();          /* 开启ETH_MDC时钟 */
    ETH_RXD0_GPIO_CLK_ENABLE();         /* 开启ETH_RXD0时钟 */
    ETH_RXD1_GPIO_CLK_ENABLE();         /* 开启ETH_RXD1时钟 */
    ETH_TX_EN_GPIO_CLK_ENABLE();        /* 开启ETH_TX_EN时钟 */
    ETH_TXD0_GPIO_CLK_ENABLE();         /* 开启ETH_TXD0时钟 */
    ETH_TXD1_GPIO_CLK_ENABLE();         /* 开启ETH_TXD1时钟 */
    ETH_RESET_GPIO_CLK_ENABLE();        /* 开启ETH_RESET时钟 */
    __HAL_RCC_ETH_CLK_ENABLE();         /* 开启ETH时钟 */


    /* 网络引脚设置 RMII接口
     * ETH_MDIO -------------------------> PA2
     * ETH_MDC --------------------------> PC1
     * ETH_RMII_REF_CLK------------------> PA1
     * ETH_RMII_CRS_DV ------------------> PA7
     * ETH_RMII_RXD0 --------------------> PC4
     * ETH_RMII_RXD1 --------------------> PC5
     * ETH_RMII_TX_EN -------------------> PG11
     * ETH_RMII_TXD0 --------------------> PG13
     * ETH_RMII_TXD1 --------------------> PG14
     * ETH_RESET-------------------------> PI8
     */

    /* PA1,2,7 */
    gpio_init_struct.Pin = ETH_CLK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 推挽复用 */
    gpio_init_struct.Pull = GPIO_NOPULL;                    /* 不带上下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* 高速 */
    gpio_init_struct.Alternate = GPIO_AF11_ETH;             /* 复用为ETH功能 */
    HAL_GPIO_Init(ETH_CLK_GPIO_PORT, &gpio_init_struct);    /* ETH_CLK引脚模式设置 */
    
    gpio_init_struct.Pin = ETH_MDIO_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);   /* ETH_MDIO引脚模式设置 */
    
    gpio_init_struct.Pin = ETH_CRS_GPIO_PIN;    
    HAL_GPIO_Init(ETH_CRS_GPIO_PORT, &gpio_init_struct);    /* ETH_CRS引脚模式设置 */

    /* PC1 */
    gpio_init_struct.Pin = ETH_MDC_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDC_GPIO_PORT, &gpio_init_struct);    /* ETH_MDC初始化 */

    /* PC4 */
    gpio_init_struct.Pin = ETH_RXD0_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD0初始化 */
    
    /* PC5 */
    gpio_init_struct.Pin = ETH_RXD1_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_RXD1初始化 */
    
    
    /* PG11,13,14 */
    gpio_init_struct.Pin = ETH_TX_EN_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TX_EN_GPIO_PORT, &gpio_init_struct);  /* ETH_TX_EN初始化 */

    gpio_init_struct.Pin = ETH_TXD0_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TXD0_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD0初始化 */
    
    gpio_init_struct.Pin = ETH_TXD1_GPIO_PIN; 
    HAL_GPIO_Init(ETH_TXD1_GPIO_PORT, &gpio_init_struct);   /* ETH_TXD1初始化 */
    
    
    /* 复位引脚 */
    gpio_init_struct.Pin = ETH_RESET_GPIO_PIN;      /* ETH_RESET初始化 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;    /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_NOPULL;            /* 无上下拉 */
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;       /* 高速 */
    HAL_GPIO_Init(ETH_RESET_GPIO_PORT, &gpio_init_struct);

    ETHERNET_RST(0);     /* 硬件复位 */
    HAL_Delay(50);
    ETHERNET_RST(1);     /* 复位结束 */

    HAL_NVIC_SetPriority(ETH_IRQn, 1, 0);           /* 网络中断优先级应该高一点 */
    HAL_NVIC_EnableIRQ(ETH_IRQn);
}

/**
 * @breif       读取以太网芯片寄存器值
 * @param       reg：读取的寄存器地址
 * @retval      无
 */
uint32_t ethernet_read_phy(uint16_t reg)
{
    uint32_t regval;

    HAL_ETH_ReadPHYRegister(&g_eth_handler, reg, &regval);
    return regval;
}

/**
 * @breif       向以太网芯片指定地址写入寄存器值
 * @param       reg   : 要写入的寄存器
 * @param       value : 要写入的寄存器
 * @retval      无
 */
void ethernet_write_phy(uint16_t reg, uint16_t value)
{
    uint32_t temp = value;
    
    HAL_ETH_ReadPHYRegister(&g_eth_handler, reg, &temp);
}

/**
 * @breif       获得网络芯片的速度模式
 * @param       无
 * @retval      1:获取100M成功
                0:失败
 */
uint8_t ethernet_chip_get_speed(void)
{
    uint8_t speed;
    #if(PHY_TYPE == LAN8720) 
    speed = ~((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS));         /* 从LAN8720的31号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == SR8201F)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 13);    /* 从SR8201F的0号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == YT8512C)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 14);    /* 从YT8512C的17号寄存器中读取网络速度和双工模式 */
    #elif(PHY_TYPE == RTL8201)
    speed = ((ethernet_read_phy(PHY_SR) & PHY_SPEED_STATUS) >> 1);     /* 从RTL8201的16号寄存器中读取网络速度和双工模式 */
    #endif

    return speed;
}

extern void lwip_pkt_handle(void);                  /* 在lwip_comm.c里面定义 */

/**
 * @breif       中断服务函数
 * @param       无
 * @retval      无
 */
void ETH_IRQHandler(void)
{
    while (ethernet_get_eth_rx_size(g_eth_handler.RxDesc))
    {
        lwip_pkt_handle();      /* 处理以太网数据，即将数据提交给LWIP */
    }

    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_NIS);   /* 清除DMA中断标志位 */
    __HAL_ETH_DMA_CLEAR_IT(&g_eth_handler, ETH_DMA_IT_R);     /* 清除DMA接收中断标志位 */
}

/**
 * @breif       获取接收到的帧长度
 * @param       dma_rx_desc : 接收DMA描述符
 * @retval      frameLength : 接收到的帧长度
 */
uint32_t  ethernet_get_eth_rx_size(ETH_DMADescTypeDef *dma_rx_desc)
{
    uint32_t frameLength = 0;

    if (((dma_rx_desc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) &&
        ((dma_rx_desc->Status & ETH_DMARXDESC_ES)  == (uint32_t)RESET) &&
        ((dma_rx_desc->Status & ETH_DMARXDESC_LS)  != (uint32_t)RESET))
    {
        frameLength = ((dma_rx_desc->Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }

    return frameLength;
}

/**
 * @breif       为ETH底层驱动申请内存
 * @param       无
 * @retval      0,正常
 *              1,失败
 */
uint8_t ethernet_mem_malloc(void)
{
    g_eth_dma_rx_dscr_tab = mymalloc(SRAMIN, ETH_RXBUFNB * sizeof(ETH_DMADescTypeDef)); /* 申请内存 */
    g_eth_dma_tx_dscr_tab = mymalloc(SRAMIN, ETH_TXBUFNB * sizeof(ETH_DMADescTypeDef)); /* 申请内存 */
    g_eth_rx_buf = mymalloc(SRAMIN, ETH_RX_BUF_SIZE * ETH_RXBUFNB); /* 申请内存 */
    g_eth_tx_buf = mymalloc(SRAMIN, ETH_TX_BUF_SIZE * ETH_TXBUFNB); /* 申请内存 */

    if (!(uint32_t)&g_eth_dma_rx_dscr_tab || !(uint32_t)&g_eth_dma_tx_dscr_tab || !(uint32_t)&g_eth_rx_buf || !(uint32_t)&g_eth_tx_buf)
    {
        ethernet_mem_free();
        return 1;   /* 申请失败 */
    }

    return 0;       /* 申请成功 */
}

/**
 * @breif       释放ETH 底层驱动申请的内存
 * @param       无
 * @retval      无
 */
void ethernet_mem_free(void)
{
    myfree(SRAMIN, g_eth_dma_rx_dscr_tab);  /* 释放内存 */
    myfree(SRAMIN, g_eth_dma_tx_dscr_tab);  /* 释放内存 */
    myfree(SRAMIN, g_eth_rx_buf);           /* 释放内存 */
    myfree(SRAMIN, g_eth_tx_buf);           /* 释放内存 */
}
