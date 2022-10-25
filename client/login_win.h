#ifndef LOGIN_WIN_H
#define LOGIN_WIN_H

#include <QDialog>

namespace Ui {
class login_win;
}

class login_win : public QDialog
{
    Q_OBJECT

public:
    explicit login_win(QWidget *parent = 0);
    bool checkIPV4(std::string s);
    void getIP_Name(QString &ip, QString &name);
    ~login_win();

private slots:
    void on_cancel_clicked();

    void on_submit_clicked();

private:
    Ui::login_win *ui;
    QString ip;
    QString nickname;

};

#endif // LOGIN_WIN_H
