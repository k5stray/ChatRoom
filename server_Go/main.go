package main

import (
	"ChatServer/server"
)

func main() {
	server := server.NewServer(9527)
	server.Run()
}
