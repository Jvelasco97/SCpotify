#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H

#include "includes.h"

void insert_node(char *, ssize_t size);
void insert_search_node(char *, char *, char *, char *);
void printList();
struct search_song_request print_avaible_songs();
void clear_linked_list();
void clear_search_list();

#endif
