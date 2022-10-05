// HTTP REST API to receive the ID as an int32 and the Distance as a float32 float from the bins

package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

type Message struct {
	ID       int32
	Distance float32
	Message  string
}

func main() {
	http.HandleFunc("/", handler)
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("method:", r.Method)
	if r.Method == "POST" {
		var m Message
		body, err := ioutil.ReadAll(r.Body)
		if err != nil {
			fmt.Println(err)
		}
		err = json.Unmarshal(body, &m)
		if err != nil {
			fmt.Println(err)
		}
		fmt.Println("ID:", m.ID)
		fmt.Println("Distance:", m.Distance)
		fmt.Fprintf(w, "You sent the id %d and the Distance %f\n", m.ID, m.Distance)
	}
}
