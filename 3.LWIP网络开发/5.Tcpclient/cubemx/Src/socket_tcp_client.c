#include "socket_tcp_client.h"
#include "sockets.h"
#include "ctype.h"  //toupper

char ReadBuff[BUFF_SIZE];

void vTcpClientTask(void){
	
	int cfd, n, i;
	struct sockaddr_in server_addr;
	//创建Socket
	cfd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4，字节流，默认协议
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  //绑定服务器ip
	//连接到服务器
	connect(cfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("server is connected in!\n");
	
	while(1){
		//等待服务端发送数据
		n = read(cfd, ReadBuff, BUFF_SIZE);
		//小写转大写
		for(i=0; i<n; ++i){
			ReadBuff[i] = toupper(ReadBuff[i]);
		}
		//响应服务端
		write(cfd, ReadBuff, n);
	}
}
