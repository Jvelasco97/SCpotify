#ifndef SPOTIFY_PARSERS_H
#define SPOTIFY_PARSERS_H

/* #include "includes.h" */
#include <stdlib.h>
#include "spotify_structs.h"

ssize_t cut(char *artist_info);
void parse_currently_playing(char*);
void parse_search_info(char*);
char *insert_parsed_data(char **, const char **, int);

#endif
