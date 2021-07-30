#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H

#include "includes.h"
#include "spotify_structs.h"

void insert_node(char *, ssize_t size);
void insert_search_node(struct available_song_node *);
void printList();
struct search_song_request print_avaible_songs();
void clear_linked_list();
void clear_search_list();

#endif
