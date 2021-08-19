# SCpotify

### Minimal wrapper in C for the spotify client. 

I wrote this because I use the web client, while I can simply  
switch to the site and use my vim keybindings to change songs  
and such, I decided I also wanted a way to show what song  
was playing in my polybar.  

![sample 1](samples/2021-07-15_18-34.png)
![sample 2](samples/2021-07-15_18-34_1.png)

you need to follow the guide [here](https://developer.spotify.com/documentation/general/guides/authorization-guide/) to setup your refresh token.

#### sample commands
`spotifyC --show`  
Will display currently playing song  

`spotifyC -n`  
Will play the next song  

`spotifyC -b`  
Will play the previous song  

`spotifyC -p`  
Will pause the song  

`spotifyC -r`  
Will reseume the song  
  
`spotifyC -s "example song"`  
Will search and prompt  

`spotifyC -t`  
Will display past 20 songs  

`spotifyC -q "example song"`  
Will seach and prompt what song to add in queue  

`spotifyC --plist`
Will prompt all your saved playlists and will then  
ask you which one you would like, to listen from.  
you will then be asked what song you want to play.  

![sample 3](samples/2021-07-26_17-34.png)

## TODO

### General
- [ ] add error checking  
- [X] reduce redundant code
- [X] add colors

### Player
- [X] shuffle functionality
- [X] repeat functionality
- [X] seek functionality (in ms for now)
- [X] volume functionality
- [X] previously played functionality
- [X] add song to current player

### Personalization
- [ ] display top artists and songs

### Artists
- [ ] display related artists 
- [ ] display artists and play song

### Albums
- [ ] search album and play song

### OAUTH  
- [ ] have the program handle OAUTH.
