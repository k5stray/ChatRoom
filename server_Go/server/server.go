package server

import (
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"os"
	"sync"
	"unsafe"
)

type Server struct {
	Port         int
	OnlineMap    map[string]*User
	mapLock      sync.RWMutex
	numLock      sync.RWMutex
	connnect_num int
	MessageCore  chan []byte
}

func NewServer(port int) *Server {
	return &Server{
		Port:         port,
		OnlineMap:    make(map[string]*User),
		MessageCore:  make(chan []byte),
		connnect_num: 0,
	}
}

func (this *Server) ListenMessagCore() {
	for {
		msg := <-this.MessageCore
		this.mapLock.Lock()
		for _, cli := range this.OnlineMap {
			cli.Message <- msg
		}
		this.mapLock.Unlock()
	}
}

func (this *Server) Run() {
	listen, err := net.Listen("tcp4", fmt.Sprintf("%s:%d", "0.0.0.0", 9527))
	if err != nil {
		fmt.Println("net.Listen error:", err)
		return
	}
	fmt.Println("Accept connect...........")
	defer listen.Close()

	go this.ListenMessagCore()

	for {
		conn, err := listen.Accept()
		if err != nil {
			fmt.Println("Accept() error:", err)
			return
		}
		fmt.Println(conn.RemoteAddr(), "  connect....")
		this.numLock.Lock()
		this.connnect_num++
		this.numLock.Unlock()
		go this.Handler(conn)
	}

}

func (this *Server) BroadCast(msg []byte) {
	this.MessageCore <- msg
}

func (this *Server) BroadCastFile(head []byte, file *os.File, size int64) {
	this.mapLock.Lock()
	for _, cli := range this.OnlineMap {
		cli.conn.Write(head)
		io.CopyN(cli.conn, file, size)
	}
	this.mapLock.Unlock()
	defer file.Close()
}

func (this *Server) UpdateConnectNum() {
	var curr_head message_head
	curr_head.connect_num = int32(this.connnect_num)
	curr_head.message_type = g_status
	copy(curr_head.name[:], "")
	curr_head.size = int32(0)
	midleBytes := &SliceMock{
		addr: uintptr(unsafe.Pointer(&curr_head)),
		len:  int(unsafe.Sizeof(curr_head)),
		cap:  int(unsafe.Sizeof(curr_head)),
	}
	re_head := *(*[]byte)(unsafe.Pointer(midleBytes))
	this.BroadCast(re_head)
}

func (this *Server) PrintHead(pre string, xx *message_head) {
	fmt.Println(pre, ".message_type:", xx.message_type)
	fmt.Println(pre, ".size:", xx.size)
	fmt.Println(pre, ".connect_num:", xx.connect_num)
	fmt.Println(pre, ".name:", string(xx.name[:]))
	fmt.Println(pre, ".filename:", string(xx.filename[:]))
}

func (this *Server) Handler(conn net.Conn) {
	defer conn.Close()
	user := NewUser(conn)
	this.mapLock.Lock()
	this.OnlineMap[user.Addr] = user
	this.mapLock.Unlock()

	this.UpdateConnectNum()
	for {
		buf := make([]byte, unsafe.Sizeof(message_head{}))
		n, err := conn.Read(buf)
		if n == 0 {
			this.numLock.Lock()
			this.connnect_num--
			this.UpdateConnectNum()
			this.numLock.Unlock()
			this.mapLock.Lock()
			delete(this.OnlineMap, user.Name)
			this.mapLock.Unlock()
			return
		}
		if err != nil && err != io.EOF {
			fmt.Println("conn Read ERROR:", err)
			return
		}
		var curr_head *message_head = *(**message_head)(unsafe.Pointer(&buf))
		if curr_head.message_type == g_message {
			data := make([]byte, curr_head.size)
			n, err = conn.Read(data)
			if err != nil && err != io.EOF {
				fmt.Println("conn Read ERROR:", err)
				return
			}
			this.numLock.RLock()
			xy := &message_head{
				message_type: g_message,
				connect_num:  int32(this.connnect_num),
				size:         curr_head.size,
				name:         curr_head.name,
			}
			this.numLock.RUnlock()
			midleBytes := &SliceMock{
				addr: uintptr(unsafe.Pointer(xy)),
				len:  int(unsafe.Sizeof(*xy)),
				cap:  int(unsafe.Sizeof(*xy)),
			}
			re_head := *(*[]byte)(unsafe.Pointer(midleBytes))
			this.BroadCast(re_head)
			this.BroadCast(data)
		} else if curr_head.message_type == g_picture {
			data := make([]byte, 8192)
			fmt.Println("filename: ", Bytes64String(curr_head.filename))
			file, err := os.OpenFile(Bytes64String(curr_head.filename), os.O_CREATE|os.O_RDWR|os.O_TRUNC, 0755)
			if err != nil {
				fmt.Println("OpenFile ERROR:", err)
				return
			}
			for size := curr_head.size; size > 0; {
				n, _ = conn.Read(data)
				size -= int32(n)
				file.Write(data[:n])
			}
			file.Close()

			xy := &message_head{
				message_type: g_picture,
				connect_num:  int32(this.connnect_num),
				size:         curr_head.size,
				name:         curr_head.name,
				filename:     curr_head.filename,
			}
			midleBytes := &SliceMock{
				addr: uintptr(unsafe.Pointer(xy)),
				len:  int(unsafe.Sizeof(*xy)),
				cap:  int(unsafe.Sizeof(*xy)),
			}
			re_head := *(*[]byte)(unsafe.Pointer(midleBytes))
			re_data, _ := ioutil.ReadFile(Bytes64String(curr_head.filename))
			this.BroadCast(re_head)
			this.BroadCast(re_data)

		} else if curr_head.message_type == g_file {
			data := make([]byte, 8192)
			filename := Bytes64String(curr_head.filename)
			f, err := os.OpenFile(filename, os.O_CREATE|os.O_RDWR|os.O_TRUNC, 0755)
			if err != nil {
				fmt.Println("OpenFile ERROR:", err)
				return
			}
			for size := curr_head.size; size > 0; {
				n, _ = conn.Read(data)
				size -= int32(n)
				f.Write(data[:n])
			}
			f.Close()

			xy := &message_head{
				message_type: g_file,
				connect_num:  int32(this.connnect_num),
				size:         curr_head.size,
				name:         curr_head.name,
				filename:     curr_head.filename,
			}
			midleBytes := &SliceMock{
				addr: uintptr(unsafe.Pointer(xy)),
				len:  int(unsafe.Sizeof(*xy)),
				cap:  int(unsafe.Sizeof(*xy)),
			}
			re_head := *(*[]byte)(unsafe.Pointer(midleBytes))
			re_data, _ := ioutil.ReadFile(filename)
			this.BroadCast(re_head)
			this.BroadCast(re_data)
		} else if curr_head.message_type == g_status {
			name := Bytes64String(curr_head.name)
			xy := &message_head{
				message_type: g_status,
				connect_num:  int32(this.connnect_num),
				size:         -1,
				name:         [64]byte{'r', 'e', 'n', 'a', 'm', 'e'},
				filename:     curr_head.filename,
			}
			xx := &message_head{
				message_type: g_status,
				connect_num:  int32(this.connnect_num),
				size:         -1,
				name:         [64]byte{'n', 'a', 'm', 'e'},
				filename:     curr_head.filename,
			}
			midleBytes_xy := &SliceMock{
				addr: uintptr(unsafe.Pointer(xy)),
				len:  int(unsafe.Sizeof(*xy)),
				cap:  int(unsafe.Sizeof(*xy)),
			}
			midleBytes_xx := &SliceMock{
				addr: uintptr(unsafe.Pointer(xx)),
				len:  int(unsafe.Sizeof(*xx)),
				cap:  int(unsafe.Sizeof(*xx)),
			}
			re_xy := *(*[]byte)(unsafe.Pointer(midleBytes_xy))
			re_xx := *(*[]byte)(unsafe.Pointer(midleBytes_xx))
			this.mapLock.Lock()
			_, ok := this.OnlineMap[name]
			if ok {
				this.BroadCast(re_xy)
			} else {
				_, oo := this.OnlineMap[user.Addr]
				if oo {
					delete(this.OnlineMap, user.Addr)
				}
				this.OnlineMap[name] = user
				user.Name = name
				fmt.Println("Accept name: ", name)
				this.BroadCast(re_xx)
			}
			this.mapLock.Unlock()
		}

	}
}
