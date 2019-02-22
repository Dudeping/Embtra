#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "m0.h"


int uart_init()
{
	int fd;
	fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
	if(fd < 0) {
		perror("open!");
		exit(EXIT_FAILURE);
	}
//	int ret = serial_setup(fd,115200,8,0,'N');
	int ret = set_opt(fd,115200,8,'N',1);
	if(ret < 0) {
		printf("set error!\n");
	}
	return fd;
}
int m0_send(int fd,uint8_t cm)
{
	memset(sbuf,0,sizeof(sbuf));
	if(m0_pack(cm) < 0) {
		return -1;	
	}
	uint8_t *m0_command = sbuf;
	if(writen(fd,m0_command,sizeof(sbuf)) != sizeof(sbuf)) {
		return -1;
	}
	return 0;		
}

int m0_pack(uint8_t m0_cm)
{
/*
	uint8_t buf[] = {0xDD,0x08,0x24,0x00,m0_cm,0x00};
	strcpy(sbuf,buf);
*/

	sbuf[0] = 0xDD;
	sbuf[1] = 0x06;
	sbuf[2] = 0x24;
	sbuf[3] = 0x00;
	sbuf[4] = m0_cm;
	sbuf[5] = 0x00;
	return 0;
}

int *m0_recv(int fd)
{
	memset(rbuf,0,sizeof(rbuf));
	memset(s,0,sizeof(s));
	uint8_t *p = rbuf;
	if(readn(fd,p,sizeof(rbuf)) < 0) {
		printf("readn error!\n");
	}
	s[0] = p[5];
	s[1] = p[7];
	s[2] = (int)p[20] + (int)p[21]*100;
	return s;
}

size_t readn(int fd,uint8_t *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	uint8_t *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if(errno == EINTR)
				nread = 0;	/* and call read() again */
			else
				return(-1);
		} else if(nread == 0)
			break;		/* EOF */

		nleft -= nread;
		ptr += nread;
	}
	return(n - nleft);	/* return >= 0 */
}
/* end readn */

ssize_t writen(int fd, const uint8_t *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const uint8_t *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;        /* and call write() again */
			else
				return(-1);            /* error */
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return(n);
}

int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
	struct termios newtio,oldtio;
	if(tcgetattr(fd,&oldtio)!= 0)
	{
		perror("SetupSerial 1");
		return -1;
	}
	bzero(&newtio,sizeof(newtio));
	/*步骤一，设置字符大小*/
	newtio.c_cflag |= CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
	/*设置停止位*/
	switch(nBits)
	{
		case 7:
			newtio.c_cflag |=CS7;
			break;
		case 8:
			newtio.c_cflag |=CS8;
			break;
	}
	/*设置奇偶校验位*/
	switch(nEvent)
	{
		case 'O'://奇数
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |=PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E'://偶数
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
		case 'N'://无奇偶校验位
			newtio.c_cflag &= ~PARENB;
			break;
	}
	/*设置波特率*/
	switch(nSpeed)
	{
		case 2400:
			cfsetispeed(&newtio,B2400);
			cfsetospeed(&newtio,B2400);
			break;
		case 4800:
			cfsetispeed(&newtio,B4800);
			cfsetospeed(&newtio,B4800);
			break;
		case 9600:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;
		case 115200:
			cfsetispeed(&newtio,B115200);
			cfsetospeed(&newtio,B115200);
			break;
		case 460800:
			cfsetispeed(&newtio,B460800);
			cfsetospeed(&newtio,B460800);
			break;
		default:
			cfsetispeed(&newtio,B9600);
			cfsetospeed(&newtio,B9600);
			break;
	}
	/*设置停止位*/
	if(nStop==1)
		newtio.c_cflag &= ~CSTOPB;
	else if(nStop==2)
		newtio.c_cflag |= CSTOPB;
	/*设置等待时间和最小接收字符*/
	newtio.c_cc[VTIME] =40;
	newtio.c_cc[VMIN]=0;
	/*处理未接受字符*/
	tcflush(fd, TCIFLUSH);
	/*激活新配置*/
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	return 0;
} 
