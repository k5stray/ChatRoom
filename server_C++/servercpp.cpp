#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <map>
#include "message_head.h"
#include "wrap.h"

#define SRV_PORT 9527

using namespace std;

class Server
{
public:
	static Server *GetInstance();
	void Init();
	void Run();
	void UpdateConnect();
	void DoAccept();
	void DoSendMessage(int sfd, message_head &curr_head);
	void DoSendPicture(int sfd, message_head &curr_head);
	void DoSendFile(int sfd, message_head &curr_head);
	void DoCheck(int sfd, message_head &curr_head);
private:

	Server();
	~Server();
	Server(const Server&);
	const Server &operator=(const Server&);

private:
	static int connect_num;		//连接人数
	int lfd;			//监听文件描述符
	int cfd;			//客户端文件描述符
	int n=0;			//read到的字节数
	int i=0;			//遍历因子
	int maxi=0;			//客户端文件描述符最大下标
	int nready;			//描述符集合中包含的文件描述符的数量
	int maxfd;              	//最大文件描述符
	struct sockaddr_in srv_addr;	//服务器结构体
	struct sockaddr_in clt_addr;	//客户端结构体
        socklen_t clt_addr_len;		//客户端地址长度
        char buf[BUFSIZ];		//数据缓存
	int client[FD_SETSIZE];		//客户端文件描述符数组
	fd_set rset;			//文件描述符集合
	fd_set allset;			//文件描述符集合备份
	map<string, int> UserList;	//用户列表
};

Server::Server() 
{

}

Server::~Server() 
{

}

Server *Server::GetInstance()
{
	static Server server;
	return &server;
}

void Server::Init()
{
        bzero(&srv_addr, sizeof(srv_addr));
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_port = htons(SRV_PORT);
        srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        lfd = Socket(AF_INET, SOCK_STREAM, 0);

	//设置服务器端口复用
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        Bind(lfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));

        Listen(lfd, 128);

        printf("Accept client connect......\n");
	
	maxfd = lfd;
	for(i=0;i<FD_SETSIZE;i++) {
		client[i]=-1;   
	}

	FD_ZERO(&allset);
	FD_SET(lfd, &allset);	
}

void Server::Run()
{
	Init();
	while(1) {
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(nready<0)
			sys_err("select error");
		
		//处理连接请求
		if(FD_ISSET(lfd, &rset)) {

			DoAccept();

			if( 0 == --nready) {
				continue;
			}
		}
		for(i = 0;i<=maxi;i++) {
			int sfd = client[i];
			if(sfd < 0)
				continue;
			if(FD_ISSET(sfd, &rset)) {
				message_head curr_head;
				n = Read(sfd, &curr_head, sizeof(curr_head));
				curr_head.connect_num = connect_num;
				if(n == 0) {
					printf("the client %d closed\n", sfd);
					close(sfd);
					FD_CLR(sfd, &allset);
					
					for(map<string, int>::iterator it = UserList.begin();it != UserList.end();it++) {
						if(it->second == sfd) {
							UserList.erase(it);
							break;
						}
					}

					client[i]=-1;
					if(connect_num > 0) {
						connect_num--;
					}
					//更新状态
					UpdateConnect();					
				}else if(n>0) {
					if(curr_head.m_type == m_mesage) {
						DoSendMessage(sfd, curr_head);
					} else if(curr_head.m_type == m_picture) {
						DoSendPicture(sfd, curr_head);
					} else if(curr_head.m_type == m_file) {
						DoSendFile(sfd, curr_head);
					} else if(curr_head.m_type == m_status) {
						DoCheck(sfd, curr_head);
					}

				}
			
			} 
		}
	}
	close(lfd);
}

void Server::DoAccept()
{
	char str[INET_ADDRSTRLEN];
	clt_addr_len = sizeof(clt_addr);

	//由于是有连接请求才触发到这里，所以accept不会阻塞等待
	cfd = Accept(lfd, (struct sockaddr*)&clt_addr, &clt_addr_len);
	printf("received from %s at PORT %d\n",
                inet_ntop(AF_INET, &clt_addr.sin_addr.s_addr, str, sizeof(str)),
                        ntohs(clt_addr.sin_port));

	FD_SET(cfd, &allset);
	
	//连接人数加1
	connect_num++;
	
	for(i=0;i<FD_SETSIZE;i++) {
		if(client[i] == -1){
			client[i] = cfd;
			break;
		}
	}

	UpdateConnect();

	if(i == FD_SETSIZE) {
		fputs("too many clients\n", stderr);
		exit(1);
	}
	if(maxi <i) {
		maxi = i;
	}

	if(maxfd < cfd) {
		maxfd = cfd;
	}
}

void Server::UpdateConnect()
{
	//向客户端更新人数状态
	message_head status_head;
	status_head.m_type = m_status;
	status_head.connect_num = connect_num;
	status_head.size = 0;
	for(int k=0;k<=maxi;k++) {
		if(client[k] < 0) {
			continue;
		}
		Write(client[k], (char*)&status_head, sizeof(status_head));
	}
}

void Server::DoSendMessage(int sfd, message_head &curr_head)
{
	curr_head.connect_num = connect_num;
	n = Read(sfd, buf, sizeof(buf));
	Write(STDOUT_FILENO, buf, n);
	for(int k=0;k<=maxi;k++) {
		if(client[k] < 0 || client[k] == sfd) {
			continue;
		}
		Write(client[k], (char*)&curr_head, sizeof(curr_head));
		Write(client[k], buf, n);
	}
	bzero(buf, sizeof(buf));	
}

void Server::DoSendPicture(int sfd, message_head &curr_head)
{
	curr_head.connect_num = connect_num;
	int size = curr_head.size;
	FILE *fp = fopen(curr_head.filename, "wb+");

	while(size > 0) {
		n = Read(sfd, buf, sizeof(buf));
		fwrite(buf, n, 1, fp);
		size -= n;							
	}
	fclose(fp);
	for(int k=0;k<=maxi;k++) {
		if(client[k] < 0 || client[k] == sfd) {
			continue;
		}

		Write(client[k], (char*)&curr_head, sizeof(curr_head));

		int filefd = open(curr_head.filename, O_RDONLY);
			
		struct stat st;
		stat(curr_head.filename, &st);

		sendfile(client[k], filefd, NULL, static_cast<size_t>(curr_head.size));

	}						
	bzero(buf, sizeof(buf));	
}

void Server::DoSendFile(int sfd, message_head &curr_head)
{
	curr_head.connect_num = connect_num;
	int size = curr_head.size;

	FILE *fp = fopen(curr_head.filename, "wb+");

	while(size > 0) {
		n = Read(sfd, buf, sizeof(buf));
		fwrite(buf, n, 1, fp);
		size -= n;							
	}
	fclose(fp);
	for(int k=0;k<=maxi;k++) {
		if(client[k] < 0 || client[k] == sfd) {
			continue;
		}

		Write(client[k], (char*)&curr_head, sizeof(curr_head));

		int filefd = open(curr_head.filename, O_RDONLY);
			
		struct stat st;
		stat(curr_head.filename, &st);

		sendfile(client[k], filefd, NULL, static_cast<size_t>(curr_head.size));

	}						
	bzero(buf, sizeof(buf));
}

void Server::DoCheck(int sfd, message_head &curr_head)
{
	message_head re_head;
	re_head.size = -1;
	re_head.m_type = m_status;
	re_head.connect_num = connect_num;
	if(UserList[curr_head.name] == 0) {
		UserList[curr_head.name] = sfd;
		strcpy(re_head.name, "name");
		Write(sfd, (char*)&re_head, sizeof(re_head));
	} else {
		strcpy(re_head.name, "rename");
		Write(sfd, (char*)&re_head, sizeof(re_head));
	}
	
}

int Server::connect_num = 0;

int main()
{
	Server *server = Server::GetInstance();
	server->Run();
	return 0;
}

