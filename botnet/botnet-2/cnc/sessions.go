package main

import "fmt"

func (session *Admin) Print(a ...interface{}) {
	_, _ = session.conn.Write([]byte(fmt.Sprint(a...)))
}

func (session *Admin) Printf(format string, val ...any) {
	session.Print(fmt.Sprintf(format, val...))
}

func (session *Admin) Println(a ...interface{}) {
	session.Print(fmt.Sprint(a...) + "\r\n")
}

func (session *Admin) Clear() {
	session.Printf("\x1bc")
}

func (session *Admin) Close() {
	session.conn.Close()
}
