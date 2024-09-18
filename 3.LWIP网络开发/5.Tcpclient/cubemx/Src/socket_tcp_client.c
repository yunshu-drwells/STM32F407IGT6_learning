#include "socket_tcp_client.h"
#include "sockets.h"
#include "ctype.h"  //toupper

char ReadBuff[BUFF_SIZE];

void vTcpClientTask(void){
	
	int cfd, n, i;
	struct sockaddr_in server_addr;
	//����Socket
	cfd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  //�󶨷�����ip
	//���ӵ�������
	connect(cfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("server is connected in!\n");
	
	while(1){
		//�ȴ�����˷�������
		n = read(cfd, ReadBuff, BUFF_SIZE);
		//Сдת��д
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//��Ӧ�����
		write(cfd, ReadBuff, n);
	}
}
