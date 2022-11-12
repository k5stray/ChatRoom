package server

import (
	"net"
)

type User struct {
	Name    string
	Message chan []byte
	conn    net.Conn
	Addr    string
}

func NewUser(conn net.Conn) *User {
	user := &User{
		Message: make(chan []byte),
		conn:    conn,
		Addr:    conn.RemoteAddr().String(),
	}

	go user.ListenMessage()

	return user
}

func (this *User) ListenMessage() {
	for {
		msg := <-this.Message
		this.conn.Write(msg)
	}
}
