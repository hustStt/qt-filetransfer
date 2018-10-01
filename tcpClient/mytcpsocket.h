#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "config.h"

class MyTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    void connect_to_server();
    void send_command();
    void send_file();
    void set_file(QString filePath);

private:
    QTcpSocket *command_socket;
    QTcpSocket *file_socket;

    QFile file;
    QString filename;
    qint64 filesize;
    qint64 recvSize;
    qint64 sendSize;
    QString ip;
    quint16 port1;
    quint16 port2;
private slots:
    void on_read_command();
    void on_read_file();

};

#endif // MYTCPSOCKET_H
