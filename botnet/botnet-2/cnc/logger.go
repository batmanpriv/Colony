package main

import (
	"fmt"
	"time"
)

var (
	currt = time.DateTime
)

func Info(text string) {
	fmt.Println("\x1b[96m", currt, "\x1b[95mINFO\x1b[0m", text)
}

func Warning(text string) {
	fmt.Println("\x1b[96m", currt, "\x1b[93mWARN\x1b[0m", text)
}

func Error(text string) {
	fmt.Println("\x1b[31m", currt, "\x1b[31mERROR\x1b[0m", text)
}
