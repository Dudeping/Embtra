#ifndef _M0_H
#define _M0_H
#include <sys/types.h>
#include <unistd.h>

#define start_machine_t 0xAA //开机
#define data_flow_t 0xBB //数据采集
#define rfid_msg_t 0xCC //rfid 信息
#define command_tag_t 0xDD //命令
#define key_msg_t 0xEE //按键
#define other_type_t 0x00 //其它(未定义)

#define on_led1 0 //开 LED1 灯
#define off_led1 1 //关 LED1 灯
#define on_speaker 2 //开蜂鸣器
#define off_speaker 3 //关蜂鸣器
#define on_fan 4 //开风扇
#define on_fan_low 5 //使风扇工作在低速状 态
#define on_fan_mid 6 //使风扇工作在中速状 态
#define on_fan_high 7 //使风扇工作在高速状 态
#define off_fan 8 //关风扇
#define on_seven_led 9 //开数码管
#define off_seven_led 10 //关数码管
#define off_machine 11  //关机器

typedef unsigned char uint8_t;
typedef signed char int8_t;
//--------------------------------------
int uart_init();
int *m0_recv(int fd);
int m0_send(int fd,uint8_t cm);
//--------------------------------------


int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);
size_t readn(int fd,uint8_t *vptr,size_t n);
ssize_t writen(int fd,const uint8_t *vptr,size_t n);
int m0_pack(uint8_t m0_cm);


uint8_t rbuf[36];
uint8_t sbuf[36];
int s[3];
#endif

