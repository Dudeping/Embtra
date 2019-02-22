#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int wendu = 66;
int getfilesize(char filename[])
{
	struct stat st;
	stat(filename,&st);
	printf("%lu\n",st.st_size);
	return st.st_size;
}
//处理http的请求
int http_deal(int cfd)
{
	char recvbuf[1024],sendbuf[10240];
	unsigned int filelen;
	//read message
	bzero(recvbuf,sizeof(recvbuf));
	read(cfd,recvbuf,1024);
	//deal messageGET / HTTP/1.1
	if(strstr(recvbuf,"GET / HTTP/1.1") != NULL)
	{//封装http协议头以及数据到sendbuf

		bzero(sendbuf,sizeof(sendbuf));
		strcpy(sendbuf,"HTTP/1.1 200 OK\n");//封装状态行
		//封装响应头标
		strcat(sendbuf,"Content-Type: text/html\n");
		strcat(sendbuf,"Content-Length: ");
		filelen = getfilesize("./html/index.html");
		sprintf(sendbuf+strlen(sendbuf),"%d\n\n",filelen);
		//封装响应数据，也就是文件的内容
		int fd = open("./html/index.html",O_RDONLY);
		read(fd,sendbuf+strlen(sendbuf),filelen);
	
		printf("%s\n",sendbuf);

		//发送消息	
		write(cfd,sendbuf,10240);
		
	}
	//获取温度POST /html/index.html?temp
	if(strstr(recvbuf,"POST /html/index.html?temp") != NULL)
	{//封装http协议头以及数据到sendbuf
		bzero(sendbuf,sizeof(sendbuf));
		strcpy(sendbuf,"HTTP/1.1 200 OK\n");//封装状态行
		//封装响应头标
		strcat(sendbuf,"Content-Type: text/html\n");
		
		strcat(sendbuf,"Content-Length: ");
		sprintf(sendbuf+strlen(sendbuf),"%d\n\n",sizeof(int));
		//封装响应数据，也就是温度
		sprintf(sendbuf+strlen(sendbuf),"%d",wendu);
	
		printf("%s\n",sendbuf);

		//发送消息	
		write(cfd,sendbuf,10240);
	}
	return 0;		
}

int main()
{
	int ret;
	//创建套结字
	int lfd = socket(AF_INET,SOCK_STREAM,0);//AF_INET==PF_INET
	if(lfd < 0)
	{
		perror("socket fail:");
		return -1;
	}
	//封装一个套结字地址结构，本地的IP地址和端口
	struct sockaddr_in saddr,caddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(7777);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//绑定：将套结字和套结字地址结构进行绑定
	ret = bind(lfd,(struct sockaddr*)&saddr,sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		perror("bind fail:");
		return -2;
	}
	//将套结字变成监听套结字
	listen(lfd,5);
	int cfd;
	char recvmsg[1024],sendbuf[2048];
	int addrlen;
	//循环接受客户端的请求，然后处理请求，并将处理的结果返回给客户端
	while(1)
	{
		addrlen =sizeof(caddr);
		//取回一个客户端的链接，返回一个链接套结字
		cfd = accept(lfd,(struct sockaddr*)&caddr,&addrlen);
		if(cfd < 0)
		{
			perror("accept fail:");
			return -2;
		}
		printf("client is: %s:%d\n",inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port));

		http_deal(cfd);	
		
		close(cfd);
	}
	close(lfd);//发起两次挥手
	return 0;
}
