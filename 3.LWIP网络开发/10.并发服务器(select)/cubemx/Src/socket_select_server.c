#include "socket_select_server.h"
#include "socket_wrap.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ctype.h"

static char ReadBuff[BUFF_SIZE];
/**
  * @brief  select ����������
  * @param  none
  * @retval none
  */
void vSelectServerTask(void)
{

	int 	 sfd, cfd, maxfd, i, nready, n, j;
	struct sockaddr_in server_addr, client_addr;
	socklen_t	client_addr_len;
	fd_set all_set, read_set;
	//FD_SETSIZE��������˷�������fd
	int		 clientfds[FD_SETSIZE - 1];
	
	//����socket
	sfd = Socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family 			= AF_INET;
	server_addr.sin_port   			= htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��socket
	Bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//����socket
	Listen(sfd, 5);
	
	client_addr_len = sizeof(client_addr);
	//��ʼ�� maxfd ���� sfd
	maxfd = sfd;
	
	//���fdset
	FD_ZERO(&all_set);
	
	
	//��sfd�ļ���������ӵ�������
	
	FD_SET(sfd, &all_set);
	
	
	//��ʼ���ͻ���fd�ļ���
	
	for(i = 0; i < FD_SETSIZE -1 ; i++){
		//��ʼ��Ϊ-1
		clientfds[i] = -1;
	}
	while(1)
	{
		//ÿ��select����֮��fd_set���Ͼͻ�仯����selectʱ���Ͳ���ʹ�ã�
		//��������Ҫ��������fd_set �� ��ȡ��fd_set
		read_set = all_set;
		nready = select(maxfd + 1, &read_set, NULL, NULL, NULL);
		//û�г�ʱ���ƣ����᷵��0
		if(nready < 0){
			printf("select error \r\n");
			vTaskDelete(NULL);
		}
		//�жϼ������׽����Ƿ�������
		if(FD_ISSET(sfd, &read_set)){
		
			//���˿ͻ��˽���������
			//�ͻ�����
			cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_addr_len);
			if(cfd < 0){
				printf("accept socket error\r\n");
				//����select
				continue;
			}
			printf("new client connect fd = %d\r\n", cfd);
			//���µ�cfd ��ӵ�fd_set������
			FD_SET(cfd, &all_set);
			//����Ҫselect��maxfd
			maxfd = (cfd > maxfd)?cfd:maxfd;
			//���µ�cfd ���浽cfds������
			for(i = 0; i < FD_SETSIZE -1 ; i++){
				if(clientfds[i] == -1){
					clientfds[i] = cfd;
					//�˳�������Ҫ���
					break;
				
				}
			}
			//û�������׽�����Ҫ���������ֹ�ظ��������Ͳ�ȥִ����������
			if(--nready == 0){
				//����select
				continue;
			}
		
		}
		//�������еĿͻ����ļ�������
		for(i = 0; i < FD_SETSIZE -1 ; i++){
			if(clientfds[i] == -1){
				//��������
				continue;
			}
			//�Ƿ�������fd_set��������
			if(FD_ISSET(clientfds[i], &read_set)){
				n = Read(clientfds[i], ReadBuff, BUFF_SIZE);
				//Read�����Ѿ��ر�������ͻ��˵�fd
				if(n <= 0){
					//�Ӽ����������
					FD_CLR(clientfds[i], &all_set);
					//��ǰ�Ŀͻ���fd ��ֵΪ-1
					clientfds[i] = -1;
				}else{
					//���д�Сдת��
					for(j = 0; j < n; j++){
					
						ReadBuff[j] = toupper(ReadBuff[j]);		
					}
					//д�ؿͻ���
					n = Write(clientfds[i], ReadBuff, n);
					if(n < 0){
						//�Ӽ����������
						FD_CLR(clientfds[i], &all_set);
						//��ǰ�Ŀͻ���fd ��ֵΪ-1
						clientfds[i] = -1;		
					}				
				}
			}

		}		
	}
}
