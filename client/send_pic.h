#ifndef SEND_PIC_H
#define SEND_PIC_H

#include <QObject>

class Send_Pic : public QObject
{
    Q_OBJECT
public:
    explicit Send_Pic(QObject *parent = nullptr);
    void send_pic(qintptr cfd, QString path, const char* m_name);

signals:

public slots:
};

#endif // SEND_PIC_H
