#include <QThread>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include "send_pic.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//#define MULT_THREAD
#define PORT 9527

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    this->setWindowIcon(QIcon(":/icon.ico"));

    //登录界面
    login = new login_win;

    //文件接收界面
    recv = new recv_file;

    setWindowTitle("聊天窗口");

    m_tcp = new QTcpSocket;

    //子线程处理图片上传
    QThread *t = new QThread;
    Send_Pic *worker = new Send_Pic;
    worker->moveToThread(t);
    connect(this, &MainWindow::send_pic, worker, &Send_Pic::send_pic);
    t->start();

    qRegisterMetaType<qintptr>("qintptr");

    message_head curr_head;
    connect(m_tcp, &QTcpSocket::readyRead, this, [=](){

        m_tcp->read((char*)&curr_head, sizeof(curr_head));
        connect_num->setText(QString::number((int)curr_head.connect_num));
        strcpy(m_peer_name, curr_head.name);
        QString tmp = "";
        if(curr_head.m_type == message_type::m_mesage) {
            QByteArray data = m_tcp->readAll();
            int total = curr_head.size - data.size();
            while(total > 0) {
                if(m_tcp->waitForReadyRead()) {
                    QByteArray tp = m_tcp->readAll();
                    data.append(tp);
                    total -= tp.size();
                }
            }

            tmp = pack_message(QString(m_peer_name), data, false);
        } else if(curr_head.m_type == message_type::m_picture) {
            QFile *file = new QFile(curr_head.filename);
            file->open(QFile::WriteOnly);
            int total = curr_head.size;
            while(total > 0) {
                if(m_tcp->waitForReadyRead()) {
                    QByteArray n = m_tcp->readAll();
                    file->write(n, n.size());
                    total -= n.size();
                }
            }
            file->close();

            tmp = pack_pic(QString(m_peer_name), curr_head.filename, false);
        }  else if(curr_head.m_type == message_type::m_status) {
            connect_num->setText(QString::number(curr_head.connect_num));
        } else if(curr_head.m_type == message_type::m_file) {
            recv->init();
            recv->setFileName(curr_head.filename);
            recv->setOwn(curr_head.name);
            recv->setFileSize((float)curr_head.size / 1024 / 1024);
            int total = curr_head.size;
            QByteArray data;
            while(total > 0) {
                if(m_tcp->waitForReadyRead()) {
                    QByteArray n = m_tcp->read(total);
                    data.append(n);
                    total -= n.size();
                }
            }
            tmp = pack_pre_file(curr_head.name, false, curr_head.filename, curr_head.size);
            ui->record->append(tmp);
            if(recv->exec() == QDialog::Accepted) {
                QFile *file = new QFile(curr_head.filename);
                file->open(QFile::WriteOnly);
                file->write(data);
                file->close();
                tmp = pack_file_mes(curr_head.filename, "接收完成");
            } else {             
                tmp = pack_file_mes(curr_head.filename, "取消接收");
            }
        }

        if(curr_head.m_type != message_type::m_status) {
            ui->record->append(tmp);
        }
    });

    connect(m_tcp, &QTcpSocket::connected, this, [=](){

        m_status->setPixmap(QPixmap(":/connect.jpeg").scaled(20, 20));
        ui->record->append(QString(m_name) + " 已经成功连接到服务器...........\n");

    });

    connect(m_tcp, &QTcpSocket::disconnected, this, [=](){
        m_tcp->close();
        m_tcp->deleteLater();
        m_status->setPixmap(QPixmap(":/disconnect.jpeg").scaled(20, 20));
        ui->record->append("已经成功断开服务器连接...........\n");

        t->exit();
        t->deleteLater();
        worker->deleteLater();
    });


    //连接状态设置
    m_status = new QLabel;
    m_status->setPixmap(QPixmap("://disconnect.jpeg").scaled(20, 20));
    connect_num = new QLabel;
    connect_num->setText("0");
    ui->statusBar->addWidget(new QLabel("连接状态："));
    ui->statusBar->addWidget(m_status);
    ui->statusBar->addWidget(new QLabel("              在线人数("));
    ui->statusBar->addWidget(connect_num);
    ui->statusBar->addWidget(new QLabel(")"));

    ui->picture->setIcon(QIcon(":/picture.ico"));
    ui->file->setIcon(QIcon(":/file.ico"));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_sendMassage_clicked()
{
    QString msg = ui->message->text();
    if(msg == "") {
        return;
    }

    message_head curr_head;

    strcpy(curr_head.name, m_name);
    curr_head.m_type = message_type::m_mesage;
    curr_head.size = msg.size();
    m_tcp->write((char*)&curr_head, sizeof(curr_head));

    m_tcp->write(msg.toUtf8());

    QString tmp = pack_message(m_name, msg, true);

    ui->record->append(tmp);
    ui->message->clear();
}


QString MainWindow::pack_message(QString name, QString mes, bool isMe)
{
    QString align = "", color = "", res = "";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString cdt = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    if(isMe) {
        align = "right";
        color = "aquamarine";
        res = "<div align=\"right\"  position=\"absolute\"><table><tr><td>" + cdt + "    <strong>" + QString(name) + "</strong></td></tr></table></div>";
    } else {
        align = "left";
        color = "aliceblue";
        res = "<div align=\"left\"  position=\"absolute\"><table><tr><td><strong>" + QString(name)+ "</strong>    " + cdt + "</td></tr></table></div>";
    }
    res += "<div align=\"" + align +"\"  position=\"absolute\"><table><tr><td bgcolor=\"" + color + "\" style=\"max-width: 400px; border:1px double orange; font-size: 20px;\">"+ mes + "</td></tr></table><br/></div>";
    return res;
}

QString MainWindow::pack_pic(QString name, QString mes, bool isMe)
{
    QString align = "", color = "", res = "";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString cdt = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    if(isMe) {
        align = "right";
        res = "<div align=\"right\"  position=\"absolute\"><table><tr><td>" + cdt + "    <strong>" + QString(name) + "</strong></td></tr></table></div>";
    } else {
        align = "left";
        res = "<div align=\"left\"  position=\"absolute\"><table><tr><td><strong>" + QString(name)+ "</strong>    " + cdt + "</td></tr></table></div>";
    }
    res += "<div align=\"" + align +"\"  position=\"absolute\"><table><tr><td><img src=\""+ mes + "\"/></td></tr></table><br/></div>";
    return res;
}

QString MainWindow::pack_pre_file(QString name, bool isMe, QString filename, int filesize)
{
    QString align = "", color = "", res = "";
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString cdt = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    if(isMe) {
        align = "right";
        res = "<div align=\"right\"  position=\"absolute\"><table><tr><td>" + cdt + "    <strong>" + QString(name) + "</strong></td></tr></table></div>";
    } else {
        align = "left";
        res = "<div align=\"left\"  position=\"absolute\"><table><tr><td><strong>" + QString(name)+ "</strong>    " + cdt + "</td></tr></table></div>";
    }
    res += "<div><table align=\""+ align+"\" style=\" width: fit-content; border:1px solid black;\"><tr><td align=\"center\"><img src=\"://file.png\"/></td></tr><tr><td align=\"center\"><font size=\"1\">文件</font></td></tr><tr><td><font size=\"2\">"+
            filename +"("+ QString::number((float)filesize/1024/1024, 'f', 2) +"MB)</font></td></tr></table></div><br/>";
    qDebug()<<"this----"<<align;
    return res;
}

QString MainWindow::pack_file(QString name, bool isMe, QString filename, int filesize, QString mes)
{
    QString res = pack_pre_file(name, isMe, filename, filesize);
    res += pack_file_mes(filename, mes);
    return res;
}

QString MainWindow::pack_file_mes(QString filename, QString mes)
{
    return "<div align=\"center\"><table><tr><td><font color=\"blue\" size=\"3\">----文件\""+ filename +"\""+ mes +"----</font></td></tr></table></div>";
}

void MainWindow::on_message_returnPressed()
{
    on_sendMassage_clicked();
}

void MainWindow::on_picture_clicked()
{
    QString filepath=QFileDialog::getOpenFileName(this,tr("Open Image"),QDir::homePath(),tr("(*.jpg)\n(*.bmp)\n(*.png)\n(*.gif)"));
    if(filepath.isEmpty()) {
        return;
    }
    QString filename = filepath.mid(filepath.lastIndexOf('/')+1);
    QString mes = pack_pic(m_name, filepath, true);
    ui->record->append(mes);

#ifndef MULT_THREAD
    QFile file(filepath);
    QFileInfo info(filepath);
    int fileSize = info.size();
    file.open(QFile::ReadOnly);
    message_head curr_head;
    curr_head.m_type = message_type::m_picture;
    strcpy(curr_head.name, m_name);
    strcpy(curr_head.filename, filename.toStdString().c_str());
    curr_head.size = fileSize;
    m_tcp->write((char*)&curr_head, sizeof(curr_head));

    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        m_tcp->write(line);
    }
#else
    qintptr cfd = m_tcp->socketDescriptor();
    if(cfd == -1) {
        return ;
    }
    emit send_pic(cfd, filepath, m_name);
#endif
}


void MainWindow::on_record_textChanged()
{
    ui->record->moveCursor(QTextCursor::End);
}

bool MainWindow::checkLoginAxec()
{
    if(login->exec() == QDialog::Accepted) {
        QString ip, name;
        this->login->getIP_Name(ip, name);
        strcpy(m_name, name.toStdString().c_str());
        unsigned short port = PORT;
        m_tcp->connectToHost(ip, port);
        return true;
    }
    return false;
}

void MainWindow::on_file_clicked()
{
    QString filepath=QFileDialog::getOpenFileName();
    if(filepath.isEmpty()) {
        return;
    }

    QString filename = filepath.mid(filepath.lastIndexOf('/')+1);
    if(m_tcp->state() != QAbstractSocket::ConnectedState) {
        QString mes = pack_file_mes(filename, "发送失败:未连接到服务器！！");
        ui->record->append(mes);
        return;
    }

    QFile file(filepath);
    QFileInfo info(filepath);
    int fileSize = info.size();
    file.open(QFile::ReadOnly);
    message_head curr_head;
    curr_head.m_type = message_type::m_file;
    strcpy(curr_head.name, m_name);
    strcpy(curr_head.filename, filename.toStdString().c_str());
    curr_head.size = fileSize;
    m_tcp->write((char*)&curr_head, sizeof(curr_head));

    while(!file.atEnd()) {
        QByteArray line = file.readLine();
        m_tcp->write(line);
    }
    QString mes = pack_pre_file(curr_head.name, true, filename, curr_head.size);
//    mes += pack_file_mes(filename, "已成功发送！");
    ui->record->append(mes);
}
