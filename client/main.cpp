#include "mainwindow.h"
#include "login_win.h"
#include <QApplication>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    if(w.checkLoginAxec()) {
        w.show();
        return a.exec();
    }

    return 0;

}
