#include "socket_tcp_server.h"
#include "sockets.h"
#include "ctype.h"  //toupper

char ReadBuff[BUFF_SIZE];

void vTcpServerTask(void){
	//����Socket
	int sfd, cfd, n, i;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len;
	sfd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��ip
	bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	//����
	listen(sfd, 5);  //���5���ͻ���
	client_addr_len = sizeof(client_addr);
	//����accept
	cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
	printf("client is connect cfd = %d\n", cfd);
	
	while(1){
		//�ȴ��ͻ��˷�������
		n = read(cfd, ReadBuff, BUFF_SIZE);
		//Сдת��д
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//��Ӧ�ͻ���
		write(cfd, ReadBuff, n);
	}
}
