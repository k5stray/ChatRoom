#include "rename.h"
#include "ui_rename.h"

Rename::Rename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Rename)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
    setWindowTitle("昵称冲突");

}

Rename::~Rename()
{
    delete ui;
}

void Rename::SetName(QString name)
{
    ui->name->setText(name);
}

void Rename::on_pushButton_clicked()
{
    accept();
}

void Rename::on_pushButton_2_clicked()
{
    accept();
}
