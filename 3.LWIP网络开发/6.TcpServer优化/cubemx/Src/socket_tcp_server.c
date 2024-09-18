#include "socket_tcp_server.h"
#include "socket_wrap.h"  //使用自己封装的socket接口
#include "ctype.h"  //toupper 
#include "usart.h"  //fputc

char ReadBuff[BUFF_SIZE];

void vTcpServerTask(void){
	int sfd, cfd, n, i;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len;
	//创建Socket 创建失败就会删除任务
	sfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4，字节流，默认协议
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//绑定ip
	Bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	//监听
	Listen(sfd, 5);  //最多5个客户端
	client_addr_len = sizeof(client_addr);
again:
	//阻塞accept
	cfd = Accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
	printf("client is connect cfd = %d\n", cfd);
	
	while(1){
		//等待客户端发送数据
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if( n<= 0){  //出错，会关闭文件描述符
			goto again;
		}
		//小写转大写
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//响应客户端
		n = Write(cfd, ReadBuff, n);
		if(n <= 0){
			goto again;
		}
	}
}
