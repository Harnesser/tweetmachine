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
    buf, err := ioutil.ReadFile("./dev_data/search.milklabs.json")
    if err != nil {
        fmt.Fprintf(w, "Error: %s", err.String() )
        return
    }
    tweets := arduino_json.RewriteTwitterJSON(buf)
    fmt.Fprintf(w, "<PRE>%s</PRE>", tweets)
}
