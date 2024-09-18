#include "http_server.h"
#include "socket_wrap.h"  //ʹ���Լ���װ��socket�ӿ�
#include "usart.h"

static char ReadBuff[BUFF_SIZE];
char sendBuffer[128];

char* htmlPage  = 
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" />"
"<title>web ���</title>"
"<script defer=\"defer\">"
"        function ledSwitch(string) {"
"            var xmlhttp;"
"            if (window.XMLHttpRequest) {"
"                xmlhttp = new XMLHttpRequest();"
"            } else {"
"                xmlhttp = new ActiveXObject(\"Microsoft.XMLHTTP\");"
"            }"
"            xmlhttp.onreadystatechange = function () {"
"                    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {"
"                        document.getElementById(\"txtState\").style.backgroundColor = xmlhttp.responseText;"
"						console.log(xmlhttp.responseText);"
"					}"
"                },"
"            xmlhttp.open(\"GET\", string, true);"
"            xmlhttp.send(); "
"        }"
"    </script>"
"</head>"
""
"<body style=\"background-color:black\">"
"<font size=\"12\" color=\"yellow\">"
"<b>"
"<div class=\"text\" style=\" text-align:center;\"><big>Web ���</big></div>"
"</b>"
"</font>"
"<br> </br>     "
"<div align=\"center\" id=\"txtState\"style=\"margin:auto;width:160px;height:160px;border-radius:50%;background:white;\"></div>"
"<br> </br>"
"<div style=\" text-align:center;\">"
"<input type=\"button\" value=\"��\" style=\"width:160px;height:80px;background:green;\" onclick=\"ledSwitch('on')\" />"
"<input type=\"button\" value=\"�ر�\" style=\"width:160px;height:80px;background:red;\" onclick=\"ledSwitch('off')\" />"
"</div>"
"</body>"
"</html>";

//Ҫʹ��sd������fatfs�ļ�ϵͳ
// ����һ����������ȡ�ļ����ݲ�����һ���ַ���
char* readFileToString(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return NULL;
    }

    // �ƶ��ļ�ָ�뵽�ļ�ĩβ
    fseek(file, 0, SEEK_END);
    // ��ȡ�ļ���С
    long fileSize = ftell(file);
    // ���ļ�ָ�������ƶ����ļ���ͷ
    fseek(file, 0, SEEK_SET);

    // Ϊ�ļ����ݷ����ڴ�
    char *content = (char*)malloc(fileSize + 1);
    if (content == NULL) {
        printf("�ڴ����ʧ��\n");
        fclose(file);
        return NULL;
    }

    // ��ȡ�ļ����ݵ��ڴ���
    fread(content, 1, fileSize, file);
    content[fileSize] = '\0'; // ȷ���ַ����Կ��ַ���β

    fclose(file);
    return content;
}


void HttpParResponse(int cfd, char* buff){
	//�Ƿ�������ҳ
	if(strstr(buff, "GET /index.html HTTP/1.1") != NULL){
		//��Ӧͷ
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		sprintf(sendBuffer, "Content-Type: text/html\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen(htmlPage));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		Write(cfd, htmlPage, strlen(htmlPage));
	
	}else if(strstr(buff, "GET /on HTTP/1.1") != NULL){ //�Ƿ��LED
		/*
		HTTP/1.1 200 OK
		Server: EasyWebServer/1.9
		Content-Type: application/octet-stream
		Last-Modified: Thu, 20 Jun 2024 16:24:04 GMT
		Connection: Keep-Alive
		Content-Length: 4
		*/
		//��Ӧͷ
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		sprintf(sendBuffer, "Content-Type: application/octet-stream\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen("red"));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		Write(cfd, "red", strlen("red"));
		//ִ�е�Ʋ���
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		
	}else if(strstr(buff, "GET /off HTTP/1.1") != NULL){ //�Ƿ�ر�LED
		/*
		HTTP/1.1 200 OK
		Server: EasyWebServer/1.9
		Content-Type: application/octet-stream
		Last-Modified: Thu, 20 Jun 2024 16:25:09 GMT
		Connection: Keep-Alive
		Content-Length: 5
		*/
		//��Ӧͷ
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		sprintf(sendBuffer, "Content-Type: application/octet-stream\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen("white"));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//��Ӧ��
		Write(cfd, "white", strlen("white"));
		//ִ����Ʋ���
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
	}else{ //������Դ��Ч��404
		printf("GET method Error!\n");
		close(cfd);
	}
}

/**
  * @brief  HTTP����������
  * @param  None
  * @retval None
  */
void vHttpServerTask(void){
	int sfd, cfd, n;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len;
	//����Socket ����ʧ�ܾͻ�ɾ������
	sfd = Socket(AF_INET, SOCK_STREAM, 0);  //ipv4���ֽ�����Ĭ��Э��
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//��ip
	Bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	//����
	Listen(sfd, 5);  //���5���ͻ���
	client_addr_len = sizeof(client_addr);
again:
	//����accept
	cfd = Accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);
	printf("client is connect cfd = %d\n", cfd);
	
	while(1){
		//�ȴ��ͻ��˷�������
		n = Read(cfd, ReadBuff, BUFF_SIZE);
		if( n<= 0){  //������ر��ļ�������
			goto again;
		}
		//����HTTP ����Ӧ
		HttpParResponse(cfd, ReadBuff);
		//http��Ӧ֮��ر�fd
		close(cfd);
		goto again;
	}
}
