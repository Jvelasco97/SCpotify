#ifndef SPOTIFY_PARSERS_H
#define SPOTIFY_PARSERS_H

/* #include "includes.h" */
#include <stdlib.h>
#include "spotify_structs.h"

ssize_t cut(char *artist_info);
void parse_currently_playing(char*);
void parse_search_info(char*);
char *parse_artist_id(char*);
void parse_related_artists(char*);
void parse_queue_search_info(char*);
void parse_playlist_json(char *);
char *parse_selected_playlist_json(char *);
void parse_top_tracks(char *);
void parse_top_artists(char *);
void parse_related_artists(char *);
void parse_searched_artists(char *);
void parse_albums(char *);
void parse_shows(char *);
void parse_podcast_episodes(char *);
char *insert_parsed_data(char **, const char **, int, bool);
char *insert_parsed_queue_data(char **, const char **, int);
char *insert_parsed_playlist_info(char **, const char**, int, bool);
char *insert_top_artist(char **, const char **);
char *parse_auth_token(char **);

#endif
