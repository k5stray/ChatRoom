
# 多人网络聊天室 [ Qt(客户端) <==>  Socket(C++服务端)/Go(服务端) ]
## 1.简介
基于Qt和socket网络编程实现的简易网络聊天室，采用select进行监听，使用Tcp连接，可同时容纳最高1024个连接，支持发送文字消息、图片和文件消息。
## 2.由来
上网搜了下，发现有很多用Qt实现的聊天软件实例，都是只有文字或是只有图片传输的实例，没有看到哪位老哥整合过这个。本着学了就要试试的原则，完成了这个项目。项目的逻辑就是消息的转发与控制，虽然看着挺简单，但实现过程会遇到各种各样的问题，总之还是挺有意思的，就上传上来了。
## 3.详情
#### 消息数据头部
```
enum message_type {
    m_mesage=0,
    m_picture=1,
    m_status=2,
    m_file=3
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

#### 聊天
坤与神明
![](https://github.com/k5stray/ChatRoom/blob/main/image/chat1.png)
![](https://github.com/k5stray/ChatRoom/blob/main/image/chat2.png)

## 4.关于Go后端
使用Go实现后端，功能实际上跟C++没有什么区别，只是没有了最高1024个连接的限制，可接纳的同时在线人数要更高，这也是select的缺陷之一。后端根据Go的特性使用了协程和管道，与C++后端略微有一些差别，写法参照（[https://www.bilibili.com/video/BV1gf4y1r79E?p=37](https://www.bilibili.com/video/BV1gf4y1r79E?p=37)）。GO在数据类型转换的时候就没有C和C++那么的自由了，struct转byte切片参考大佬博客（[go中struct和[]byte互相转换](https://blog.csdn.net/JineD/article/details/121605278)）。个人体会：传输层往下的封包与解包都是由操作系统完成的，网络编程实际是利用操作系统给出的传输层接口进行上层逻辑的开发，与使用什么语言无关，只要传输层使用相同的协议即可通信。
## 5.参考

##### 《Linux高性能服务器编程》-中国-游双
##### Qt多线程网络通信: [https://www.bilibili.com/video/BV1LB4y1F7P7/?vd_source=21b2b7195bef3c50374169c309007961](https://www.bilibili.com/video/BV1LB4y1F7P7/?vd_source=21b2b7195bef3c50374169c309007961)
##### 黑马程序员-Linux网络编程: [https://www.bilibili.com/video/BV1iJ411S7UA/?vd_source=21b2b7195bef3c50374169c309007961](https://www.bilibili.com/video/BV1iJ411S7UA/?vd_source=21b2b7195bef3c50374169c309007961)

