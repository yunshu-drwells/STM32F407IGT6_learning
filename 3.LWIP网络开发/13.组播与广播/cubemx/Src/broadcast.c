#include "broadcast.h"
#include "socket_wrap.h"
#include "string.h"
#include "FREERTOS.h"
#include "task.h"


/**
  * @brief  broadcast ����������
  * @param  None
  * @retval None
  */
void vBroadcastTask(void){
	int 	 sfd;
	struct sockaddr_in client_addr;
	socklen_t	client_addr_len;
	int optval = 1;  //
	//����socket	SOCK_DGRAM����udpͨ��
	sfd = Socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SERVER_PORT);
	client_addr.sin_addr.s_addr = inet_addr("255.255.255.255");  //�㲥ip
	client_addr_len = sizeof(client_addr);
	while(1){
		//���͹㲥����
		Sendto(sfd, "broadcast data", strlen("broadcast data"), 0, (struct sockaddr *)&client_addr, client_addr_len);
		vTaskDelay(5000);
	}
	
}
