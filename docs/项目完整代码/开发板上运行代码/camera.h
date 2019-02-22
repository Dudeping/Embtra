#ifndef _CARMERA_H
#define _CARMERA_H
#define CAMERA_DEVICE "/dev/video0" 

#define CLEAR(x) memset (&(x), 0, sizeof (x))  
#define PIC_WIDTH 320//640
#define PIC_HEIGHT 240//480  
#define BUFFER_COUNT 10
#define QUALITY 75
#define JPEG_FILE "cam.jpeg"
#define RGB_FILE "cam.rgb"

#define YCbCrtoR(Y,Cb,Cr) (1000000*Y + 1370705*(Cr-128))/1000000

#define YCbCrtoG(Y,Cb,Cr) (1000000*Y - 337633*(Cb-128) - 698001*(Cr-128))/1000000

#define YCbCrtoB(Y,Cb,Cr) (1000000*Y + 1732446*(Cb-128))/1000000

#define PHSIZE 1024*50
struct PIC {
    unsigned long len;
    char picbuf[PHSIZE];
}pic;
void open_device();//打开设备
void get_capability();//查看设备信息
void get_format() ;//get the format
void set_format() ;//set the format 
int camera_capture() ;//视频采集
void request_buf();// 申请内存；
void query_mmap_buf() ;//获得内存，实现内存映射；
int yuyv_to_rgb() ;//yuyv格式转为rgb格式
int rgb_to_jpeg() ;//rgb格式压缩为jpeg格式
void video_save() ;//保存视频；
void uninit_device() ;//撤销内存映射
void camera_off() ; //关闭设备
#endif
