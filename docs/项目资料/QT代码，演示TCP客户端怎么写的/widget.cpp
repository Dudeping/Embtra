#include "widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
 //   char buf[1024];
 //   setFixedSize(640,480);
    tcpsocket = new QTcpSocket(this);
    le = new QLineEdit(this);
    te = new QTextEdit(this);
    te->setMinimumSize(600,500);
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(le);
    vbox->addWidget(te);
    this->setLayout(vbox);

    connect(tcpsocket,SIGNAL(connected()),this,SLOT(tcp_connected()));
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(tcp_read()));
    qDebug()<<"create successssssssssss...";
    tcpsocket->connectToHost("127.0.0.1",6666);
    if(tcpsocket->waitForConnected(30000))
        qDebug()<<"connectToHost success...";
    else
        qDebug()<<"connectToHost fail...";
    connect(le,SIGNAL(editingFinished()),this,SLOT(display()));

}

Widget::~Widget()
{

}
void Widget::tcp_read()
{
    char buf[100],send[100]="you: ";
    int ret;
    memset(buf,0,sizeof(buf));
 //   memset(send,0,sizeof(send));
    ret = tcpsocket->read(buf,100);
    qDebug()<<"read ok: "<<buf<<ret;
    strcat(send,buf);
    te->append(send);
//    tcpsocket->write(buf,ret);
}
void Widget::display()
{
//    te->append("me:");
    int ret;
    te->append("me: "+le->text());
    ret = tcpsocket->write(le->text().toStdString().c_str());
    qDebug()<<"write bytes: "<<ret;
}
