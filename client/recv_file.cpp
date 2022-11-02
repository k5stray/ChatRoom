#include "recv_file.h"
#include "ui_recv_file.h"

recv_file::recv_file(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::recv_file)
{
    ui->setupUi(this);

    init();
}

recv_file::~recv_file()
{
    delete ui;
}

void recv_file::on_accpet_clicked()
{
    accept();
}

void recv_file::on_cancel_clicked()
{
    reject();
}

void recv_file::setFileName(QString filename)
{
    ui->filename->setText(filename);
}

void recv_file::init()
{
    ui->filename->setText("");
    ui->own->setText("");
    setWindowTitle("接收文件");
    this->setWindowIcon(QIcon(":/icon.ico"));
}

void recv_file::setOwn(QString name)
{
    ui->own->setText(name);
}

void recv_file::setFileSize(float size)
{
    ui->filesize->setText(QString::number(size, 'f', 2));
}
