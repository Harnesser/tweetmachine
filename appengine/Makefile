APPENGINE_BIN=../../../../tmp/ae/google_appengine/

all:
	${APPENGINE_BIN}/dev_appserver.py milklabs_tweets

json:
	wget http://search.twitter.com/search.json?q=milklabs&result_type=mixed&count=10 
	mv search.json\?q=milklabs ./data/search_milklabs.json
