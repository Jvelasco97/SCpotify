#!/bin/sh

token=$(/usr/bin/curl -s -H "Authorization: Basic ZmUyN2EyMGUyODhjNDcwZjlkOTMxZWM5MDE0MzRmZTg6MmYyMjAxNzlmY2FlNGQxODgxMmFiNDNiYzk1NmRiYzc=" -d grant_type=refresh_token -d refresh_token=AQAEhasQ1jse-6smdWbEY1DQykTlYgL-czP3cF2WpsDsHVwBtrYuuaxnHECiKDuwTFlOTOWqH7PkdYdIeG2SXJlU_4cS22VKS9btBDxjfPdukfONsvCALh60qkfm-dZI6mU https://accounts.spotify.com/api/token)

parsed_token=$(/usr/bin/echo $token | /usr/bin/cut -c 18-312)

refresh=$(/usr/bin/echo "\"Authorization: Bearer "$parsed_token\"\;)

/usr/bin/sed -i 's/\"\"\;/\'"${refresh}"'/g' token.h

/usr/bin/make

/usr/bin/sudo cp spotifyC ~ && /usr/bin/sudo cp spotifyC /usr/local/bin

/usr/bin/make clean && /usr/bin/rm token.h

/usr/bin/echo const char token [] = \"\"\; > /home/xerothyl/projects/spotify-polybar/token.h
