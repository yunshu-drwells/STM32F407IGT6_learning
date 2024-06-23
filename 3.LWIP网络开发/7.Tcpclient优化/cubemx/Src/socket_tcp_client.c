#include "socket_tcp_client.h"
#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ctype.h"  //toupper
#include "usart.h"

char ReadBuff[BUFF_SIZE];

void vTcpClientTask(void){
	
	int cfd, n, i, ret;
	struct sockaddr_in server_addr;
again:
	//����Socket
	cfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	
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
