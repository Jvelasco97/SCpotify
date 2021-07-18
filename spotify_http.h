#ifndef SPOTIFY_HTTP_H
#define SPOTIFY_HTTP_H

#include "includes.h"

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct);
char *display_currently_playing();
void change_player_status(char *, int *);

#endif
