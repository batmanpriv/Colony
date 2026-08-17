package main

import (
	"math/rand"
)

func random(min int, max int) int {
	return rand.Intn(max-min) + min
}
