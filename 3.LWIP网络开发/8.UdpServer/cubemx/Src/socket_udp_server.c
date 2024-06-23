#include "socket_udp_server.h"
#include "socket_wrap_udp.h"
#include "ctype.h"

static char ReadBuff[BUFF_SIZE];

/**
  * @brief  udp ����������
  * @param  None
  * @retval None
  */
void vUdpServerTask(void){

	int 	 sfd, n, i;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	client_addr_len;
	
	//����socket	SOCK_DGRAM����udpͨ��
	sfd = Socket(AF_INET, SOCK_DGRAM, 0);
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port   			= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��socket
	Bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	client_addr_len = sizeof(client_addr);
	while(1){
		//�ȴ��ͻ��˷�������
		n = Recvfrom(sfd, ReadBuff, BUFF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
		//���д�Сдת��
		for(i = 0; i < n; i++){
		
			ReadBuff[i] = toupper(ReadBuff[i]);		
		}
		//д�ؿͻ���
		Sendto(sfd, ReadBuff, BUFF_SIZE, 0, (struct sockaddr *)&client_addr, client_addr_len);
	}
	
}
