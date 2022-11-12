#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QFileDialog>

#include "login_win.h"
#include "message_head.h"
#include "recv_file.h"
#include "rename.h"

#define NAME_SIZE 64

namespace Ui {
class MainWindow;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool checkLoginAxec();
    void PrintMesaageHead(QString pre, const message_head& curr_head);
    ~MainWindow();

private slots:

    void on_sendMassage_clicked();

    void on_message_returnPressed();

    void on_picture_clicked();

    void on_record_textChanged();

    void on_file_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_tcp;
    QLabel *m_status;
    QLabel *connect_num;
    char m_name[NAME_SIZE];
    char m_peer_name[NAME_SIZE];
    login_win *login;
    recv_file *recv;
    Rename    *rename;
    message_head check_head;
    bool connect_stat;
    QString Ip;
private:
    QString pack_message(QString name, QString mes, bool isMe);
    QString pack_pic(QString name, QString mes, bool isMe);
    QString pack_pre_file(QString name, bool isMe, QString filename, int filesize);
    QString pack_file(QString name, bool isMe, QString filename, int filesize, QString mes);
    QString pack_file_mes(QString filename, QString mes);


signals:
    void send_pic(qintptr cfd, QString path, const char* m_name);
    void CheckBack();
};

#endif // MAINWINDOW_H
