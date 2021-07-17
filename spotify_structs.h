#ifndef SPOTIFY_STRUCTS_H
#define SPOTIFY_STRUCTS_H

#include "includes.h"

struct json_data;
struct song_info_node;

/* holds the received data from the packets */
struct json_data {
  char *data;
  size_t current_size;
  size_t allocated_max_size;
};

/* put parsed song information because strings are scary in C, and it helps in the long run */
struct song_info_node {
   char* data;
   struct song_info_node *next;
};

#endif
