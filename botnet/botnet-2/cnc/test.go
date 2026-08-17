package main

import (
	"net"
	"strconv"
	"strings"
	"fmt"
)

type Printer struct {
	conn net.Conn
}

func parseInt(s string) int {
	n, _ := strconv.Atoi(s)
	return n
}

func printGradient(text, startColor, endColor string) string {
	startRGB := strings.Split(startColor, ",")
	endRGB := strings.Split(endColor, ",")
	changer := (parseInt(endRGB[0]) - parseInt(startRGB[0])) / len(text)
	changeg := (parseInt(endRGB[1]) - parseInt(startRGB[1])) / len(text)
	changeb := (parseInt(endRGB[2]) - parseInt(startRGB[2])) / len(text)

	r := parseInt(startRGB[0])
	g := parseInt(startRGB[1])
	b := parseInt(startRGB[2])

	var output string
	for _, letter := range text {
		r += changer
		g += changeg
		b += changeb

		if letter != '\n' {
			output += fmt.Sprintf("\x1b[40;38;2;%d;%d;%dm%c\033[0m", r, g, b, letter)
		} else {
			output += string(letter)
		}
	}

	return output
}

func (p *Printer) PrintGradientf(text string) {
	gradientText := printGradient(text, "5,189,245", "252,8,8")
	p.conn.Write([]byte(gradientText))
}

func (p *Printer) PrintGradientln(text string) {
	gradientText := printGradient(text, "5,189,245", "252,8,8") + "\r\n"
	p.conn.Write([]byte(gradientText))
}

func (p *Printer) Printf(text string) {
	p.conn.Write([]byte(text))
}

func (p *Printer) Println(text string) {
	p.conn.Write([]byte(text + "\r\n"))
}