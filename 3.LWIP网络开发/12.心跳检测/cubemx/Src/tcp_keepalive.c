#include "tcp_keepalive.h"
#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

char ReadBuff[BUFF_SIZE];

void vKeepaliveTask(void){
	int so_keepalive = 1;  /* �������� */  
	int tcp_keepidle = 3;  /* ���������������� S*/
	int tcp_keepintvl = 4; /* ����������� S */
	int tcp_keepcnt = 5;   /* �����ط����� */
	int tcp_nodelay = 1;   /* Nagle�㷨*/
	int cfd, n, i, ret;
	struct sockaddr_in server_addr;
again:	
	//����Socket ����ʧ�ܾͻ�ɾ������
	cfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	setsockopt(cfd, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive, sizeof(int));  //ʹ��socket����������
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  //�󶨷�����ip
	//���ӵ�������
	ret = Connect(cfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		//100ms����һ�η���������ֹ�˷�cpu��Դ
		vTaskDelay(100);
		goto again;
	}
	//��������������  Ĭ�ϲ�������ʱ��ܳ�
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepidle, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepintvl, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPCNT, &tcp_keepcnt, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(int));
	
	printf("server is connected in!\n");
	
	while(1){
		//�ȴ�����˷�������
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if(n <= 0){
			goto again;
		}
		//Сдת��д
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//��Ӧ�����
		n = Write(cfd, ReadBuff, n);
		if(n <= 0){
			goto again;
		}
	}
}

