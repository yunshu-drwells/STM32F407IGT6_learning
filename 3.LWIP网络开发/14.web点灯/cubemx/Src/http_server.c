#include "http_server.h"
#include "socket_wrap.h"  //使用自己封装的socket接口
#include "usart.h"

static char ReadBuff[BUFF_SIZE];
char sendBuffer[128];

char* htmlPage  = 
"<html>"
"<head>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\" />"
"<title>web 点灯</title>"
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
"<div class=\"text\" style=\" text-align:center;\"><big>Web 点灯</big></div>"
"</b>"
"</font>"
"<br> </br>     "
"<div align=\"center\" id=\"txtState\"style=\"margin:auto;width:160px;height:160px;border-radius:50%;background:white;\"></div>"
"<br> </br>"
"<div style=\" text-align:center;\">"
"<input type=\"button\" value=\"打开\" style=\"width:160px;height:80px;background:green;\" onclick=\"ledSwitch('on')\" />"
"<input type=\"button\" value=\"关闭\" style=\"width:160px;height:80px;background:red;\" onclick=\"ledSwitch('off')\" />"
"</div>"
"</body>"
"</html>";

//要使用sd卡或者fatfs文件系统
// 定义一个函数来读取文件内容并返回一个字符串
char* readFileToString(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        return NULL;
    }

    // 移动文件指针到文件末尾
    fseek(file, 0, SEEK_END);
    // 获取文件大小
    long fileSize = ftell(file);
    // 将文件指针重新移动到文件开头
    fseek(file, 0, SEEK_SET);

    // 为文件内容分配内存
    char *content = (char*)malloc(fileSize + 1);
    if (content == NULL) {
        printf("内存分配失败\n");
        fclose(file);
        return NULL;
    }

    // 读取文件内容到内存中
    fread(content, 1, fileSize, file);
    content[fileSize] = '\0'; // 确保字符串以空字符结尾

    fclose(file);
    return content;
}


void HttpParResponse(int cfd, char* buff){
	//是否请求网页
	if(strstr(buff, "GET /index.html HTTP/1.1") != NULL){
		//响应头
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应首
		sprintf(sendBuffer, "Content-Type: text/html\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen(htmlPage));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应体
		Write(cfd, htmlPage, strlen(htmlPage));
	
	}else if(strstr(buff, "GET /on HTTP/1.1") != NULL){ //是否打开LED
		/*
		HTTP/1.1 200 OK
		Server: EasyWebServer/1.9
		Content-Type: application/octet-stream
		Last-Modified: Thu, 20 Jun 2024 16:24:04 GMT
		Connection: Keep-Alive
		Content-Length: 4
		*/
		//响应头
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应首
		sprintf(sendBuffer, "Content-Type: application/octet-stream\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen("red"));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应体
		Write(cfd, "red", strlen("red"));
		//执行点灯操作
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
		
	}else if(strstr(buff, "GET /off HTTP/1.1") != NULL){ //是否关闭LED
		/*
		HTTP/1.1 200 OK
		Server: EasyWebServer/1.9
		Content-Type: application/octet-stream
		Last-Modified: Thu, 20 Jun 2024 16:25:09 GMT
		Connection: Keep-Alive
		Content-Length: 5
		*/
		//响应头
		sprintf(sendBuffer, "HTTP/1.1 200 OK\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应首
		sprintf(sendBuffer, "Content-Type: application/octet-stream\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Connection: Keep-Alive\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		sprintf(sendBuffer, "Content-Length: %d\r\n", strlen("white"));
		Write(cfd, sendBuffer, strlen(sendBuffer));
		
		sprintf(sendBuffer, "\r\n");
		Write(cfd, sendBuffer, strlen(sendBuffer));
		//响应体
		Write(cfd, "white", strlen("white"));
		//执行灭灯操作
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
	}else{ //请求资源无效，404
		printf("GET method Error!\n");
		close(cfd);
	}
}

/**
  * @brief  HTTP服务器任务
  * @param  None
  * @retval None
  */
void vHttpServerTask(void){
	int sfd, cfd, n;
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
		//解析HTTP 并响应
		HttpParResponse(cfd, ReadBuff);
		//http响应之后关闭fd
		close(cfd);
		goto again;
	}
}
