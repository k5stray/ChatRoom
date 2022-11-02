
# 基于QT和select的多人网络聊天室 Qt(客户端)  Socket(服务端)
## 1.简介
基于Qt和socket网络编程实现的简易网络聊天室，采用select进行监听，使用Tcp连接，可同时容纳最高1024个连接，支持发送文字消息、图片和文件消息。
## 2.由来
上网搜了下，发现有很多用Qt实现的聊天软件实例，都是只有文字或是只有图片传输的实例，没有看到哪位老哥整合过这个。本着学了就要试试的原则，完成了这个项目。项目很简单，但是实现过程遇到了各种各样的问题，大概只有自己试过了才知道，总之还是挺有意思的，就上传上来了。
## 3.详情
#### 消息数据头部
```
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
```
#### 传输的数据格式:
![](https://github.com/k5stray/ChatRoom/blob/main/image/%E6%95%B0%E6%8D%AE%E6%A0%BC%E5%BC%8F.jpg)

#### 通信的数据流程:
![](https://github.com/k5stray/ChatRoom/blob/main/image/cs2.jpg)

#### 登录：
![](https://github.com/k5stray/ChatRoom/blob/main/image/%E7%99%BB%E5%BD%95.png)

#### 聊天图
![](https://github.com/k5stray/ChatRoom/blob/main/image/chat1.png)
![](https://github.com/k5stray/ChatRoom/blob/main/image/chat2.png)

## 4.参考

##### 《Linux高性能服务器编程》-中国-游双
##### Qt多线程网络通信: [https://www.bilibili.com/video/BV1LB4y1F7P7/?vd_source=21b2b7195bef3c50374169c309007961](https://www.bilibili.com/video/BV1LB4y1F7P7/?vd_source=21b2b7195bef3c50374169c309007961)
##### 黑马程序员-Linux网络编程: [https://www.bilibili.com/video/BV1iJ411S7UA/?vd_source=21b2b7195bef3c50374169c309007961](https://www.bilibili.com/video/BV1iJ411S7UA/?vd_source=21b2b7195bef3c50374169c309007961)

