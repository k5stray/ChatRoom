#ifndef RECV_FILE_H
#define RECV_FILE_H

#include <QDialog>

namespace Ui {
class recv_file;
}

class recv_file : public QDialog
{
    Q_OBJECT

public:
    explicit recv_file(QWidget *parent = 0);
    void setFileName(QString filename);
    void init();
    void setOwn(QString name);
    void setFileSize(float size);
    ~recv_file();

private slots:
    void on_accpet_clicked();

    void on_cancel_clicked();

private:
    Ui::recv_file *ui;
};

#endif // RECV_FILE_H
