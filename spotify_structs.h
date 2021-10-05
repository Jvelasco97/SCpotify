#ifndef SPOTIFY_STRUCTS_H
#define SPOTIFY_STRUCTS_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>


typedef struct scpotify_context
{
  u_int8_t http_type;
  u_int8_t spotify_command;
  
  bool enable_write;

  char *endpoint;

  struct spotify_search_context *search_struct;
  struct spotify_auth_payload *auth_struct;
} scpotify_context;

/* comand line arguements types */
typedef struct spotify_search_context
{
  char *search_query;
  char *spotify_json_response;
  char *spotify_json_data;

  int  query_type;
} spotify_search_context;

typedef struct spotify_playlist_context
{
  char *playlist_name;
  char *playlist_owner;
  char *playlist_uri;

  struct spotify_playlist_context *next;
} spotify_playlist_context;

typedef struct spotify_playlist_songs 
{
  char *playlist_song_name;
  char *playlist_artist_name;
  char *playlist_album_name;

  struct spotify_playlist_songs *next;
} spotify_playlist_songs;

typedef struct spotify_url_query_type
{
  bool display_url;
  bool search_url;
  bool recent_url;
  bool queue_url;
} spotify_url_query_type;

/* holds the received data from the packets */
typedef struct spotify_json_data 
{
  char *data;
  size_t current_size;
  size_t allocated_max_size;
} spotify_json_data;

/* put parsed song information because strings are scary in C, and it helps in the long run */
typedef struct song_info_node 
{
   char *data;
   struct song_info_node *next;
} song_info_node;

/* holds all the info for the node whhich displays what info we have available */
typedef struct spotify_song_search_node
{
  char *album_api_info;
  char *album_name;
  char *artist_info;
  char *album_position;
  char *song_title;

  struct spotify_song_search_node *next;
} spotify_song_search_node;

typedef struct spotify_album_search_node
{
  char *album_id;
  char *album_name;

  struct spotify_album_search_node *next;
} spotify_album_search_node;

typedef struct spotify_podcast_search_node
{
  char *description;
  char *podcast_id; 
  char *name;
  char *publisher;
  char *number_of_episodes;

  struct spotify_podcast_search_node *next;
} spotify_podcast_search_node;

typedef struct spotify_podcast_episode_node
{
  char *description;
  char *duration_ms;
  char *episode_id; 
  char *name;
  char *release_date;

  struct spotify_podcast_episode_node *next;
} spotify_podcast_episode_node;

/* holds info about the parsed json that we want to play the track for */
typedef struct spotify_song_query_info 
{
  char *track_info;
  char *track_position;
} spotify_song_query_info;

typedef struct spotify_episode_query_info 
{
  char *podcast_id;
  char *number_of_episodes;
} spotify_episode_query_info;

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

typedef struct spotify_auth_payload
{
  char *base64_client_secret;
  char *refresh_token;
  char *auth_token;
} spotify_auth_payload;

typedef struct spotify_artists_search_node
{
  char *genre_array;
  char *artist_name;
  char *artists_id;

  struct spotify_artists_search_node *spotify_artists_search_node;
} spotify_artists_search_node; 

#endif
