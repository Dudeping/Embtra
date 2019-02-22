#ifndef __TCP_H__
#define __TCP_H__

typedef int ssize_t;


int tcp_server_init(const char *ip, const char *port);
int tcp_server_wait_connect(int sockfd);
ssize_t tcp_server_recv(int connfd, void *buf, ssize_t count);
ssize_t tcp_server_send(int connfd, const void *buf, ssize_t count);
int tcp_server_disconnect(int connfd);
int tcp_server_exit(int sockfd);


#endif
