#include "socket_tcp_server.h"
#include "socket_wrap.h"  //ʹ���Լ���װ��socket�ӿ�
#include "ctype.h"  //toupper 
#include "usart.h"  //fputc

char ReadBuff[BUFF_SIZE];

void vTcpServerTask(void){
	int sfd, cfd, n, i;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len;
	//����Socket ����ʧ�ܾͻ�ɾ������
	sfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��ip
	Bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	//����
	Listen(sfd, 5);  //���5���ͻ���
	client_addr_len = sizeof(client_addr);
again:
	//����accept
	cfd = Accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
	printf("client is connect cfd = %d\n", cfd);
	
	while(1){
		//�ȴ��ͻ��˷�������
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if( n<= 0){  //������ر��ļ�������
			goto again;
		}
		//Сдת��д
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//��Ӧ�ͻ���
		n = Write(cfd, ReadBuff, n);
		if(n <= 0){
			goto again;
		}
	}
}
