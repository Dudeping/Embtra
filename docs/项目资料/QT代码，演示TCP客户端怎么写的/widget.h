#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QDebug>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
public slots:
    void tcp_connected(){
        qDebug()<<"connect success...";
    }
    void tcp_read();
    void display();
private:
    QTcpSocket *tcpsocket;
    QLineEdit *le;
    QTextEdit *te;
};

#endif // WIDGET_H
