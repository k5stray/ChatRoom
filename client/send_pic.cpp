#include "send_pic.h"
#include "message_head.h"
#include <QFile>
#include <QFileInfo>
#include <QTcpSocket>


Send_Pic::Send_Pic(QObject *parent) : QObject(parent)
{

}

void Send_Pic::send_pic(qintptr cfd, QString filepath, const char* m_name)
{
    QString filename = filepath.mid(filepath.lastIndexOf('/')+1);
    QTcpSocket *tcp = new QTcpSocket(this);
    tcp->setSocketDescriptor(cfd);
    QFile file(filepath);
    QFileInfo info(filepath);
    int fileSize = info.size();
    file.open(QFile::ReadOnly);
    message_head curr_head;
    curr_head.m_type = message_type::m_picture;
    strcpy(curr_head.name, m_name);
    strcpy(curr_head.filename, filename.toStdString().c_str());
    curr_head.size = fileSize;
    tcp->write((char*)&curr_head, sizeof(curr_head));

    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        tcp->write(line);
    }
}
