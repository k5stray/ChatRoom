package server

const (
	g_message = int32(0)
	g_picture = int32(1)
	g_status  = int32(2)
	g_file    = int32(3)
)

type message_head struct {
	message_type int32
	size         int32
	connect_num  int32
	name         [64]byte
	filename     [64]byte
}

type SliceMock struct {
	addr uintptr
	len  int
	cap  int
}

func Bytes64String(p [64]byte) string {
	for i := 0; i < len(p); i++ {
		if p[i] == 0 {
			return string(p[0:i])
		}
	}
	return string(p[:])
}
