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
	// ÿһ�����񣬶��ж�����ջ�ռ�
	int cfd = * (int *)argument;
	int n, i;
	while(1){
		//�ȴ��ͻ��˷�������
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if(n <= 0){
			close(cfd);
			vTaskDelete(NULL);
		}
		//���д�Сдת��
		for(i = 0; i < n; i++){
		
			ReadBuff[i] = toupper(ReadBuff[i]);		
		}
		//д�ؿͻ���
		n = Write(cfd, ReadBuff, n);
		if(n < 0){
			close(cfd);
			vTaskDelete(NULL);			
		}
	}
}


/**
  * @brief  ���̷߳���������
  * @param  None
  * @retval None
  */
void vThreadServerTask(void){

	int 	 sfd, cfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	client_addr_len;
	
	//����socket
	sfd = Socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port   			= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��socket
	Bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//����socket
	Listen(sfd, 5);
	//�ȴ��ͻ�������
	client_addr_len = sizeof(client_addr);
	while(1){  //��ѭ���������˳��������
			/*
				sockets.c:
				#define NUM_SOCKETS MEMP_NUM_NETCONN
				ÿ����socket lwip�������һƬ�ڴ�ռ䣬��lwip�ڲ���
				�ܶ�꣬NUM_SOCKETS�Ͷ��� һ��֧�ֶ��ٸ�socket��Ҳ����˵
				�ܷ������fd
				#define MEMP_NUM_NETCONN                8
			*/
			cfd = Accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
			printf("client is connect cfd = %d\r\n",cfd);
			if(xTaskCreate( (TaskFunction_t) vNewClientTask,
											"Client",
											128, //ջ��С
											(void *)&cfd,
											osPriorityNormal,  //0
											NULL) != pdPASS){	
				printf("create task fail!\r\n");		

			}
	}
}
