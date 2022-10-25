#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QFileDialog>

#include "login_win.h"
#include "message_head.h"

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
    ~MainWindow();

private slots:

    void on_sendMassage_clicked();

    void on_message_returnPressed();

    void on_picture_clicked();

    void on_record_textChanged();

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_tcp;
    QLabel *m_status;
    QLabel *connect_num;
    char m_name[NAME_SIZE];
    char m_peer_name[NAME_SIZE];
    QString pack_message(QString name, QString mes, bool isMe);
    QString pack_pic(QString name, QString mes, bool isMe);
    login_win *login;


signals:
    void send_pic(qintptr cfd, QString path, const char* m_name);

};

#endif // MAINWINDOW_H
