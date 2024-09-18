#include "socket_wrap.h"
#include "FreeRTOS.h"
#include "task.h"

/**
  * @brief  �����׽���
  * @param  domain: 	Э����
  * @param  type: 		Э������ 
  * @param  protocol: Э��汾
  * @retval int:			0
  */
int Socket(int domain, int type, int protocol)
{
	int fd;
	fd = socket(domain, type, protocol);
	//������ֵΪ-1��ʱ�򣬻�����lwip���ڴ治��
	if(fd < 0){
		printf("create socket error\r\n");
		//������ɾ�����񣬾ͻ��л������ģ�CPUִ����������
		vTaskDelete(NULL);
	}
	return fd;
}


/**
  * @brief  ���׽���
  * @param  sockfd: 	�ļ�������
  * @param  addr: 		�󶨵ĵ�ַ��Ϣ 
  * @param  addrlen: 	��ַ�ṹ�峤��
  * @retval int:			0
  */
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret;
	ret = bind(sockfd, addr, addrlen);
	if(ret < 0){
		printf("bind socket error\r\n");
		//������ɾ�����񣬾ͻ��л������ģ�CPUִ����������
		vTaskDelete(NULL);		
	}
	return ret;
}

/**
  * @brief  �����׽���
  * @param  sockfd: 	Ҫ�������ļ�������
  * @param  backlog: 	�������еĴ�С
  * @retval int:			0
  */
int Listen(int sockfd, int backlog)
{
	int ret;
	ret = listen(sockfd, backlog);
	if(ret < 0){
		printf("listen socket error\r\n");
		//������ɾ�����񣬾ͻ��л������ģ�CPUִ����������
		vTaskDelete(NULL);		
	}
	return ret;

}

/**
  * @brief  �ȴ��ͻ��˽���������
  * @param  sockfd: 	�ļ�������
  * @param  addr: 		�󶨵ĵ�ַ��Ϣ 
  * @param  addrlen: 	��ַ�ṹ�峤��---ָ��
  * @retval int:			0
  */
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){

	int fd;
again:
	//accept ������������ֻ�пͻ������ӳɹ��󣬲Ż᷵�أ����ߴ��󷵻�
	fd = accept(sockfd, addr, addrlen);
	//�͑������Ӵ���
	if(fd < 0){
		printf("accept socket error\r\n");
		goto again;
	
	}
	return fd;
}

/**
  * @brief  ��Ŀ���������������
  * @param  sockfd: 	�ļ�������
  * @param  addr: 		�󶨵ĵ�ַ��Ϣ 
  * @param  addrlen: 	��ַ�ṹ�峤��---ָ��
  * @retval int:			��ȷ��0������С��0
  */
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int ret;
	ret = connect(sockfd, addr, addrlen);
	if(ret < 0){
		printf("connect socket error\r\n");
		//�ȹرյ�ǰ��socket����ʵ�ڲ���ɾ�����socket���ڴ��
		close(sockfd);
	}
	return ret;
}
/**
  * @brief  ���׽��ַ�������
  * @param  fd: 	�ļ�������
  * @param  buf: 		Ҫ���͵Ļ����� 
  * @param  nbytes: 	�������ݵĴ�С����λΪ�ֽ�
  * @retval int:			��ȷ�������Ѿ����������ݳ��ȣ�����С��0
  */
int Write(int fd,const void *buf,size_t nbytes)
{
	int ret;
	ret = write(fd, buf, nbytes);
	//��������socket�����ˣ�����Է�socket�ر���
	if(ret < 0){
		printf("Write socket error\r\n");
		//�ȹرյ�ǰ��socket����ʵ�ڲ���ɾ�����socket���ڴ��
		close(fd);	
	}
	return ret;

}
/**
  * @brief  ���׽��ֶ�ȡ����
  * @param  fd: 	�ļ�������
  * @param  buf: 		Ҫ���յĻ����� 
  * @param  nbytes: 	�������ݵĴ�С����λΪ�ֽ�
  * @retval int:			��ȷ�������Ѿ����յ����ݳ��ȣ�����С��0,socket�رյ���0
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
  * @brief  �������ݵ�ָ����ַ
  * @param  sockfd: �ļ�������
  * @param  msg: 		Ҫ���͵Ļ����� 
  * @param  len: 		Ҫ���ʹ�С
  * @param  flags: 	��־ Ĭ�ϴ�0
  * @param  to: 		���͵ĵ�ַ��Ϣ
  * @param  tolen: 	��ַ�ṹ�峤��
  * @retval int:			��ȷ�������Ѿ����͵����ݳ��ȣ�����С��0
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
  * @brief  ��socket��������
  * @param  sockfd: 	�ļ�������
  * @param  buf: 			Ҫ���յĻ����� 
  * @param  len: 			���ջ������Ĵ�С
  * @param  flags: 		��־ Ĭ�ϴ�0
  * @param  from: 		���յ��ĵ�ַ��Ϣ
  * @param  fromlen: 	��ַ�ṹ���С
  * @retval int:			��ȷ�������Ѿ����͵����ݳ��ȣ�����С��0
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
