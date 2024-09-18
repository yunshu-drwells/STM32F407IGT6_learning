/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "netif/etharp.h"  
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"
#include "ethernetif.h" 
#include "lwip_comm.h" 
#include "string.h"


/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
void  ethernetif_input(struct netif *netif);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
    netif->hwaddr_len = ETHARP_HWADDR_LEN; /* 设置MAC地址长度,为6个字节 */
    /* 初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复 */
    netif->hwaddr[0] = g_lwipdev.mac[0]; 
    netif->hwaddr[1] = g_lwipdev.mac[1]; 
    netif->hwaddr[2] = g_lwipdev.mac[2];
    netif->hwaddr[3] = g_lwipdev.mac[3];   
    netif->hwaddr[4] = g_lwipdev.mac[4];
    netif->hwaddr[5] = g_lwipdev.mac[5];
    
    netif->mtu=1500; /* 最大允许传输单元,允许该网卡广播和ARP功能 */

    /* 网卡状态信息标志位，是很重要的控制字段，它包括网卡功能使能、广播 */
    /* 使能、 ARP 使能等等重要控制位 */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;   /* 广播 ARP协议 链接检测 */
    
    HAL_ETH_DMATxDescListInit(&g_eth_handler,g_eth_dma_tx_dscr_tab,g_eth_tx_buf,ETH_TXBUFNB); /* 初始化发送描述符 */
    HAL_ETH_DMARxDescListInit(&g_eth_handler,g_eth_dma_rx_dscr_tab,g_eth_rx_buf,ETH_RXBUFNB); /* 初始化接收描述符 */
    HAL_ETH_Start(&g_eth_handler); /* 开启ETH */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;
    
    uint8_t *buffer = (uint8_t *)(g_eth_handler.TxDesc->Buffer1Addr);
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;

    DmaTxDesc = g_eth_handler.TxDesc;
    bufferoffset = 0;

#if ETH_PAD_SIZE
  pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif
    
    /* 从pbuf中拷贝要发送的数据 */
    for(q = p;q != NULL;q = q->next)
    {
        /* 判断此发送描述符是否有效，即判断此发送描述符是否归以太网DMA所有 */
        if((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
        {
            errval = ERR_USE;
            goto error;               /* 发送描述符无效，不可用 */
        }
        
        byteslefttocopy = q->len;     /* 要发送的数据长度 */
        payloadoffset = 0; 
        
        /* 将pbuf中要发送的数据写入到以太网发送描述符中，有时候我们要发送的数据可能大于一个以太网
           描述符的Tx Buffer，因此我们需要分多次将数据拷贝到多个发送描述符中 */
        while((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE )
        {
            /* 将数据拷贝到以太网发送描述符的Tx Buffer中 */
            memcpy((uint8_t*)((uint8_t*)buffer + bufferoffset),(uint8_t*)((uint8_t*)q->payload + payloadoffset),(ETH_TX_BUF_SIZE - bufferoffset));
            /* DmaTxDsc指向下一个发送描述符 */
            DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
            /* 检查新的发送描述符是否有效 */
            if((DmaTxDesc->Status&ETH_DMATXDESC_OWN) != (uint32_t)RESET)
            {
                errval = ERR_USE;
                goto error;     /* 发送描述符无效，不可用 */
            }
            
            buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);   /* 更新buffer地址，指向新的发送描述符的Tx Buffer */
            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }
        /* 拷贝剩余的数据 */
        memcpy( (uint8_t*)((uint8_t*)buffer + bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),byteslefttocopy );
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }
    
    /* 当所有要发送的数据都放进发送描述符的Tx Buffer以后就可发送此帧了 */
    HAL_ETH_TransmitFrame(&g_eth_handler,framelength);
    errval = ERR_OK;
error:            
    /* 发送缓冲区发生下溢，一旦发送缓冲区发生下溢TxDMA会进入挂起状态 */
    if((g_eth_handler.Instance->DMASR&ETH_DMASR_TUS) != (uint32_t)RESET)
    {
        /* 清除下溢标志 */
        g_eth_handler.Instance->DMASR = ETH_DMASR_TUS;
        /* 当发送帧中出现下溢错误的时候TxDMA会挂起，这时候需要向DMATPDR寄存器 */
        /* 随便写入一个值来将其唤醒，此处我们写0 */
        g_eth_handler.Instance->DMATPDR=0;
    }
    
#if ETH_PAD_SIZE
  pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    
    return errval;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{  
    struct pbuf *p, *q;
    u16_t len;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t i = 0;
  
    if(HAL_ETH_GetReceivedFrame(&g_eth_handler) != HAL_OK)  /* 判断是否接收到数据 */
    return NULL;
    
    len = g_eth_handler.RxFrameInfos.length;                /* 获取接收到的以太网帧长度 */
    
#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
    
    buffer = (uint8_t *)g_eth_handler.RxFrameInfos.buffer;  /* 获取接收到的以太网帧的数据buffer */
  
    p = pbuf_alloc(PBUF_RAW,len,PBUF_POOL);                 /* 申请pbuf */
    
    if(p != NULL)                                           /* pbuf申请成功 */
    {
        dmarxdesc = g_eth_handler.RxFrameInfos.FSRxDesc;    /* 获取接收描述符链表中的第一个描述符 */
        bufferoffset = 0;
        
        for(q = p;q != NULL;q = q->next)
        {
            byteslefttocopy = q->len;
            payloadoffset = 0;
            
            /* 将接收描述符中Rx Buffer的数据拷贝到pbuf中 */
            while((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
            {
                /* 将数据拷贝到pbuf中 */
                memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer + bufferoffset),(ETH_RX_BUF_SIZE - bufferoffset));
                 /* dmarxdesc向下一个接收描述符 */
                dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                /* 更新buffer地址，指向新的接收描述符的Rx Buffer */
                buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);
 
                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* 拷贝剩余的数据 */
            memcpy((uint8_t*)((uint8_t*)q->payload + payloadoffset),(uint8_t*)((uint8_t*)buffer + bufferoffset),byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }
    else
    {
        /* drop packet();  丢包函数自行编写 */
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
    }
    
    /* 释放DMA描述符 */
    dmarxdesc = g_eth_handler.RxFrameInfos.FSRxDesc;
    
    for(i = 0;i < g_eth_handler.RxFrameInfos.SegCount; i ++)
    {  
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;       /* 标记描述符归DMA所有 */
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    
    g_eth_handler.RxFrameInfos.SegCount = 0;           /* 清除段计数器 */
    
    if((g_eth_handler.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)  /* 接收缓冲区不可用 */
    {
        /* 清除接收缓冲区不可用标志 */
        g_eth_handler.Instance->DMASR = ETH_DMASR_RBUS;
        /* 当接收缓冲区不可用的时候RxDMA会进去挂起状态，通过向DMARPDR写入任意一个值来唤醒Rx DMA */
        g_eth_handler.Instance->DMARPDR = 0;
    }
    
    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* if no packet could be read, silently ignore this */
    if (p != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }

} 

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    
    if (ethernetif == NULL)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */
    MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
 * @ingroup sys_time
 * Returns the current time in milliseconds,
 * may be the same as sys_jiffies or at least based on it.
 * Don't care for wraparound, this is only used for time diffs.
 * Not implementing this function means you cannot use some modules (e.g. TCP
 * timestamps, internal timeouts for NO_SYS==1).
 */
u32_t sys_now(void)
{
    return HAL_GetTick();
}

