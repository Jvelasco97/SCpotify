#ifndef SPOTIFY_STRUCTS_H
#define SPOTIFY_STRUCTS_H

/* #include "includes.h" */
#include <stdlib.h>
#include <stdbool.h>

struct args_type;
struct get_url;
struct json_data;
struct song_info_node;
struct available_song_node;
struct search_song_request;

/* comand line arguements types */
struct args_type 
{
  bool display_song;
  bool modify_player;
  bool modify_change_song;
  bool show_recently_played;
};

struct get_url
{
  bool display_url;
  bool search_url;
  bool recent_url;
};

/* holds the received data from the packets */
struct json_data 
{
  char *data;
  size_t current_size;
  size_t allocated_max_size;
};

/* put parsed song information because strings are scary in C, and it helps in the long run */
struct song_info_node 
{
   char *data;
   struct song_info_node *next;
};

/* holds all the info for the node whhich displays what info we have available */
struct available_song_node 
{
  char *album_api_info;
  char *album_name;
  char *artist_info;
  char *album_position;
  char *song_title;
  struct available_song_node *next;
};

/* holds info about the parsed json that we want to play the track for */
struct search_song_request 
{
  char *track_info;
  char *track_position;
};

#endif
