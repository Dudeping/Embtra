#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sqlite3.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

#include "tcp.h"
#include "camera.h"
#include "m0.h"

#define CAMERA_DEV_PATH "/dev/video0"


void *thrd_ret;
pthread_mutex_t mutex;
pthread_t ID;
pthread_t thread;
int ffd;

typedef struct
{
	char name[16];
	char data[16];
}MSG;

typedef struct
{
	int wen;
	int shi;
	int guang;
}TH;

void do_register(int connfd, MSG *msg, sqlite3 *db);
void do_login(int connfd, MSG *msg, sqlite3 *db);
void *thread_camera();
void *thread_camera();
void *do_client(void *connfd);
void *thread_sev();
void *do_M0();
void *do_TH();
void *srv_do_camera();
void *do_kill_thread(pthread_t tid);
void *do_thread_camera();
void *do_led_on();
void *do_led_off();
void *do_fan_on();
void *do_fan_off();
void *do_speaker_on();
void *do_speaker_off();

/*********注册用户***********/
void do_register(int connfd, MSG *msg, sqlite3 *db)
{
	char sqlstr[128];
	char *errmsg;
	int rc;

	sprintf(sqlstr, "insert into usr values ('%s', '%s')", msg->name, msg->data); //封装SQL语句
	printf("%s\n", sqlstr);
	//执行SQL操作语句
	rc = sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg);
	if(rc != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		sprintf(msg->data, "%s exist!!!", msg->name);
		strncpy(msg->data, "regfault", 16);
	}
	else
	{
		strncpy(msg->data, "regsuc", 16); //操作成功
	}

	send(connfd, msg->data, 16, 0);
	return;
}

/*********登录用户************/
void do_login(int connfd, MSG *msg, sqlite3 *db)
{
	char sqlstr[128];
	char *errmsg, **result;
	int nrow, ncolumn;

	sprintf(sqlstr, "select * from usr where name = '%s' and pass = '%s'", msg->name, msg->data);
	if (sqlite3_get_table(db, sqlstr, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		sqlite3_free(errmsg);
	}

	if (nrow == 0)
	{
		strncpy(msg->data, "lfault", 16);
	}
	else
	{
		strncpy(msg->data, "success", 16);
	}

	send(connfd, msg->data, 16, 0);
	sqlite3_free_table(result);
	return ;
}

/*************************M0控制************************************/
void *do_M0(char *buf)
{
	while(1)
	{
		printf("%s\n",buf+4);
		if ( !strncmp(buf+4, "led_on", 32))
		{
			do_led_on();
		} else if ( !strncmp(buf+4, "led_off", 16))
		{
			do_led_off();
		} else if ( !strncmp(buf+4, "fei_on", 16))
		{
			do_fan_on();
		} else if ( !strncmp(buf+4, "fei_off", 16))
		{
			do_fan_off();
		} else if ( !strncmp(buf+4, "bee_on", 16))
		{
			do_speaker_on();
		} else if ( !strncmp(buf+4, "bee_off", 16))
		{
			do_speaker_off();
		}
		break;
	}
}

/***********************LED灯的开关***********************************/
void *do_led_on()
{
	printf("led_on\n");
	uint8_t  cm = 0;
	m0_send(ffd,cm);
	return;
}
void *do_led_off()
{
	printf("led_off\n");
	uint8_t cm = 1;
	m0_send(ffd,cm);
	return;
}

/*******************************风扇得开关****************************/
void *do_fan_on()
{
	uint8_t cm = 4;
	m0_send(ffd,cm);
	return;
}
void *do_fan_off()
{
	uint8_t cm = 8;
	m0_send(ffd,cm);
	return;
}

/********************************蜂鸣器开关*****************************/
void *do_speaker_on()
{
	uint8_t cm = 2;
	m0_send(ffd,cm);
}
void *do_speaker_off()
{
	uint8_t cm = 3;
	m0_send(ffd,cm);
}

/*****************************获取温湿度****************************/
void *do_TH(int connfd)
{
	int ret;
	char request[12];
//	printf("123456789\n");
	TH mm;
	int *q;
	q = m0_recv(ffd);
	mm.wen = q[0];
	mm.shi = q[1];
	mm.guang = q[2];
	printf("shujucaiji:%d,%d,%d\n",mm.wen,mm.shi,mm.guang);
	snprintf(request, sizeof(request), "%d%d%d", mm.wen,mm.shi,mm.guang);
	ret=send(connfd, request, 12, 0);
//		printf("%d\n", ret);
}

/************************打开视频********************************/
void *srv_do_camera(int arg)
{
	int connfd = arg;
	char request[28];
	int ret;
	ID=thread;

	while(1)
	{
		memset(request, 0, sizeof(request));
		pthread_mutex_lock(&mutex);
		snprintf(request, sizeof(request), "len%d", pic.len);
		printf("line:%d,%d\n", __LINE__,pic.len);
		ret = send(connfd, request, sizeof(request), 0);
		if(ret < 0)
			perror("send fail:");
//		signal(SIGPIPE, SIG_IGN);
		ret = send(connfd, pic.picbuf, pic.len, 0);
		if(ret < 0)
			perror("send fail:");
//		signal(SIGPIPE, SIG_IGN);
		//printf("line:%d,ret:%d\n", __LINE__,ret);
		pthread_mutex_unlock(&mutex);
/*		int fd = open("1.jpg",O_WRONLY|O_CREAT,0666);
		if(fd < 0)
			perror("open fail:");
		ret = write(fd,pic.picbuf,pic.len);
		if(ret < 0)
			perror("write fail:");
		close(fd);
*/		
		usleep(100000);
	}

}

/********************终止当前线程**********************************/
void *do_kill_thread(pthread_t tid)
{
	printf("tid %u\n",(unsigned int)tid);
	pthread_cancel(tid);
}

/***********摄像头线程*************/
void *thread_camera()
{
	open_device();//打开设备
	get_capability();//查看设备信息
	get_format() ;//get the format
	set_format() ;//set the format
	request_buf();// 申请内存；
	query_mmap_buf() ;//获得内存，实现内存映射；
	camera_capture() ;//视频采集
	while(1)
	{
	yuyv_to_rgb() ;//yuyv格式转为rgb格式
	rgb_to_jpeg() ;//rgb格式压缩为jpeg格式
	}
}

/************启动M0****************/
void *thread_M0()
{
	int fd;
	fd = uart_init();//调用这个函数之后会初始化串口，同时返回一个文件描述符
	if (fd < 0) {
		perror("m0 failure!");
		exit(EXIT_FAILURE);
	}
	ffd = fd;
}

#if 0
/******************创建摄像头线程************************/
void *do_thread_camera(int connfd)
{
	int ret;
	pthread_mutex_init(&mutex, NULL);
	ret = pthread_create(&thread, NULL, srv_do_camera, (void *) connfd);
	if (ret) {
		perror("create srv_do_camera thread");
		exit(EXIT_FAILURE);
	} else
		printf("create srv_do_camera thread success\n");

	ret = pthread_detach(thread);
	if(ret) {
		errno = ret;
		perror("detach srv_do_camera thread");
		exit(EXIT_FAILURE);
	} else
		printf("detach srv_do_camera thread success\n");

	pid_t pid;
	pthread_t tid;

	pid = getpid();
	printf("pid %u tid %u (0x%x)\n",(unsigned int)pid,(unsigned int)thread,(unsigned int)thread); //线程ID号 
	ID=thread;
}
#endif

/**************服务器与客户端连接****************/
void *thread_sev()
{
	int listenfd;
	int connfd;
	int ret;


	listenfd = tcp_server_init(NULL, "8888");
	if(listenfd == -1) 
		exit(EXIT_FAILURE);
	else
		fprintf(stdout, "init server success\n");
	
	while(1)
	{
		connfd = tcp_server_wait_connect(listenfd);
		
		ret = pthread_create(&thread, NULL, do_client,(void *)connfd);
		if (ret != 0) {
			printf("create thread failed\n");
			exit(ret);
		}

	}

	return;
}

/****************处理客户机发来得信息***************************/
void *do_client(void *arg)
{
	int connfd = (int) arg;
	MSG msg;
	int ret;
	sqlite3 *db;
	char buf[36];
	
/*	if (sqlite3_open("mylb.db", &db) != SQLITE_OK) {  //打开数据库文件
		printf("error : %s\n", sqlite3_errmsg(db));
		exit(-1);
	}
*/	memset(buf,0,sizeof(buf));
	while ((ret=recv(connfd, buf, 36, 0)) > 0)
	{
		printf("line:%d,%s,ret=%d\n",__LINE__,buf,ret);
		strncpy(msg.name, buf+4, 16);
		strncpy(msg.data, buf+20,16);
/*		if ( !strncmp(buf, "reg", 4)) 
		{
			do_register(connfd, &msg, db);   //注册
		} 
		else if ( !strncmp(buf, "lod", 4)) 
		{
			do_login(connfd, &msg, db); //登录
		}
		else*/
		if ( !strncmp(buf, "cmo", 4))
		{
			do_M0(buf);       //启动M0
		}
		else if ( !strncmp(buf, "gmo", 4))
		{
			printf("line:%d,cfd=%d\n",__LINE__,connfd);
			do_TH(connfd); //获取温湿度
		}
		else if ( !strncmp(buf, "oca", 4))
		{
			printf("line:%d,cfd=%d\n",__LINE__,connfd);
			srv_do_camera(connfd);
			//do_thread_camera(connfd); //打开摄像头
		}
		else if ( !strncmp(buf, "cca", 4))
		{
			do_kill_thread(ID);   //关闭摄像头
		}
		else
		{
			strcpy(buf, "cmd error!!");
		}
	}
	printf("client quit!\n");
	return;
}

int main(int argc, char *argv[])
{
	
	
/*****************启动摄像头*****************************************/
	int ret;

	pthread_mutex_init(&mutex, NULL);

	ret = pthread_create(&thread, NULL, thread_camera, NULL);
	if(ret) {
		errno = ret;
		perror("open camera failed");
		exit(EXIT_FAILURE);
	} else
		printf("open camera success\n");

	ret = pthread_detach(thread);
	if(ret) {
		errno = ret;
		perror("detach camera thread");
		exit(EXIT_FAILURE);
	} else
		printf("detach camera thread success\n");

/*********************启动M0**********************************/
	//pthread_mutex_init(&mutex, NULL);

	ret = pthread_create(&thread, NULL, thread_M0, NULL);
	if(ret) {
		errno = ret;
		perror("open M0 failed");
		exit(EXIT_FAILURE);
	} else
		printf("open M0 success\n");

	ret = pthread_detach(thread);
	if(ret) {
		errno = ret;
		perror("detach M0 thread");
		exit(EXIT_FAILURE);
	} else
		printf("detach M0 thread success\n");

/****************启动客户端**********************************/
	thread_sev();
	

	
	return 0;
}
