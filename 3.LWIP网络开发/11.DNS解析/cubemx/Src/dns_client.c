#include "dns_client.h"
#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ctype.h"  //toupper
#include "usart.h"
#include "netdb.h"  //gethostbyname

void vDnsClientTask(void){
	
	int i;
	//DNS������������
	struct hostent *p_hostent = NULL;
	
	p_hostent = gethostbyname("www.makeru.com.cn");  //ʧ�ܷ���null
	if(p_hostent){
		for(i=0; p_hostent->h_addr_list[i]; ++i){
			printf("host ip:%s\n", inet_ntoa(*p_hostent->h_addr_list[i]) );
		}
	}else{
		printf("get host ip is failed!\n");
	}

}
