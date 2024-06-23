#include "multicast.h"
#include "sockets.h"  //ip_mreq
#include "socket_wrap.h"
#include "FREERTOS.h"
#include "task.h"
/**
  * @brief  multicast 服务器任务
  * @param  None
  * @retval None
  */
void vMulticastTask(void){
	int 	 sfd;
	struct sockaddr_in client_addr;
	socklen_t	client_addr_len;
	ip_mreq multicast_mreq;
	//创建socket	SOCK_DGRAM——udp通信
	sfd = Socket(AF_INET, SOCK_DGRAM, 0);
	
	multicast_mreq.imr_multiaddr.s_addr = inet_addr("224.0.1.1");  //组播地址
	multicast_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	//设置组播选项
	setsockopt(sfd, IPPROTO_IP, IP_MULTICAST_IF, &multicast_mreq, sizeof(multicast_mreq));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr("224.0.1.1");  //组播ip
	client_addr_len = sizeof(client_addr);
	while(1){
		//发送组播数据
		Sendto(sfd, "multicast data", strlen("multicast data"), 0, (struct sockaddr *)&client_addr, client_addr_len);
		vTaskDelay(5000);
	}
	
}
