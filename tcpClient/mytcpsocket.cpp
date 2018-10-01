#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket()
{
    command_socket = new QTcpSocket();
    file_socket = new QTcpSocket();
    ip = IP;
    port1 = PORT1;
    port2 = PORT2;

    connect(command_socket,SIGNAL(readyRead()),this,SLOT(on_read_command()));
    connect(file_socket,SIGNAL(readyRead()),this,SLOT(on_read_file()));
}

void MyTcpSocket::connect_to_server(){
    command_socket->connectToHost(QHostAddress(ip),port1);
    file_socket->connectToHost(QHostAddress(ip),port2);
}

void MyTcpSocket::set_file(QString filePath){
    if(filePath.isEmpty()){
        qDebug()<<"设置文件路径失败";
        return;
    }
    QFileInfo info(filepath);
    filename = info.fileName();
    filesize = info.size();
    sendSize = 0;

    file.setFileName(filepath);
    bool isOk = file.open(QIODevice::ReadOnly);
    if(false == isOk)
    {
        qDebug()<<ERROR_CODE_77;
    }
    return;
}

void MyTcpSocket::send_command(){
    QString head = QString("%1##%2##%3").arg(FILE_CODE).arg(filename).arg(filesize);
    qint64 len = command_socket->write(head.toUtf8());
    if(len <= 0){
        qDebug()<<ERROR_CODE_110;
        file.close();
    }
}

void MyTcpSocket::send_file(){
    qint64 len = 0;
    do
    {
        char buf[4*1024] = {0};
        len = file.read(buf,sizeof(buf));

        qDebug()<<file_socket->write(buf,len);

        sendSize += len;
        qDebug()<<len<<sendSize;
    }while(len > 0);

     if(sendSize == filesize)
     {
         qDebug("文件发送完毕");
         file.close();

         file_socket->disconnect();
         file_socket->close();
     }
}

void MyTcpSocket::on_read_command(){
    QByteArray buf = command_socket->readAll();
    qDebug()<<buf;

    int code = QString(buf).section("##",0,0).toInt();
    if(code == FILE_CODE){
        filename = QString(buf).section("##",1,1);
        filesize = QString(buf).section("##",2,2).toInt();
        recvSize = 0;

        file.setFileName(filename);
        bool isOk = file.open(QIODevice::WriteOnly);
        if(false == isOk)
        {
            qDebug()<<"writeonly error 49";
            command_socket->disconnectFromHost();
            command_socket->close();
            return;
        }
        qDebug()<<QString("文件名：%1\n大小:%2 kb").arg(filename).arg(filesize/1024);
    }
    if(code == COMMENT_CODE){
        send_file();
    }

}

void MyTcpSocket::on_read_file(){
    QByteArray buf = file_socket->readAll();
    qint64 len = file.write(buf);

    qDebug()<<recvSize;
    if(len > 0)
    {
       recvSize += len;
       qDebug()<< len;
    }

    ui->progressBar->setValue(recvSize/1024);

    if(recvSize == filesize)
    {
        file.close();
        //QMessageBox::information(this,"完成","文件接收完成");
        file_socket->disconnectFromHost();
        file_socket->close();
    }
}
