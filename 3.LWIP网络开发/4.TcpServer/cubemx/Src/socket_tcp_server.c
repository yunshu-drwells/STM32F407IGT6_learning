#include "socket_tcp_server.h"
#include "sockets.h"
#include "ctype.h"  //toupper

char ReadBuff[BUFF_SIZE];

void vTcpServerTask(void){
	//创建Socket
	int sfd, cfd, n, i;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len;
	sfd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4，字节流，默认协议
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//绑定ip
	bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	//监听
	listen(sfd, 5);  //最多5个客户端
	client_addr_len = sizeof(client_addr);
	//阻塞accept
	cfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
	printf("client is connect cfd = %d\n", cfd);
	
	while(1){
		//等待客户端发送数据
		n = read(cfd, ReadBuff, BUFF_SIZE);
		//小写转大写
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//响应客户端
		write(cfd, ReadBuff, n);
	}
}
