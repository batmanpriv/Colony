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

	Chat bool
}

func (s *session) Remove() {
	fmt.Println("[YBot] >> Session closed.")
	sessionMutex.Lock()
	delete(sessions, s.ID)
	sessionMutex.Unlock()
}
