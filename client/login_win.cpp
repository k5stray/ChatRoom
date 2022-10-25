#include "login_win.h"
#include "ui_login_win.h"

#include <QMessageBox>

login_win::login_win(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login_win)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.ico"));
}

login_win::~login_win()
{
    delete ui;
}

/*检测ip合法性*/
bool login_win::checkIPV4(std::string s)
{
    int k=0;        //记录每个segment起始位置
    int pCnt=0;     //记录'.'的个数
    s.push_back('.');   //方便atoi使用
    for(int i=0; i<s.size(); ++i){
        if(s[i] == '.'){
            s[i] = '\0';    //方便atoi使用
            if( s[k]=='\0'                                //连续..或第一个为.的情况
                || (s[k]=='0' && strlen(&s[k])>1)         //以0开头的情况
                || !(atoi(&s[k])<=255 && atoi(&s[k])>=0)) //不符合区间范围
            {
                return false;
            }
            k = i+1;
            ++pCnt;
        } else if(!(s[i]>='0' && s[i]<='9')) {            //包含非 0-9或'.' 的情况
            return false;
        }
    }

    if(pCnt != 3+1) return false;     //'.'不是3段,最后一个1是自己加的

    return true;
}

void login_win::on_cancel_clicked()
{
    ui->nickname->clear();
    ui->ip->clear();
}

void login_win::on_submit_clicked()
{
    QString ip = ui->ip->text();
    QString nickname = ui->nickname->text();
    if(!checkIPV4(ip.toStdString())) {
        QMessageBox::warning(this, "警告", "您输入的IP地址不正确！");
        return;
    }
    if(nickname.size() > 64) {
        QMessageBox::warning(this, "警告", "您输入的昵称过长！");
        return;
    }
    accept();
}

void login_win::getIP_Name(QString &ip, QString &name)
{
    ip = ui->ip->text();
    name = ui->nickname->text();
}
