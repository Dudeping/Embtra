#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
//创建一个简单的TCP服务器

int main()
{
	//创建套结字
	int lfd = socket(PF_INET,SOCK_STREAM,0);
	if(lfd < 0)
	{
		perror("socket fail:");
		return -1;
	}
	//封装一个套结字地址结构：IP地址，端口
	struct sockaddr_in saddr;
	saddr.sin_family = PF_INET;
	saddr.sin_port = htons(6000);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//绑定：将套结字和套结字地址结构进行绑定
	int ret = bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));
	if(ret < 0)
	{
		perror("bind fail:");
		return -1;
	}
	//把普通套结字变成监听套结字
	listen(lfd,5);

	int cfd;
	char buf[1024];
	while(1)
	{
		//接受客户端的一个连接，返回一个连接套结字	
		cfd = accept(lfd,NULL,NULL);
		if(cfd < 0)
		{
			perror("accept fail:");
			return -1;
		}
		//接收客户端发给我的消息
		bzero(buf,sizeof(buf));
		ret = read(cfd,buf,1024);

		//处理消息
		printf("message:%s\n",buf);
		//给客户端一个处理结果的回应	
		write(cfd,buf,ret);
		close(cfd);
	}
	//关闭套结字

	return 0;
}

