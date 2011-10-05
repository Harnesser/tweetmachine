package milklabs_tweets

import (
    "fmt"
    "http"
)

func init() {
    http.HandleFunc("/", handler )
}

func handler(w http.ResponseWriter, r *http.Request ) {
    fmt.Fprintf(w, "Hello World! ADSFadsfasdfaadsfadf")
}
