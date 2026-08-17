package main

import (
    "fmt"
    "net"
    "errors"
    "time"
    "bufio"
    "log"
    "math/rand"
    "os"
    "strings"
)


var maxCombos int = 20

func main() {
    combo, err := net.Listen("tcp", "0.0.0.0:38213")
    if err != nil {
        fmt.Println(err)
        return
    }

    for {
        conn, err := combo.Accept()
        if err != nil {
            break
        }
        go comboHandler(conn)
    }

    fmt.Println("Stopped accepting clients")
}

func comboHandler(conn net.Conn) {
    defer conn.Close()

    var combos []string

    buf := make([]byte, 1024)
    tmp := make([]byte, 1)

    l, err := conn.Read(tmp)
    if err != nil || l <= 0 {
        return
    }

    file, err := os.Open("combos.txt")
    if err != nil {
        log.Fatal(err)
    }
    defer file.Close()

    scanner := bufio.NewScanner(file)
    for scanner.Scan() {
        combos = append(combos, scanner.Text())
    }

    if err := scanner.Err(); err != nil {
        log.Fatal(err)
    }

    rand.Seed(time.Now().UTC().UnixNano())

    for i := 0; i < maxCombos; i++ {
        split := strings.Split(combos[rand.Intn(len(combos))], ":")
        buf = append(buf, uint8(len(split[0] + ":" + split[1])))
        buf = append(buf, []byte(split[0] + ":" + split[1])...)
    }

    fmt.Printf("[credentials] delivered %d credentials to %s\n", maxCombos, conn.RemoteAddr())
    conn.Write(buf)
}

func readXBytes(conn net.Conn, buf []byte) (error) {
    tl := 0

    for tl < len(buf) {
        n, err := conn.Read(buf[tl:])
        if err != nil {
            return err
        }
        if n <= 0 {
            return errors.New("Connection closed unexpectedly")
        }
        tl += n
    }

    return nil
}
