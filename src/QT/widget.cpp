#include "widget.h"
#include "ui_widget.h"

char imageData[640*480];
quint64 lenth=0;
quint64 ret=0;
//int show=0,pos;
char rec[28];
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    image=new  QPixmap();

    tcpsocket_m0 = new QTcpSocket(this);
    tcpsocket_m0->connectToHost("192.168.0.10",8888);
    if(tcpsocket_m0->waitForConnected(30000))
        qDebug()<<"connectToHost success  m0...";
    else
        qDebug()<<"connectToHost fail  m0...";
    connect(tcpsocket_m0,SIGNAL(readyRead()),this,SLOT(tcp_read_m0()));

    tcpsocket_camera = new QTcpSocket(this);
    tcpsocket_camera->connectToHost("192.168.0.10",8888);
    if(tcpsocket_camera->waitForConnected(30000))
        qDebug()<<"connectToHost success  camera...";
    else
        qDebug()<<"connectToHost fail  camera...";
    connect(ui->pb2,SIGNAL(clicked()),this,SLOT(my_pb2_clicked()));

}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_pb1_clicked()
{
    struct data{
        char way_1[4];
        char way_2[32];
    }Netpak;
     memcpy(Netpak.way_1,"gmo",4);
     memcpy(Netpak.way_2,"0",32);
     tcpsocket_m0->write((char*)&Netpak,36);
}
void Widget::tcp_read_m0()
{
    //cn->waitForReadyRead();
    char dat[12]={0};
    int ret=0;

    ret=tcpsocket_m0->read(dat,12);
    if(ret==0)
    {
        return;
    }

    QByteArray arr=QByteArray(dat);

    ui->tem->setText(arr.mid(0,2));
    ui->wet->setText(arr.mid(2,2));
    ui->light->setText(arr.mid(4,3));
    qDebug()<<arr<<endl;
    arr.clear();

}

void Widget::on_pushButton_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"led_on",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}

void Widget::on_pushButton_2_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"led_off",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}

void Widget::on_pushButton_3_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"fei_on",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}

void Widget::on_pushButton_4_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"fei_off",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}

void Widget::on_pushButton_5_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"bee_on",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}

void Widget::on_pushButton_6_clicked()
{
    struct data{
      char way_1[4];
      char way_2[32];
    }Netpak;

    memcpy(Netpak.way_1,"cmo",4);
    memcpy(Netpak.way_2,"bee_off",32);
    tcpsocket_m0->write((char*)&Netpak,36);
    tcpsocket_m0->flush();
}



void Widget::my_pb2_clicked()
{
    qDebug()<<"enter on_pb2_clicked\n";
    ui->pb2->setDisabled(true);
    //发送命令
    struct data{
        char way_1[4];
        char way_2[32];
      }Netpak;
      memcpy(Netpak.way_1,"oca",4);
      memcpy(Netpak.way_2,"0",32);
      tcpsocket_camera->write((char*)&Netpak,36);
      tcpsocket_camera->flush();
      connect(tcpsocket_camera,SIGNAL(readyRead()),this,SLOT(tcp_read_cameraa()));

}
void Widget::tcp_read_cameraa()
{
    int rt=0;
  //  qDebug()<<"enter tcp_read_camera:"<<lenth;
    if(lenth==0)
     {
      while (1) {
          rt=tcpsocket_camera->read(rec,sizeof(rec));
          if(rt>0)
          {
              QByteArray arr=QByteArray(rec);

              if(arr.mid(0,3)=="len")
              {
                       lenth=atoi(arr.mid(3,5));
                       qDebug()<<lenth<<endl;
                         memset(rec,0,28);
              }
         //     qDebug()<<"rt############:"<<rt<<"    lenth:"<<lenth;
              break;
          }else
          {
       //       qDebug()<<"rt************:"<<rt;
             tcpsocket_camera->waitForBytesWritten();
              continue;
          }
      }
    }
    if(tcpsocket_camera->bytesAvailable() >= lenth)
    {
        ret=tcpsocket_camera->read(imageData,lenth);
        qDebug()<<"lenth:"<<lenth<<"  ret:"<<ret;
        image->loadFromData((const uchar *)imageData,lenth,"JPEG");
        ui->lb->setPixmap(*image);
        lenth = 0;
    }
 /*   if(lenth != 0)
    {
        quint64 total=0;
        pos = 0;
//        while(lenth>total)
//        {
          ret=tcpsocket_camera->read(imageData+pos,lenth-pos);
//         if(ret==0)
 //         {
 //             tcpsocket_camera->waitForReadyRead();
 //             continue;
//          }
          pos += ret;
          lenth -= ret;

 //        }
        if(lenth == 0)
        {
            pos = 0;
            image->loadFromData((const uchar *)imageData,lenth,"JPEG");
            ui->lb->setPixmap(*image);
        }

    }
*/
}
