/**
 ****************************************************************************************************
 * @file        lwip_comm.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-12-02
 * @brief       LWIP������������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� F407���������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211202
 * ��һ�η���
 *
 ****************************************************************************************************
 */
 
#include "lwip_comm.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"
#include "lwip/tcpip.h"
#include "malloc.h"
//#include "./SYSTEM/delay/delay.h"
//#include "./SYSTEM/usart/usart.h"
#include <stdio.h>


__lwip_dev g_lwipdev;                                   /* lwip���ƽṹ�� */
struct netif g_lwip_netif;                              /* ����һ��ȫ�ֵ�����ӿ� */

#if LWIP_DHCP
uint32_t g_dhcp_fine_timer = 0;                         /* DHCP��ϸ�����ʱ�� */
__IO uint8_t g_lwip_dhcp_state = LWIP_DHCP_OFF;         /* DHCP״̬��ʼ�� */
#endif

void lwip_link_status_updated(struct netif *netif);
void lwip_dhcp_process(struct netif *netif);

/**
 * @breif       lwip Ĭ��IP����
 * @param       lwipx  : lwip���ƽṹ��ָ��
 * @retval      ��
 */
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
    /* Ĭ��Զ��IPΪ:192.168.1.134 */
    lwipx->remoteip[0] = 192;
    lwipx->remoteip[1] = 168;
    lwipx->remoteip[2] = 1;
    lwipx->remoteip[3] = 111;
    
    /* MAC��ַ���� */
    lwipx->mac[0] = 0xB8;
    lwipx->mac[1] = 0xAE;
    lwipx->mac[2] = 0x1D;
    lwipx->mac[3] = 0x00;
    lwipx->mac[4] = 0x07;
    lwipx->mac[5] = 0x00;
    
    /* Ĭ�ϱ���IPΪ:192.168.1.30 */
    lwipx->ip[0] = 192;
    lwipx->ip[1] = 168;
    lwipx->ip[2] = 1;
    lwipx->ip[3] = 30;
    /* Ĭ����������:255.255.255.0 */
    lwipx->netmask[0] = 255;
    lwipx->netmask[1] = 255;
    lwipx->netmask[2] = 255;
    lwipx->netmask[3] = 0;
    
    /* Ĭ������:192.168.1.1 */
    lwipx->gateway[0] = 192;
    lwipx->gateway[1] = 168;
    lwipx->gateway[2] = 1;
    lwipx->gateway[3] = 1;
    lwipx->dhcpstatus = 0; /* û��DHCP */
}

/**
 * @breif       LWIP��ʼ��(LWIP������ʱ��ʹ��)
 * @param       ��
 * @retval      0,�ɹ�
 *              1,�ڴ����
 *              2,��̫��оƬ��ʼ��ʧ��
 *              3,�������ʧ��.
 */
uint8_t lwip_comm_init(void)
{
    uint8_t retry = 0;
    struct netif *netif_init_flag;                  /* ����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ� */
    ip_addr_t ipaddr;                               /* ip��ַ */
    ip_addr_t netmask;                              /* �������� */
    ip_addr_t gw;                                   /* Ĭ������ */

    if (ethernet_mem_malloc())return 1;             /* �ڴ�����ʧ��*/

    lwip_comm_default_ip_set(&g_lwipdev);           /* ����Ĭ��IP����Ϣ */

    while (ethernet_init())                         /* ��ʼ����̫��оƬ,���ʧ�ܵĻ�������5�� */
    {
        retry++;

        if (retry > 5)
        {
            retry = 0;                              /* ��̫��оƬ��ʼ��ʧ�� */
            return 3;
        }
    }

    lwip_init();                                    /* ��ʼ��LWIP�ں� */

#if LWIP_DHCP                                       /* ʹ�ö�̬IP */
    ip_addr_set_zero_ip4(&ipaddr);                  /* ��IP��ַ���������뼰�������� */
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else   /* ʹ�þ�̬IP */
    IP4_ADDR(&ipaddr, g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
    IP4_ADDR(&netmask, g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
    IP4_ADDR(&gw, g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
    printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n", g_lwipdev.mac[0], g_lwipdev.mac[1], g_lwipdev.mac[2], g_lwipdev.mac[3], g_lwipdev.mac[4], g_lwipdev.mac[5]);
    printf("��̬IP��ַ........................%d.%d.%d.%d\r\n", g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
    printf("��������..........................%d.%d.%d.%d\r\n", g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
    printf("Ĭ������..........................%d.%d.%d.%d\r\n", g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
    g_lwipdev.dhcpstatus = 0XFF;
#endif  /* �������б������һ������ */
    netif_init_flag = netif_add(&g_lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask, (const ip_addr_t *)&gw, NULL, &ethernetif_init, &ethernet_input);


    if (netif_init_flag == NULL)
    {
        return 2;                                   /* �������ʧ�� */
    }
    else                                            /* ������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif���� */
    {
        netif_set_default(&g_lwip_netif);           /* ����netifΪĬ������ */

        if (netif_is_link_up(&g_lwip_netif))
        {
            netif_set_up(&g_lwip_netif);            /* ��netif���� */
        }
        else
        {
            netif_set_down(&g_lwip_netif);
        }

        lwip_link_status_updated(&g_lwip_netif);    /* DHCP����״̬���º��� */
        netif_set_link_callback(&g_lwip_netif, lwip_link_status_updated);
    }

#if LWIP_DHCP                                       /* ���ʹ��DHCP�Ļ� */
    g_lwipdev.dhcpstatus = 0;                       /* DHCP���Ϊ0 */
#endif
    return 0;                                       /* ����OK. */
}

/**
 * @breif       �����յ����ݺ����
 * @param       ��
 * @retval      ��
 */
void lwip_pkt_handle(void)
{
    /* �����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� */
    ethernetif_input(&g_lwip_netif);
}

/**
 * @brief       ֪ͨ�û�����ӿ�����״̬
 * @param       netif���������ƿ�
 * @retval      ��
 */
void lwip_link_status_updated(struct netif *netif)
{
    if (netif_is_up(netif))
    {
#if LWIP_DHCP
        /* Update DHCP state machine */
        g_lwip_dhcp_state = LWIP_DHCP_START;
#endif /* LWIP_DHCP */
    }
    else
    {
#if LWIP_DHCP
        /* Update DHCP state machine */
        g_lwip_dhcp_state = LWIP_DHCP_LINK_DOWN;
#endif /* LWIP_DHCP */
    }
}

/**
 * @breif       LWIP��ѯ����
 * @param       ��
 * @retval      ��
 */
void lwip_periodic_handle(void)
{
    sys_check_timeouts();

#if LWIP_DHCP       /* ���ʹ��DHCP */
    /* Fine DHCP periodic process every 500ms */
    if (HAL_GetTick() - g_dhcp_fine_timer >= DHCP_FINE_TIMER_MSECS)
    {
        g_dhcp_fine_timer =  HAL_GetTick();
        /* process DHCP state machine */
        lwip_dhcp_process(&g_lwip_netif);
    }
#endif

}

#if LWIP_DHCP       /* ���ʹ��DHCP */

/**
 * @brief       lwIP��DHCP�������
 * @param       netif���������ƿ�
 * @retval      ��
 */
void lwip_dhcp_process(struct netif *netif)
{
    uint32_t ip = 0;
    uint32_t netmask = 0;
    uint32_t gw = 0;
    struct dhcp *dhcp;
    uint8_t iptxt[20];                  /* �洢�ѷ����IP��ַ */
    g_lwipdev.dhcpstatus = 1;           /* DHCP���Ϊ1 */
    
    /* ����DHCP״̬����ִ����Ӧ�Ķ��� */
    switch (g_lwip_dhcp_state)
    {
        case LWIP_DHCP_START:
        {
            /* ��IP��ַ�����ص�ַ������ҳ��������� */
            ip_addr_set_zero_ip4(&netif->ip_addr);
            ip_addr_set_zero_ip4(&netif->netmask);
            ip_addr_set_zero_ip4(&netif->gw);
            /* ����DHCP��״̬Ϊ�ȴ�����IP��ַ */
            g_lwip_dhcp_state = LWIP_DHCP_WAIT_ADDRESS;
            /* ����DHCP */
            dhcp_start(netif);
        }
        break;

        case LWIP_DHCP_WAIT_ADDRESS:
        {
            ip = g_lwip_netif.ip_addr.addr;       /* ��ȡ��IP��ַ */
            netmask = g_lwip_netif.netmask.addr;  /* ��ȡ�������� */
            gw = g_lwip_netif.gw.addr;            /* ��ȡĬ������ */
            printf ("�ȴ�DHCP�����������......\r\n");
            
            if (dhcp_supplied_address(netif)) 
            {
                printf ("DHCP����ɹ�......\r\n");
                g_lwip_dhcp_state = LWIP_DHCP_ADDRESS_ASSIGNED;
                sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
                printf ("IP address assigned by a DHCP server: %s\r\n", iptxt);
                
                if (ip != 0)
                {
                    g_lwipdev.dhcpstatus = 2;         /* DHCP�ɹ� */
                    printf("����en��MAC��ַΪ:................%d.%d.%d.%d.%d.%d\r\n", g_lwipdev.mac[0], g_lwipdev.mac[1], g_lwipdev.mac[2], g_lwipdev.mac[3], g_lwipdev.mac[4], g_lwipdev.mac[5]);
                    /* ������ͨ��DHCP��ȡ����IP��ַ */
                    g_lwipdev.ip[3] = (uint8_t)(ip >> 24);
                    g_lwipdev.ip[2] = (uint8_t)(ip >> 16);
                    g_lwipdev.ip[1] = (uint8_t)(ip >> 8);
                    g_lwipdev.ip[0] = (uint8_t)(ip);
                    printf("ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n", g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
                    /* ����ͨ��DHCP��ȡ�������������ַ */
                    g_lwipdev.netmask[3] = (uint8_t)(netmask >> 24);
                    g_lwipdev.netmask[2] = (uint8_t)(netmask >> 16);
                    g_lwipdev.netmask[1] = (uint8_t)(netmask >> 8);
                    g_lwipdev.netmask[0] = (uint8_t)(netmask);
                    printf("ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n", g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
                    /* ������ͨ��DHCP��ȡ����Ĭ������ */
                    g_lwipdev.gateway[3] = (uint8_t)(gw >> 24);
                    g_lwipdev.gateway[2] = (uint8_t)(gw >> 16);
                    g_lwipdev.gateway[1] = (uint8_t)(gw >> 8);
                    g_lwipdev.gateway[0] = (uint8_t)(gw);
                    printf("ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n", g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
                }
            }
            else
            {
                dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

                /* DHCP��ʱ */
                if (dhcp->tries > LWIP_MAX_DHCP_TRIES)
                {
                    printf ("DHCP����ʧ�ܣ����뾲̬����......\r\n");
                    g_lwip_dhcp_state = LWIP_DHCP_TIMEOUT;

                    /* ֹͣDHCP */
                    dhcp_stop(netif);
                    g_lwipdev.dhcpstatus = 0XFF;
                    /* ʹ�þ�̬IP��ַ */
                    IP4_ADDR(&(g_lwip_netif.ip_addr), g_lwipdev.ip[0], g_lwipdev.ip[1], g_lwipdev.ip[2], g_lwipdev.ip[3]);
                    IP4_ADDR(&(g_lwip_netif.netmask), g_lwipdev.netmask[0], g_lwipdev.netmask[1], g_lwipdev.netmask[2], g_lwipdev.netmask[3]);
                    IP4_ADDR(&(g_lwip_netif.gw), g_lwipdev.gateway[0], g_lwipdev.gateway[1], g_lwipdev.gateway[2], g_lwipdev.gateway[3]);
                    netif_set_addr(netif, &g_lwip_netif.ip_addr, &g_lwip_netif.netmask, &g_lwip_netif.gw);
                    sprintf((char *)iptxt, "%s", ip4addr_ntoa((const ip4_addr_t *)&netif->ip_addr));
                    printf ("Static IP address: %s\r\n", iptxt);
                }
            }
        }
        break;
        case LWIP_DHCP_LINK_DOWN:
        {
            /* ֹͣDHCP */
            dhcp_stop(netif);
            g_lwip_dhcp_state = LWIP_DHCP_OFF; 
        }
        break;
        default: break;
    }
}
#endif
