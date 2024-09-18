#ifndef _SOCKET_WRAP_H
#define _SOCKET_WRAP_H
#include "sockets.h"

int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Write(int fd,const void *buf,size_t nbytes);
int Read(int fd,void *buf,size_t nbyte);
int Sendto(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, int tolen);
int Recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, socklen_t *fromlen);

#endif
