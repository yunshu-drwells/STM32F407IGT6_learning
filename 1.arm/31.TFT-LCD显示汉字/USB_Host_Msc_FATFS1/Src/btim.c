#include "btim.h"
#include "usbh_core.h"
#include "usbh_diskio_dma.h"


TIM_HandleTypeDef timx_handler;         /* 定时器参数句柄 */

extern USBH_HandleTypeDef hUsbHostFS;   /* USB Host处理结构体 */
extern void usbh_hid_reconnect(void);


/**
 * @brief       USB枚举状态死机检测,防止USB枚举失败导致的死机
 * @param       phost : USB HOST结构体指针
 * @retval      0, 没有死机
 *              1, 死机了,外部必须重新启动USB连接.
 */
uint8_t usbh_check_enume_dead(USBH_HandleTypeDef *phost)
{
    static uint16_t errcnt = 0;

    /* 这个状态,如果持续存在,则说明USB死机了 */
    if (phost->gState == HOST_ENUMERATION && (phost->EnumState == ENUM_IDLE || phost->EnumState == ENUM_GET_FULL_DEV_DESC))
    {
        errcnt++;

        if (errcnt > 2000)              /* 死机了 */
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
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timx_handler);  /* 定时器回调函数 */
}

/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&timx_handler))
    {
//        LED1_TOGGLE(); /* LED1反转 */
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        USBH_Process(&hUsbHostFS);
        if(usbh_check_enume_dead(&hUsbHostFS))    /* 检测USB HOST 枚举是否死机了?死机了,则重新初始化 */
        {
            printf("Error! USB HID reconnect!\r\n");
            usbh_hid_reconnect();       /* 重连 */
        }
    }
}


/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为42M, 所以定时器时钟 = 84Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr : 自动重装值
 * @param       psc : 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    timx_handler.Instance = BTIM_TIMX_INT;                      /* 通用定时器X */
    timx_handler.Init.Prescaler = psc;                          /* 设置预分频器 */
    timx_handler.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    timx_handler.Init.Period = arr;                             /* 自动装载值 */

    HAL_TIM_Base_Init(&timx_handler);
    HAL_TIM_Base_Start_IT(&timx_handler);                       /* 使能通用定时器x和及其更新中断：TIM_IT_UPDATE */
}

/**
 * @brief       定时器底层驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                     /* 使能TIM时钟 */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 2, 2); /* 抢占2，子优先级2，组2 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);         /* 开启ITM6中断 */
    }
}
