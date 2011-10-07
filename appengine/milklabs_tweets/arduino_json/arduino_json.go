/*
  Package to take JSON returned by a Twitter search and reduce it to 
a smaller size for sending to the arduino.

  This will also tag usernames and hashtags for display as different
colours. !!!FIXME!!!

  References:
  http://stackoverflow.com/questions/2403520/
*/

package arduino_json

import (
    "os"
    "fmt"
    "json"
)

type results struct {
    Results []tweet
}

type tweet struct {
    From_user string
    Id_str string
    Text string
}

func get_json_data(tw_str []byte ) (results, os.Error) {
    var tv = results{}
    err := json.Unmarshal(tw_str, &tv)  
    return tv, err
}


func RewriteTwitterJSON( tw_str []byte ) string {
    retstr := "nothing"
 
    data, err := get_json_data(tw_str)
    if err != nil {
        return err.String()
    }
    
    for _, res := range data.Results {
        fmt.Println(res.From_user)
        retstr += fmt.Sprintf("<P>User: %s", res.From_user)
        retstr += fmt.Sprintf("<BR><PRE>%s</PRE>", res.Text )
    }
    return retstr
}
