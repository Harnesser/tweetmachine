/*
  Package to take JSON returned by a Twitter search and reduce it to 
a smaller size for sending to the arduino.

  This will also tag usernames and hashtags for display as different
colours. !!!FIXME!!!
 
*/

package arduino_json

import (
    "os"
    "fmt"
    "json"
    "regexp"
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
    retstr := ""
 
    // Make a datastructure from the JSON file
    data, err := get_json_data(tw_str)
    if err != nil {
        return err.String()
    }

    // Start a regexp so we can highlight 'milklabs'
    reg, err := regexp.Compile("milklabs")
    if err != nil {
        return "Can't build regexp"
    }
    
    // Build the Arduino-friendly twitter feed data
    for _, res := range data.Results {
        fmt.Println("%s", reg.String())
        
        // Tweeter 
        retstr += fmt.Sprintf("u:%s\n", res.From_user)
        
        // Tweet with 'milklabs' highlighted
        //tweet := res.Text
        tweet := reg.ReplaceAll( []byte(res.Text),
                   []byte("\ng:milklabs\no:") )
        
        //fmt.Println("%s", string(res.Text))       
        retstr += fmt.Sprintf("o:%s\n", string(tweet) ) 
        
    }
    return retstr
}
