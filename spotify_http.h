#ifndef SPOTIFY_HTTP_H
#define SPOTIFY_HTTP_H

#include "includes.h"

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct);
char *GET_currently_playing();

#endif
