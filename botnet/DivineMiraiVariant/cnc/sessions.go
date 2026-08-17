package main

import (
	"fmt"
	"net"
	"sync"
)

var (
	sessions     = make(map[int64]*session)
	sessionMutex sync.Mutex
)

type session struct {
	ID       int64
	Username string
	Conn     net.Conn
}

func (s *session) Remove() {
	fmt.Println("\033[00;1;95mSession Has Been Killed!\033[0m")
	sessionMutex.Lock()
	delete(sessions, s.ID)
	sessionMutex.Unlock()
}
