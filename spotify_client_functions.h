#ifndef SPOTIFY_CLIENT_FUNCTIONS_H
#define SPOTIFY_CLIENT_FUNCTIONS_H

#include "spotify_structs.h"

void get_config_values(spotify_auth_payload *);
void regenerate_token(scpotify_context *);
void transfer_playback(scpotify_context *);
void refresh_oath_token_and_resume(scpotify_context *, u_int8_t , char *);
void spotify_show_current_song(scpotify_context *);
void spotify_display_top_songs_of_current_year(scpotify_context *);
void spotify_play_podcast(scpotify_context *);
void cleanup_playlist(scpotify_context *);
void change_playlist_to_play_context(scpotify_context *);
void spotify_play_playlist(scpotify_context *);
void build_related_artist_url(scpotify_context *);
void cleanup_related_artists(scpotify_context *);
void spotify_show_related_artists(scpotify_context *);
void spotify_play_song(scpotify_context *, spotify_song_query_info *);
void spotify_play_searched_artist(scpotify_context *, spotify_song_query_info *);
void recents_change_to_play_context(scpotify_context *, spotify_song_query_info *);
void spotify_play_recents(scpotify_context *, spotify_song_query_info *);
void change_queue_to_play(scpotify_context *, spotify_song_query_info *);
void spotify_add_song_queue(scpotify_context *, spotify_song_query_info *);
void spotifyC(scpotify_context *, spotify_song_query_info *);
void init_search(spotify_search_context *);
void init_cmd_args(scpotify_context *);
void free_refreshed_token(char *);
void free_auth_payload(scpotify_context *);
void free_args_and_search(scpotify_context *, spotify_search_context *, spotify_auth_payload *);
void free_previous_context(scpotify_context **);
void free_spotify_json_response(char *);
void free_spotify_json_data(char *);

#endif
