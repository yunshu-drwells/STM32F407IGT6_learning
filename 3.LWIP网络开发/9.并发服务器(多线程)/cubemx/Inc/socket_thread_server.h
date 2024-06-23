#ifndef _SOCKET_THREAD_SERVER_H
#define _SOCKET_THREAD_SERVER_H

#define BUFF_SIZE   1024
#define SERVER_PORT 6666

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
void vNewClientTask(void const * argument);
/**
  * @brief  UDP服务器任务
  * @param  None
  * @retval None
  */
void vThreadServerTask(void);

#endif
