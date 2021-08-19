#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H

#include "includes.h"
#include "spotify_structs.h"

void insert_node(char *, ssize_t size);
void insert_search_node(struct available_song_node *);
void insert_playlist_node(struct spotify_playlist *);
void insert_playlist_song_node(struct spotify_playlist_songs *);
void printList();
char *print_playlist();
u_int8_t print_playlist_songs();
struct search_song_request print_avaible_songs(u_int8_t);
void clear_linked_list();
void clear_search_list();
void clear_playlist();

#endif
