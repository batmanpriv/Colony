package main

import (
    "fmt"
    "net/http"
	"io/ioutil"
    "encoding/json"
    "log"
    "bufio"
    "os"
    "time"
    "strings"
    )
type geo struct {
    IP string `json:"ip"`
    ISP string `json:"isp"`
    ORG string `json:"org"`
    HOSTNAME string `json:hostname`
    COUNTRY string `json:country_name`
    CONTINENT_CODE string `"json:continent_code"`
    ASN string `"json:asn"`
}

func getGEO(target string) string{
    response, err := http.Get("https://json.geoiplookup.io/"+target)
    if err != nil{
        fmt.Printf("%s", err)
    }else{
        defer response.Body.Close()
        contents, err := ioutil.ReadAll(response.Body)
        if err != nil {
            fmt.Printf("%s", err)
        }

        jsonOut := geo{}
        jsonErr := json.Unmarshal(contents, &jsonOut)
        if jsonErr != nil {
            log.Fatal(jsonErr)
        }
        return "\033[31mIP: "+jsonOut.IP+"\r\n\033[31mISP: "+jsonOut.ISP+"\r\n\033[31mORG: "+jsonOut.ORG+"\r\n\033[31mHostname: "+jsonOut.HOSTNAME+"\r\n\033[31mCountry: "+jsonOut.COUNTRY+"\r\n\033[31mContinent:"+jsonOut.CONTINENT_CODE+"\r\n\033[31mASN: "+jsonOut.ASN+"\r\n\033[0;91m"
    }
    return ""
}

func DisplayBanner(this *Admin, path string) error {
    file, err := os.Open(path)
    if err != nil {
        return err
    }

    scanner := bufio.NewScanner(file)
    scanner.Split(bufio.ScanLines)
    for scanner.Scan() {
        if strings.Contains(scanner.Text(), "<<100>>") {
            time.Sleep(100 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<200>>") {
            time.Sleep(200 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<300>>") {
            time.Sleep(300 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<400>>") {
            time.Sleep(400 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<500>>") {
            time.Sleep(500 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<600>>") {
            time.Sleep(600 * time.Millisecond)
        }

        if strings.Contains(scanner.Text(), "<<700>>") {
            time.Sleep(700 * time.Millisecond)
        }

        if strings.Contains(scanner.Text(), "<<800>>") {
            time.Sleep(800 * time.Millisecond)
        }

        if strings.Contains(scanner.Text(), "<<900>>") {
            time.Sleep(900 * time.Millisecond)
        }

        if strings.Contains(scanner.Text(), "<<1000>>") {
            time.Sleep(1000 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(120)>>") {
            time.Sleep(120 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(3)>>") {
            time.Sleep(3 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(20)>>") {
            time.Sleep(20 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(40)>>") {
            time.Sleep(40 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(60)>>") {
            time.Sleep(60 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(80)>>") {
            time.Sleep(80 * time.Millisecond)
        }
        if strings.Contains(scanner.Text(), "<<sleep(100)>>") {
            time.Sleep(100 * time.Millisecond)
        }
        replacer := strings.NewReplacer(
            "<<100>>", "",
            "<<200>>", "",
            "<<300>>", "",
            "<<400>>", "",
            "<<500>>", "",
            "<<600>>", "",
            "<<700>>", "",
            "<<800>>", "",
            "<<900>>", "",
            "<<1000>>", "",
            "<<sleep(120)>>", "",
            "<<sleep(20)>>", "",
            "<<sleep(40)>>", "",
            "<<sleep(60)>>", "",
            "<<sleep(80)>>", "",
            "<<sleep(100)>>", "",
            "<<$clear>>", "\033c",
        )
        lel := replacer.Replace(scanner.Text())
        this.conn.Write([]byte(fmt.Sprintf("%s\r\n", lel)))
    }
    return nil
}

