#ifndef SPOTIFY_PARSERS_H
#define SPOTIFY_PARSERS_H

/* #include "includes.h" */
#include <stdlib.h>

ssize_t cut(char *artist_info);
void parse_currently_playing(char*);
void parse_search_info(char*);

#endif
