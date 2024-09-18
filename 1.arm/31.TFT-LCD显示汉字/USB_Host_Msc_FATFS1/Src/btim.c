#include "btim.h"
#include "usbh_core.h"
#include "usbh_diskio_dma.h"


TIM_HandleTypeDef timx_handler;         /* ��ʱ��������� */

extern USBH_HandleTypeDef hUsbHostFS;   /* USB Host����ṹ�� */
extern void usbh_hid_reconnect(void);


/**
 * @brief       USBö��״̬�������,��ֹUSBö��ʧ�ܵ��µ�����
 * @param       phost : USB HOST�ṹ��ָ��
 * @retval      0, û������
 *              1, ������,�ⲿ������������USB����.
 */
uint8_t usbh_check_enume_dead(USBH_HandleTypeDef *phost)
{
    static uint16_t errcnt = 0;

    /* ���״̬,�����������,��˵��USB������ */
    if (phost->gState == HOST_ENUMERATION && (phost->EnumState == ENUM_IDLE || phost->EnumState == ENUM_GET_FULL_DEV_DESC))
    {
        errcnt++;

        if (errcnt > 2000)              /* ������ */
        {
            errcnt = 0;
            return 1;
        }
    }
    else 
    {
        errcnt = 0;
    }

    return 0;
}
/**
 * @brief       ������ʱ��TIMX�жϷ�����
 * @param       ��
 * @retval      ��
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timx_handler);  /* ��ʱ���ص����� */
}

/**
 * @brief       �ص���������ʱ���жϷ���������
 * @param       ��
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&timx_handler))
    {
//        LED1_TOGGLE(); /* LED1��ת */
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        USBH_Process(&hUsbHostFS);
        if(usbh_check_enume_dead(&hUsbHostFS))    /* ���USB HOST ö���Ƿ�������?������,�����³�ʼ�� */
        {
            printf("Error! USB HID reconnect!\r\n");
            usbh_hid_reconnect();       /* ���� */
        }
    }
}


/**
 * @brief       ������ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ42M, ���Զ�ʱ��ʱ�� = 84Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr : �Զ���װֵ
 * @param       psc : ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    timx_handler.Instance = BTIM_TIMX_INT;                      /* ͨ�ö�ʱ��X */
    timx_handler.Init.Prescaler = psc;                          /* ����Ԥ��Ƶ�� */
    timx_handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* ���ϼ����� */
    timx_handler.Init.Period = arr;                             /* �Զ�װ��ֵ */

    HAL_TIM_Base_Init(&timx_handler);
    HAL_TIM_Base_Start_IT(&timx_handler);                       /* ʹ��ͨ�ö�ʱ��x�ͼ�������жϣ�TIM_IT_UPDATE */
}

/**
 * @brief       ��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
                �˺����ᱻHAL_TIM_Base_Init()��������
 * @param       ��
 * @retval      ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                     /* ʹ��TIMʱ�� */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 2, 2); /* ��ռ2�������ȼ�2����2 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);         /* ����ITM6�ж� */
    }
}
