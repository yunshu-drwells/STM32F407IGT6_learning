#ifndef _SOCKET_TCP_CLIENT_H
#define _SOCKET_TCP_CLIENT_H

#define BUFF_SIZE   1024
#define SERVER_PORT 6666
#define SERVER_IP   "10.10.1.30"


/**
  * @brief  TCP客户端任务
  * @param  None
  * @retval None
  */
void vTcpClientTask(void);

#endif
