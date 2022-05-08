#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H

#include "includes.h"
#include "spotify_structs.h"

void insert_song_info_node(char *, ssize_t);
void insert_node(char *, ssize_t size);
void insert_search_node(struct spotify_song_search_node *);
void insert_playlist_node(struct spotify_playlist_context *);
void insert_playlist_song_node(struct spotify_playlist_songs *);
void insert_top_track_node(struct spotify_top_tracks *);
void insert_top_artist_node(struct spotify_top_artists *);
void insert_related_artist_node(struct spotify_related_artists *);
void insert_artist_search_node(struct spotify_artists_search_node *);
void insert_album_search_node(struct spotify_album_search_node *);
void insert_show_search_node(struct spotify_podcast_search_node *);
void insert_episode_search_node(struct spotify_podcast_episode_node *);
void insert_device_list_node(struct spotify_device_list_node *);
void spotify_now_playing_list();
void print_top_tracks_list();
void print_available_devices();
void print_related_artists();
void print_artists_search_list();
void print_related_artists_list();
char *print_playlist();
char *print_searched_artists_list();
void print_top_tracks_list();
spotify_episode_query_info print_show_search_results(spotify_episode_query_info);
u_int16_t print_podcast_episodes();
void print_searched_artist_results(u_int8_t, spotify_song_query_info *);
spotify_episode_query_info print_show_search_results_list(spotify_episode_query_info);
void reverse_playlist_songs_list(spotify_playlist_songs**);
void reverse_top_tracks_list(spotify_top_tracks**);
void reverse_top_artists_list(spotify_top_artists** spotify_top_artist_head);
void reverse_podcast_episodes_list(spotify_podcast_episode_node**);
char * print_playlist_list();
void print_available_songs_list(u_int8_t, spotify_song_query_info *); 
void print_available_devices();
u_int8_t print_playlist_songs_list();
u_int16_t print_podcast_episodes_list();
void print_searched_artist_results_list(u_int8_t, spotify_song_query_info *);
void reverse_top_tracks(struct spotify_top_tracks **);
void reverse_top_artists(struct spotify_top_artists **);
u_int8_t print_playlist_songs();
void clear_song_search_list();
void clear_song_info_list();
void clear_linked_list();
void clear_playlist_list();
void clear_related_artists_list();

#endif
