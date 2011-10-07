package milklabs_tweets

import (
    "fmt"
    "http"
    "io/ioutil"
    "arduino_json"
)

func init() {
    http.HandleFunc("/", handler )
    http.HandleFunc("/marty/", marty_handler )
}

func handler(w http.ResponseWriter, r *http.Request ) {
    fmt.Fprintf(w, "Hello World! ADSFadsfasdfaadsfadf")
}

func marty_handler(w http.ResponseWriter, r *http.Request ) {
    fmt.Fprintf(w, "MARTYTY!!!")
    readDevJSON(w)
    
    hello := []byte("adbadsfouhaosiudhfoausdhf\n")
    tweets := arduino_json.RewriteTwitterJSON(hello)
    fmt.Fprintf(w, "%s", tweets)
}


func readDevJSON(w http.ResponseWriter){

    // Open file
    buf, err := ioutil.ReadFile("./dev_data/search.milklabs.json")
    if err == nil {
        fmt.Fprintf(w, "%s", buf)
    } else {
        fmt.Fprintf(w, "ERERER")
    }
    
}
