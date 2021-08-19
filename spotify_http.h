#ifndef SPOTIFY_HTTP_H
#define SPOTIFY_HTTP_H

#include "includes.h"
#include "spotify_structs.h"

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct);
char *get_json_from_server(char *, struct get_url);
void play_song(char *, char *);
void spotify_http(struct spotify_args *);
char *build_search_query(struct spotify_args *);
char *build_put_request(char *album_info, char* album_position);
char *build_put_request_playlist(char *, int);
#endif
