#include "socket_select_server.h"
#include "socket_wrap.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ctype.h"

static char ReadBuff[BUFF_SIZE];
/**
  * @brief  select 并发服务器
  * @param  none
  * @retval none
  */
void vSelectServerTask(void)
{

	int 	 sfd, cfd, maxfd, i, nready, n, j;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	client_addr_len;
	fd_set all_set, read_set;
	//FD_SETSIZE里面包含了服务器的fd
	int		 clientfds[FD_SETSIZE - 1];
	
	//创建socket
	sfd = Socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port   			= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//绑定socket
	Bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//监听socket
	Listen(sfd, 5);
	
	client_addr_len = sizeof(client_addr);
	//初始化 maxfd 等于 sfd
	maxfd = sfd;
	
	//清空fdset
	FD_ZERO(&all_set);
	
	
	//把sfd文件描述符添加到集合中
	
	FD_SET(sfd, &all_set);
	
	
	//初始化客户端fd的集合
	
	for(i = 0; i < FD_SETSIZE -1 ; i++){
		//初始化为-1
		clientfds[i] = -1;
	}
	while(1)
	{
		//每次select返回之后，fd_set集合就会变化，再select时，就不能使用，
		//所以我们要保存设置fd_set 和 读取的fd_set
		read_set = all_set;
		nready = select(maxfd + 1, &read_set, NULL, NULL, NULL);
		//没有超时机制，不会返回0
		if(nready < 0){
			printf("select error \r\n");
			vTaskDelete(NULL);
		}
		//判断监听的套接字是否有数据
		if(FD_ISSET(sfd, &read_set)){
		
			//有了客户端进行连接了
			//客户接入
			cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
			if(cfd < 0){
				printf("accept socket error\r\n");
				//继续select
				continue;
			}
			printf("new client connect fd = %d\r\n", cfd);
			//把新的cfd 添加到fd_set集合中
			FD_SET(cfd, &all_set);
			//更新要select的maxfd
			maxfd = (cfd > maxfd)?cfd:maxfd;
			//把新的cfd 保存到cfds集合中
			for(i = 0; i < FD_SETSIZE -1 ; i++){
				if(clientfds[i] == -1){
					clientfds[i] = cfd;
					//退出，不需要添加
					break;
				
				}
			}
			//没有其他套接字需要处理：这里防止重复工作，就不去执行其他任务
			if(--nready == 0){
				//继续select
				continue;
			}
		
		}
		//遍历所有的客户端文件描述符
		for(i = 0; i < FD_SETSIZE -1 ; i++){
			if(clientfds[i] == -1){
				//继续遍历
				continue;
			}
			//是否在我们fd_set集合里面
			if(FD_ISSET(clientfds[i], &read_set)){
				n = Read(clientfds[i], ReadBuff, BUFF_SIZE);
				//Read函数已经关闭了这个客户端的fd
				if(n <= 0){
					//从集合里面清除
					FD_CLR(clientfds[i], &all_set);
					//当前的客户端fd 赋值为-1
					clientfds[i] = -1;
				}else{
					//进行大小写转换
					for(j = 0; j < n; j++){
					
						ReadBuff[j] = toupper(ReadBuff[j]);		
					}
					//写回客户端
					n = Write(clientfds[i], ReadBuff, n);
					if(n < 0){
						//从集合里面清除
						FD_CLR(clientfds[i], &all_set);
						//当前的客户端fd 赋值为-1
						clientfds[i] = -1;		
					}				
				}
			}

		}		
	}
}
