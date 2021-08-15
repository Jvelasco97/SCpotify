#!/bin/sh

token=$(/usr/bin/curl -s -H "Authorization: Basic Zdjska...dsadsaYzc=" -d grant_type=refresh_token -d refresh_token=A......I6mU https://accounts.spotify.com/api/token)

parsed_token=$(/usr/bin/echo $token | /usr/bin/cut -c 18-312)

refresh=$(/usr/bin/echo "\"Authorization: Bearer "$parsed_token\"\;)

/usr/bin/sed -i 's/\"\"\;/\'"${refresh}"'/g' token.h

/usr/bin/make

/usr/bin/sudo cp spotifyC ~ && /usr/bin/sudo cp spotifyC /usr/local/bin

/usr/bin/make clean && /usr/bin/rm token.h

/usr/bin/echo const char token [] = \"\"\; > \Wherever you cloned the project to\
