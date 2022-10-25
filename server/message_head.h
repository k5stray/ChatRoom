#ifndef MESSAGE_HEAD_H
#define MESSAGE_HEAD_H

enum message_type {
    m_mesage=0,
    m_picture=1,
    m_status=2
};

typedef struct {
    message_type m_type;
    int size;
    int connect_num;
    char name[64];
    char filename[64];
}message_head;

#endif // MESSAGE_HEAD_H
