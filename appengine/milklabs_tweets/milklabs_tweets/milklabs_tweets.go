package milklabs_tweets

import (
    "fmt"
    "http"
    "io/ioutil"
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
    read_dev_json(w)
}


func read_dev_json(w http.ResponseWriter){

    // Open file
    buf, err := ioutil.ReadFile("./dev_data/search.milklabs.json")
    if err == nil {
        fmt.Fprintf(w, "%s", buf)
    } else {
        fmt.Fprintf(w, "ERERER")
    }
    
}
