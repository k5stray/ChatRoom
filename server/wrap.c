#include "wrap.h"

void sys_err(const char* str)
{
	        perror(str);
		exit(1);
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int n;
again:
	if((n = accept(sockfd, addr, addrlen)) < 0) {
		if((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			sys_err("accept error");
	}
	return n;
}

int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int n;
	if((n = bind(sockfd, addr, addrlen)) < 0)
		sys_err("bind error");
	return n;
}

int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int n;
	if((n = connect(sockfd, addr, addrlen)) < 0)
		sys_err("connect error");
	return n;
}

int Socket(int domain, int type, int protocol)
{
	int n;
	if((n=socket(domain, type, protocol)) < 0)
		sys_err("socket error");
	return n;
}

ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t n;
again:
	if((n=read(fd, buf, count)) == -1) {
		if( errno == EINTR)
			goto again;
		else {
		       sys_err("Read error");	
			return -1;
		}
	}
	return n;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
	        ssize_t n;
again:
		if((n=write(fd, buf, count)) == -1) {
			if( errno == EINTR)
				goto again;
			else
				return -1;
		}
		return n;
}

int Close(int fd)
{
	int n;
	if((n=close(fd)) == -1)
		sys_err("close error");
	return n;
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = (char*)vptr;
	nleft = n;

	while(nleft >0 ) {
		if((nread = read(fd, ptr, nleft)) < 0) {
			if( errno == EINTR)
				nread = 0;
			else
				return -1;
		}else if(nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n - nleft;
}

ssize_t Writen(int fd, void *vptr, size_t n)
{
        size_t nleft;
        ssize_t nwritten;
        char *ptr;

        ptr = (char*)vptr;
        nleft = n;

        while(nleft >0 ) {
                if((nwritten = read(fd, ptr, nleft)) < 0) {
                        if( errno == EINTR)
                                nwritten = 0;
                        else
                                return -1;
                }else if(nwritten == 0)
                        break;
                nleft -= nwritten;
                ptr += nwritten;
        }
        return n - nleft;
}

static ssize_t my_read(int fd, char *ptr)
{
        static int read_cnt;
        static char *read_ptr;
        static char read_buf[100];

        if(read_cnt <=0 ) {
again:
                if((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
                        if(errno == EINTR)
                                goto again;
                        return -1;
                }else if(read_cnt == 0)
                        return 0;

                read_ptr = read_buf;
        }
        read_cnt--;
        *ptr = *read_ptr++;

        return 1;
}

ssize_t Readline(int fd, void *vptr, int maxlen)
{
	ssize_t n,rc;
	char c, *ptr;
	ptr = (char*)vptr;

	for(n=1;n<maxlen;n++) {
		if((rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if(c == '\n')
				break;
		}else if(rc == 0) {
			*ptr = 0;
			return n-1;
		}else
			return -1;
	}
	*ptr=0;
	return n;
}

int Listen(int sockfd, int backlog)
{
	int n;
	if((n = listen(sockfd, backlog)) < 0) {
		sys_err("Listen error");
	}
	return n;
}
