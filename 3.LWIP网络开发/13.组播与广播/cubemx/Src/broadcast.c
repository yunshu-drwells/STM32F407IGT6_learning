#include "broadcast.h"
#include "socket_wrap.h"
#include "string.h"
#include "FREERTOS.h"
#include "task.h"


/**
  * @brief  broadcast 服务器任务
  * @param  None
  * @retval None
  */
void vBroadcastTask(void){
	int 	 sfd;
	struct sockaddr_in client_addr;
	socklen_t	client_addr_len;
	int optval = 1;  //
	//创建socket	SOCK_DGRAM——udp通信
	sfd = Socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr("255.255.255.255");  //广播ip
	client_addr_len = sizeof(client_addr);
	while(1){
		//发送广播数据
		Sendto(sfd, "broadcast data", strlen("broadcast data"), 0, (struct sockaddr *)&client_addr, client_addr_len);
		vTaskDelay(5000);
	}
	
}
