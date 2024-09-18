#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  创建套接字
  * @param  domain: 	协议域
  * @param  type: 		协议类型 
  * @param  protocol: 协议版本
  * @retval int:			0
  */
int Socket(int domain, int type, int protocol)
{
	int fd;
	fd = socket(domain, type, protocol);
	//当返回值为-1的时候，基本是lwip的内存不够
	if(fd < 0){
		printf("create socket error\r\n");
		//当调用删除任务，就会切换上下文，CPU执行其他任务
		vTaskDelete(NULL);
	}
	return fd;
}


/**
  * @brief  绑定套接字
  * @param  sockfd: 	文件描述符
  * @param  addr: 		绑定的地址信息 
  * @param  addrlen: 	地址结构体长度
  * @retval int:			0
  */
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret;
	ret = bind(sockfd, addr, addrlen);
	if(ret < 0){
		printf("bind socket error\r\n");
		//当调用删除任务，就会切换上下文，CPU执行其他任务
		vTaskDelete(NULL);		
	}
	return ret;
}

/**
  * @brief  监听套接字
  * @param  sockfd: 	要监听的文件描述符
  * @param  backlog: 	监听队列的大小
  * @retval int:			0
  */
int Listen(int sockfd, int backlog)
{
	int ret;
	ret = listen(sockfd, backlog);
	if(ret < 0){
		printf("listen socket error\r\n");
		//当调用删除任务，就会切换上下文，CPU执行其他任务
		vTaskDelete(NULL);		
	}
	return ret;

}

/**
  * @brief  等待客户端建立好连接
  * @param  sockfd: 	文件描述符
  * @param  addr: 		绑定的地址信息 
  * @param  addrlen: 	地址结构体长度---指针
  * @retval int:			0
  */
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){

	int fd;
again:
	//accept 是阻塞函数，只有客户端连接成功后，才会返回，或者错误返回
	fd = accept(sockfd, addr, addrlen);
	//客戶端连接错误
	if(fd < 0){
		printf("accept socket error\r\n");
		goto again;
	
	}
	return fd;
}

/**
  * @brief  向目标服务器建立连接
  * @param  sockfd: 	文件描述符
  * @param  addr: 		绑定的地址信息 
  * @param  addrlen: 	地址结构体长度---指针
  * @retval int:			正确：0，错误小于0
  */
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret;
	ret = connect(sockfd, addr, addrlen);
	if(ret < 0){
		printf("connect socket error\r\n");
		//先关闭当前的socket，其实内部是删除这个socket的内存块
		close(sockfd);
	}
	return ret;
}
/**
  * @brief  向套接字发送数据
  * @param  fd: 	文件描述符
  * @param  buf: 		要发送的缓冲区 
  * @param  nbytes: 	发送数据的大小，单位为字节
  * @retval int:			正确：返回已经发生的数据长度，错误小于0
  */
int Write(int fd,const void *buf,size_t nbytes)
{
	int ret;
	ret = write(fd, buf, nbytes);
	//基本上是socket错误了，比如对方socket关闭了
	if(ret < 0){
		printf("Write socket error\r\n");
		//先关闭当前的socket，其实内部是删除这个socket的内存块
		close(fd);	
	}
	return ret;

}
/**
  * @brief  从套接字读取数据
  * @param  fd: 	文件描述符
  * @param  buf: 		要接收的缓冲区 
  * @param  nbytes: 	接收数据的大小，单位为字节
  * @retval int:			正确：返回已经接收的数据长度，错误小于0,socket关闭等于0
  */
int Read(int fd,void *buf,size_t nbyte)
{
	int ret;
	ret = read(fd, buf, nbyte);
	if(ret == 0){
		printf("read socket is close\r\n");
		close(fd);
	}else if(ret < 0){
	
		printf("read socket error\r\n");
		close(fd);
	}
	return ret;

}

/**
  * @brief  发送数据到指定地址
  * @param  sockfd: 文件描述符
  * @param  msg: 		要发送的缓冲区 
  * @param  len: 		要发送大小
  * @param  flags: 	标志 默认传0
  * @param  to: 		发送的地址信息
  * @param  tolen: 	地址结构体长度
  * @retval int:			正确：返回已经发送的数据长度，错误小于0
  */
int Sendto(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, int tolen)
{
	int ret;
again:
	ret = sendto(sockfd, msg, len, flags, to, tolen);
	if(ret < 0){
		printf("sendto socket error\r\n");
		goto again;
	}
	return ret;

}
/**
  * @brief  从socket接收数据
  * @param  sockfd: 	文件描述符
  * @param  buf: 			要接收的缓冲区 
  * @param  len: 			接收缓冲区的大小
  * @param  flags: 		标志 默认传0
  * @param  from: 		接收到的地址信息
  * @param  fromlen: 	地址结构体大小
  * @retval int:			正确：返回已经发送的数据长度，错误小于0
  */
int Recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, socklen_t *fromlen)
{
	int ret;
again:
	ret = recvfrom(sockfd, buf, len, flags, from, fromlen);
	if(ret < 0){
		printf("recvfrom socket error\r\n");
		goto again;
	
	}
	return ret;
}
