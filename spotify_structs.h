#ifndef SPOTIFY_STRUCTS_H
#define SPOTIFY_STRUCTS_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct spotify_args 
{
  u_int8_t http_type;
  bool http_get_write;
  char *endpoint;
  u_int8_t spotify_command;
  struct spotify_search *search_struct;
} spotify_args;

/* comand line arguements types */
typedef struct spotify_search 
{
  char *search_query;
  char *spotify_json_res;
  char *jsonObj;
  int  query_type;
} spotify_search;

typedef struct spotify_playlist 
{
  char *playlist_name;
  char *playlist_owner;
  char *playlist_uri;
  struct spotify_playlist *next;
} spotify_playlist;

typedef struct spotify_playlist_songs 
{
  char *playlist_song_name;
  char *playlist_artist_name;
  char *playlist_song_album;
  struct spotify_playlist_songs *next;
} spotify_playlist_songs;

typedef struct get_url
{
  bool display_url;
  bool search_url;
  bool recent_url;
  bool queue_url;
} get_url;

/* holds the received data from the packets */
typedef struct json_data 
{
  char *data;
  size_t current_size;
  size_t allocated_max_size;
} json_data;

/* put parsed song information because strings are scary in C, and it helps in the long run */
typedef struct song_info_node 
{
   char *data;
   struct song_info_node *next;
} song_info_node;

/* holds all the info for the node whhich displays what info we have available */
typedef struct available_song_node 
{
  char *album_api_info;
  char *album_name;
  char *artist_info;
  char *album_position;
  char *song_title;
  struct available_song_node *next;
} available_song_node;

/* holds info about the parsed json that we want to play the track for */
typedef struct search_song_request 
{
  char *track_info;
  char *track_position;
} search_song_request;

typedef struct spotify_top_tracks
{
  char *song_name;
  char *artist_name;
  struct spotify_top_tracks *next_spotify_top_tracks;
} spotify_top_tracks;

typedef struct spotify_top_artists
{
  char *artist_name;
  struct spotify_top_artists *next_spotify_top_artists;
} spotify_top_artists;

typedef struct spotify_related_artists 
{
  char *genre_array;
  char *artist_name;
  char *artists_id;
  struct spotify_related_artists *spotify_related_artists_node;
} spotify_related_artists;

#endif
