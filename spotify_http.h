#ifndef SPOTIFY_HTTP_H
#define SPOTIFY_HTTP_H

#include "includes.h"
#include "spotify_structs.h"

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct);
char *get_json_from_server(char *, struct get_url);
void change_player_status(char *, bool);
void play_song(char *, char *);

#endif
