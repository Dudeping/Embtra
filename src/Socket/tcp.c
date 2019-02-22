#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>


#define BACKLOG 1024

/***********初始化************/

int tcp_server_init(const char *ip, const char *port)
{
	/*创建socket*/
	int ret;
	int opt = 1;
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (-1 == sockfd) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	printf("socket success!\n");

	/*绑定本机地址和端口*/
	struct sockaddr_in myaddr; //internal 协议地址结构
	bzero(&myaddr, sizeof(myaddr));
	myaddr.sin_family = PF_INET; //地址族
	
	if (ip != NULL) {
		ret = inet_pton(AF_INET, ip, &myaddr.sin_addr);
		if (ret != 1) {
			fprintf(stderr, "server->ip: ip is error\n");
			return -1;
		}
	} else
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);//地址

	if (port != NULL)
		myaddr.sin_port = htons(atoi(port));//端口号
	else {
		fprintf(stderr, "server->port: port must be assigned\n");
		return -1;
	}
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	if (0 > bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr))) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	printf("bind success!\n");

	/*监听*/
	if(0 > listen(sockfd, BACKLOG)) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

/********************接受客户端**********************/
int tcp_server_wait_connect(int sockfd)
{
	int connfd;
	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	int addrlen = sizeof(cliaddr);

	connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &addrlen);
	if (-1 == connfd) {
		perror("accpet");
		exit(EXIT_FAILURE);
	}
	printf("accept success!\n");
	printf("accept: %s\n", inet_ntoa(cliaddr.sin_addr));

	return connfd;
}

/********************读数据**************************/
ssize_t tcp_server_recv(int connfd, void *buf, size_t count)
{
	ssize_t ret;

	assert(buf != NULL);//出错处理

	ret = read(connfd, buf, count);
	if (ret < 0) {
		perror("server->read");
		return -1;
	} else if (ret == 0) {
		fprintf(stderr, "server->read: end-of-file\n");
		return 0;
	} else 
	return ret;
}

/***************************写数据*********************************/
ssize_t tcp_server_send(int connfd, const void *buf, size_t count)
{
	ssize_t ret;

	assert(buf != NULL);

	ret = write(connfd, buf, count);
	if (ret == -1) {
		perror("server->read");
		return -1;
	} else 
	return ret;
}

/****************************带标志为的读数据*************************/
ssize_t tcp_server_recv_exact_nbytes(int connfd, void *buf, size_t count)
{
	ssize_t ret;
	ssize_t total = 0;

	assert(buf != NULL);

	while (total != count) {
		ret = read(connfd, buf + total, count - total);
		if (ret == -1) {
			perror("server->read");
			return -1;
		} else if (ret == 0) {
			fprintf(stderr, "server->read: end-of-file\n");
			return total;
		} else
			total += ret;
	}

	return total;
}

/*********************带标志位得写数据******************************/
ssize_t tcp_server_send_exact_nbytes(int connfd, const void *buf,size_t count)
{
	ssize_t ret;
	ssize_t total = 0;

	assert(buf != NULL);

	while (total != count) {
		ret = write(connfd, buf + total, count - total);
		printf("ret1 = %d\n", ret);
		if (ret == -1) {
			perror("server->write");
			return total;
		} else
			total += ret;
	}
	return total;
}

/*********************关闭套接字***********************************/
int tcp_server_disconnect(int connfd)
{
	if (close(connfd)) {
		perror("server->close");
		return -1;
	}
	return 0;
}

int tcp_server_exit(int sockfd)
{
	if (close(sockfd)) {
		perror("server->close");
		return -1;
	}
	return 0;
}
