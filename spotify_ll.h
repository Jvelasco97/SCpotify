#ifndef SPOTIFY_LL_H
#define SPOTIFY_LL_H

#include "includes.h"
#include "spotify_structs.h"

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
void printList();
void print_top_tracks();
void print_available_devices();
void print_related_artists();
char *print_searched_artists();
char *print_playlist();
struct spotify_episode_query_info print_show_search_results(spotify_episode_query_info);
u_int16_t print_podcast_episodes();
spotify_song_query_info print_searched_artist_results(u_int8_t);
void reverse_top_tracks(struct spotify_top_tracks **);
void reverse_top_artists(struct spotify_top_artists **);
u_int8_t print_playlist_songs();
spotify_song_query_info print_avaible_songs(u_int8_t);
void clear_linked_list();
void clear_search_list();
void clear_playlist();
void clear_related_artists();

#endif
