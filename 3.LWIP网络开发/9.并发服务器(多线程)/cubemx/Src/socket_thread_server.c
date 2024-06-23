#include "socket_thread_server.h"
#include "socket_wrap.h"
#include "cmsis_os.h"  //osPriorityNormal
#include "FREERTOS.h"
#include "task.h"
#include "ctype.h"


static char ReadBuff[BUFF_SIZE];

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void vNewClientTask(void const * argument)
{
	// 每一个任务，都有独立的栈空间
	int cfd = * (int *)argument;
	int n, i;
	while(1){
		//等待客户端发送数据
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if(n <= 0){
			close(cfd);
			vTaskDelete(NULL);
		}
		//进行大小写转换
		for(i = 0; i < n; i++){
		
			ReadBuff[i] = toupper(ReadBuff[i]);		
		}
		//写回客户端
		n = Write(cfd, ReadBuff, n);
		if(n < 0){
			close(cfd);
			vTaskDelete(NULL);			
		}
	}
}


/**
  * @brief  多线程服务器任务
  * @param  None
  * @retval None
  */
void vThreadServerTask(void){

	int 	 sfd, cfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	client_addr_len;
	
	//创建socket
	sfd = Socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port   			= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//绑定socket
	Bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//监听socket
	Listen(sfd, 5);
	//等待客户端连接
	client_addr_len = sizeof(client_addr);
	while(1){  //死循环，不让退出这个任务
			/*
				sockets.c:
				#define NUM_SOCKETS MEMP_NUM_NETCONN
				每创建socket lwip都会分配一片内存空间，在lwip内部有
				很多宏，NUM_SOCKETS就定义 一共支持多少个socket，也就是说
				能分配多少fd
				#define MEMP_NUM_NETCONN                8
			*/
			cfd = Accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
			printf("client is connect cfd = %d\r\n",cfd);
			if(xTaskCreate( (TaskFunction_t) vNewClientTask,
											"Client",
											128, //栈大小
											(void *)&cfd,
											osPriorityNormal,  //0
											NULL) != pdPASS){	
				printf("create task fail!\r\n");		

			}
	}
}
