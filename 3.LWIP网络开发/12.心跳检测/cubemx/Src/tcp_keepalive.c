#include "tcp_keepalive.h"
#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

char ReadBuff[BUFF_SIZE];

void vKeepaliveTask(void){
	int so_keepalive = 1;  /* 保持连接 */  
	int tcp_keepidle = 3;  /* 发送心跳空闲周期 S*/
	int tcp_keepintvl = 4; /* 发送心跳间隔 S */
	int tcp_keepcnt = 5;   /* 心跳重发次数 */
	int tcp_nodelay = 1;   /* Nagle算法*/
	int cfd, n, i, ret;
	struct sockaddr_in server_addr;
again:	
	//创建Socket 创建失败就会删除任务
	cfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4，字节流，默认协议
	setsockopt(cfd, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive, sizeof(int));  //使能socket层的心跳检测
	
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
	//配置心跳检测参数  默认参数——时间很长
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepidle, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepintvl, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_KEEPCNT, &tcp_keepcnt, sizeof(int));
	setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(int));
	
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

