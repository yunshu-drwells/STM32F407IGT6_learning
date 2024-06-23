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
	//创建Socket
	cfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4，字节流，默认协议
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  //绑定服务器ip
	//连接到服务器
	ret = Connect(cfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		//100ms连接一次服务器，防止浪费cpu资源
		vTaskDelay(100);
		goto again;
	}
	printf("server is connected in!\n");
	
	while(1){
		//等待服务端发送数据
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if(n <= 0){
			goto again;
		}
		//小写转大写
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//响应服务端
		n = Write(cfd, ReadBuff, n);
		if(n <= 0){
			goto again;
		}
	}
}
