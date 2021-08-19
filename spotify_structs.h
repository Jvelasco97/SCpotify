#ifndef SPOTIFY_STRUCTS_H
#define SPOTIFY_STRUCTS_H

/* #include "includes.h" */
#include <stdlib.h>
#include <stdbool.h>

struct spotify_playlist_songs;
struct spotify_args;
struct spotify_playlist;
struct spotify_search;
struct get_url;
struct json_data;
struct song_info_node;
struct available_song_node;
struct search_song_request;

struct spotify_args 
{
  u_int8_t http_type;
  bool http_get_write;
  char *endpoint;
  u_int8_t spotify_command;
  struct spotify_search *search_struct;
};

/* comand line arguements types */
struct spotify_search 
{
  char *search_query;
  char *spotify_json_res;
  char *jsonObj;
  bool is_query;
};

struct spotify_playlist 
{
  char *playlist_name;
  char *playlist_owner;
  char *playlist_uri;
  struct spotify_playlist *next;
};

struct spotify_playlist_songs 
{
  char *playlist_song_name;
  char *playlist_artist_name;
  char *playlist_song_album;
  struct spotify_playlist_songs *next;
};

struct get_url
{
  bool display_url;
  bool search_url;
  bool recent_url;
  bool queue_url;
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
