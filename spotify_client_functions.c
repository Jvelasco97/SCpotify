/* #include "spotify_client_functions.h" */
#include "config.h"
#include "spotify_client_functions.h"
#include "spotify_command_defines.h"
#include "spotify_commands.h"
#include "spotify_http.h"
#include "spotify_linked_lists.h"
#include "spotify_parsers.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include <stdio.h>
#include <stdlib.h>

#define AUTH_TOKEN_LENGTH 322
#define TOKEN_URL_LENGHT 209
#define CONFIG_TOKEN_DISPLACEMENT 14
#define CONFIG_ACCESS_TOKEN_LENGTH 307

#define STATUS_OK 200

/**
 * @brief gets the config values from the file
 * @param payload - the payload for thr http call
 * @return none
 */
void
get_config_values(spotify_auth_payload *payload)
{
  FILE *in;

  char buffer_in[324];

  in = fopen(token_location, "r");

  if (in == NULL)
	{
		printf("Error: could not open config file...\n");
    exit(EXIT_FAILURE);
  }

  payload->base64_client_secret = malloc(324);
  memcpy(payload->base64_client_secret, fgets(buffer_in, 324, in), 324);

  payload->refresh_token = malloc(324);
  memcpy(payload->refresh_token, fgets(buffer_in, 324, in), 324);

  payload->auth_token = malloc(324);
  memcpy(payload->auth_token, fgets(buffer_in, 324, in), 324);

  payload->auth_token[AUTH_TOKEN_LENGTH] = 0;

  fclose(in);
}

/**
 * @brief frees failed auth credentials
 * @param cmd_args
 * @return none
 */
void
free_previous_context(scpotify_context **cmd_args)
{
  free((*cmd_args)->auth_struct->auth_token);
  free((*cmd_args)->auth_struct->refresh_token);
  free((*cmd_args)->auth_struct->base64_client_secret);
  free((*cmd_args)->search_struct->spotify_json_response);
  free((*cmd_args)->endpoint);
}

/**
 * @brief set the context for the refrsh http call
 * @param cmd_args
 * @return none
 */
void
set_refresh_token_context(scpotify_context *cmd_args)
{
  /* TODO call concat striong instead of build_search */
  cmd_args->http_type = POST;
  cmd_args->endpoint = "https://accounts.spotify.com/api/token?grant_type=refresh_token&refresh_token=";
  cmd_args->search_struct->search_query = cmd_args->auth_struct->refresh_token + CONFIG_TOKEN_DISPLACEMENT;
  cmd_args->endpoint = build_search_query(cmd_args);
  cmd_args->search_struct->spotify_json_data = "{}";
  cmd_args->enable_write = true;

  cmd_args->endpoint[TOKEN_URL_LENGHT] = 0;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
write_new_token_to_config(char *token)
{
  FILE *in;

  char buffer_in[CONFIG_ACCESS_TOKEN_LENGTH];

  in = fopen(token_location, "r+");

  if (in == NULL)
	{
		printf("Error: failed to open file...\n");
    exit(EXIT_FAILURE);
  }

  int counter = 0;

  while (fgets(buffer_in, CONFIG_ACCESS_TOKEN_LENGTH, in) != NULL)
	{
    counter++;
    if (counter == 2)
		{
      snprintf(buffer_in, CONFIG_ACCESS_TOKEN_LENGTH, "%s\n", token);
      fwrite(token, sizeof(char), strlen(token), in);
    }
  }

  fclose(in);
}

void
free_refreshed_token(char *token)
{
	free(token);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
regenerate_token(scpotify_context *cmd_args)
{
  set_refresh_token_context(cmd_args);
  spotify_auth_http(cmd_args);

  char *token = parse_auth_token(&cmd_args->search_struct->spotify_json_response);
  free_previous_context(&cmd_args);
  write_new_token_to_config(token);

	free_refreshed_token(token);
}

void
free_spotify_json_response(char *response)
{
	free(response);
}


void
free_spotify_endpoint(char *endpoint)
{
	free(endpoint);
}
/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
refresh_oauth_token_and_resume(scpotify_context *cmd_args, u_int8_t http_type, char *endpoint)
{

	/* since authentication failed, we free this for the new response */
	free_spotify_json_response(cmd_args->search_struct->spotify_json_response);

  regenerate_token(cmd_args);

  /* reset the context of the command */
  get_config_values(cmd_args->auth_struct);

  cmd_args->http_type = http_type;
  cmd_args->endpoint = endpoint;

  spotify_http(cmd_args);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
transfer_playback(scpotify_context *cmd_args)
{
	cmd_args->endpoint = "https://api.spotify.com/v1/me/player";
	cmd_args->http_type = PUT;
	cmd_args->enable_write = true;

	cmd_args->search_struct->spotify_json_data = "{\"device_ids\":[\"25c86e6bbd56ddea209ff30b89abe3bbaeda06b4\"]}";

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
		if (spotify_http(cmd_args) != STATUS_OK)
		{
			printf("\nLooks like spotifyd isn't running...\n");
			exit(EXIT_FAILURE);
		}
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_list_choose_device(struct scpotify_context *cmd_args)
{
	cmd_args->endpoint = "https://api.spotify.com/v1/me/player/devices";
	cmd_args->http_type = GET;
	cmd_args->enable_write = true;

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

	printf("%s\n", cmd_args->search_struct->spotify_json_response);

  parse_device_list(cmd_args->search_struct->spotify_json_response);

  print_available_devices();


  /* cleanup */
  /* free(cmd_args->search_struct->spotify_json_response); */
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_show_current_song(struct scpotify_context *cmd_args)
{
  /* hit the endpoint, get the response and parse */
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  /* spotify_http(cmd_args); */
  parse_currently_playing(cmd_args->search_struct->spotify_json_response);

  spotify_now_playing_list();

  /* cleanup */
  clear_song_info_list();
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_display_tops(struct scpotify_context *cmd_args)
{
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_top_tracks(cmd_args->search_struct->spotify_json_response);

	free_spotify_json_response(cmd_args->search_struct->spotify_json_response);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/top/artists?limit=10";

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_top_artists(cmd_args->search_struct->spotify_json_response);
  print_top_tracks_list();

	free_spotify_json_response(cmd_args->search_struct->spotify_json_response);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
cleanup_playlist(scpotify_context *cmd_args)
{
  free(cmd_args->search_struct->spotify_json_response);
  clear_playlist_list();
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
change_playlist_to_play_command(scpotify_context *cmd_args)
{
  /* since the response object gives us the endpoint in the response object,  */
  /* we can return the string to it */
  free_spotify_endpoint(cmd_args->endpoint);

  cmd_args->endpoint = parse_selected_playlist_json(cmd_args->search_struct->spotify_json_response);
  u_int8_t playlist_position = print_playlist_songs_list();
  cmd_args->search_struct->spotify_json_data = build_put_request_playlist(cmd_args->endpoint, playlist_position);

  cmd_args->http_type = PUT;
  cmd_args->enable_write = false;

  free_spotify_endpoint(cmd_args->endpoint);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
}

void
spotify_retrieve_playlists(scpotify_context *cmd_args)    
{
  /* retrieve all playlists */
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_playlist_json(cmd_args->search_struct->spotify_json_response);
  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);

  /* get the playlist uri for the playlist endpoint */
  cmd_args->search_struct->search_query = print_playlist_list();
  cmd_args->endpoint = "https://api.spotify.com/v1/playlists/";

  /* append the search query and endpoint */
  cmd_args->endpoint = build_search_query(cmd_args);
}

void
spotify_display_playlist_songs_and_select(scpotify_context *cmd_args)
{
  /* retrieve all songs from specific playlist */
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  change_playlist_to_play_command(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  cleanup_playlist(cmd_args);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void spotify_play_playlist(struct scpotify_context *cmd_args)
{
	spotify_retrieve_playlists(cmd_args);
	spotify_display_playlist_songs_and_select(cmd_args);
}

void
free_spotify_author_id(char *id)
{
	free(id);
}

void
cleanup_related_artists_url(scpotify_context *cmd_args, char *temp_endpoint, char *artist_id)
{
  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);
  free_spotify_endpoint(temp_endpoint);
  cmd_args->endpoint[73] = 0;
  free_spotify_author_id(artist_id);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
build_related_artist_url(scpotify_context *cmd_args)
{
  char *artist_id = parse_artist_id(cmd_args->search_struct->spotify_json_response);
  cmd_args->endpoint = "https://api.spotify.com/v1/artists/";
  cmd_args->search_struct->search_query = artist_id;
  cmd_args->endpoint = build_search_query(cmd_args);

  char *temp_endpoint = cmd_args->endpoint;

  cmd_args->search_struct->search_query = "/related-artists";
  cmd_args->endpoint = build_search_query(cmd_args);

	cleanup_related_artists_url(cmd_args, temp_endpoint,  artist_id);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
cleanup_related_artists(scpotify_context *cmd_args)
{
  free(cmd_args->endpoint);
  free(cmd_args->search_struct->spotify_json_response);
  clear_related_artists_list();
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_show_related_artists(scpotify_context *cmd_args)
{
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  build_related_artist_url(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_related_artists(cmd_args->search_struct->spotify_json_response);
  print_related_artists_list();

  cleanup_related_artists(cmd_args);
}

void
free_spotify_json_data(char *data)
{
	free(data);
}

void
spotify_display_available_songs(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_search_info(cmd_args->search_struct->spotify_json_response);
	print_available_songs_list(5, req);
  /* free_spotify_json_response(cmd_args->search_struct->spotify_json_response); */
  free_spotify_endpoint(cmd_args->endpoint);
}

void
spotify_set_player_context_for_play_song(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  cmd_args->http_type = PUT;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play?device=25c86e6bbd56ddea209ff30b89abe3bbaeda06b4";
  cmd_args->search_struct->spotify_json_data = build_put_request(req->track_info, req->track_position);
  cmd_args->enable_write = false;
}

void
spotify_set_player_to_play_song(scpotify_context *cmd_args, spotify_song_query_info *req)
{

	spotify_set_player_context_for_play_song(cmd_args, req);
	
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  /* free_spotify_json_data(cmd_args->search_struct->spotify_json_data); */
  /* clear_song_info_list(); */
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_play_song(scpotify_context *cmd_args, spotify_song_query_info *req)
{
	spotify_display_available_songs(cmd_args, req);
	spotify_set_player_to_play_song(cmd_args, req);
}

void
spotify_query_podcasts(scpotify_context *cmd_args)    
{
  cmd_args->search_struct->query_type = PODCAST_QUERY;
  cmd_args->endpoint = build_search_query(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_shows(cmd_args->search_struct->spotify_json_response);
  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);
}

void
spotify_set_podcast_episode_context(scpotify_context *cmd_args)
{
  spotify_episode_query_info query = {
    .number_of_episodes = NULL,
    .podcast_id = NULL,
  };

  query = print_show_search_results_list(query);
  char *podcast_url = "https://api.spotify.com/v1/shows/";
  podcast_url = concat_strings(podcast_url, query.podcast_id);
  cmd_args->endpoint = concat_strings(podcast_url, "/episodes");

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  /* once we have the neccesary info, we change the endpoint and request */
  parse_podcast_episodes(cmd_args->search_struct->spotify_json_response);
  u_int16_t position = print_podcast_episodes_list();
  printf("Returned %d\n", position);

  free_spotify_endpoint(cmd_args->endpoint);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->spotify_json_data = build_put_request_episode(query.podcast_id, query.number_of_episodes, position);
  cmd_args->http_type = PUT;
  cmd_args->enable_write = false;
}

void
spotify_play_podcast_episode(scpotify_context *cmd_args)
{
	
  spotify_http(cmd_args);

  free_spotify_json_data(cmd_args->search_struct->spotify_json_data);
}


/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_search_podcast(struct scpotify_context *cmd_args)
{
	spotify_query_podcasts(cmd_args);
	spotify_set_podcast_episode_context(cmd_args);
	spotify_play_podcast_episode(cmd_args);
}

void
spotify_lookup_artists(scpotify_context *cmd_args)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_searched_artists(cmd_args->search_struct->spotify_json_response);
  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);
}

void
spotify_query_selected_artist(scpotify_context *cmd_args)
{
  char *artist_id = print_searched_artists_list();

  char *top_tracks_url = "https://api.spotify.com/v1/artists/";
  top_tracks_url = concat_strings(top_tracks_url, artist_id);
  cmd_args->endpoint = concat_strings(top_tracks_url, "/top-tracks?market=US");

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_search_info(cmd_args->search_struct->spotify_json_response);
  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);

  char *top_albums_url = "https://api.spotify.com/v1/artists/";
  top_albums_url = concat_strings(top_albums_url, artist_id);
  cmd_args->endpoint = concat_strings(top_albums_url, "/albums?market=US");
}

void
spotify_set_context_to_play_from_artist_search(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_albums(cmd_args->search_struct->spotify_json_response);
  print_searched_artist_results_list(10, req);

  /* /\* once we have the neccesary info, we change the endpoint and request *\/ */
  cmd_args->http_type = PUT;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->spotify_json_data =
  build_put_request(req->track_info, req->track_position);
}

void
spotify_play_song_from_selected_artist(scpotify_context *cmd_args)
{
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_play_searched_artist(scpotify_context *cmd_args, spotify_song_query_info *req)
{

	spotify_lookup_artists(cmd_args);
	
	spotify_query_selected_artist(cmd_args);

	spotify_set_context_to_play_from_artist_search(cmd_args, req);

	spotify_play_song_from_selected_artist(cmd_args);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
recents_change_to_play(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  cmd_args->http_type = PUT;
  cmd_args->enable_write = false;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->spotify_json_data = build_put_request(req->track_info, req->track_position);
}

void
spotify_query_recent_songs(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  /* build endpoint for search & parse needed info*/
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    /* modify auth struct */
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_search_info(cmd_args->search_struct->spotify_json_response);
	print_available_songs_list(20, req);

  recents_change_to_play(cmd_args, req);
}

void
spotify_play_recent_song(scpotify_context *cmd_args)
{
  /* once we have the neccesary info, we change the endpoint and request */
  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  clear_song_search_list();
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_play_recents(struct scpotify_context *cmd_args, struct spotify_song_query_info *req)
{
	spotify_query_recent_songs(cmd_args, req);
	spotify_play_recent_song(cmd_args);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
change_queue_to_play(scpotify_context *cmd_args, spotify_song_query_info *req)
{
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->search_struct->search_query = req->track_info;
  cmd_args->http_type = POST;
  cmd_args->enable_write = false;

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/queue?uri=spotify:track:";
  cmd_args->endpoint = build_search_query(cmd_args);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotify_add_song_queue(struct scpotify_context *cmd_args, struct spotify_song_query_info *req)
{
  /* fetch songs and parse */
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->endpoint = build_search_query(cmd_args);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  parse_queue_search_info(cmd_args->search_struct->spotify_json_response);
  print_available_songs_list(5, req);
  free_spotify_endpoint(cmd_args->endpoint);

  /* once we have the neccesary info, we change the endpoint and request */
  change_queue_to_play(cmd_args, req);

  if (spotify_http(cmd_args) != STATUS_OK)
	{
    refresh_oauth_token_and_resume(cmd_args, GET, cmd_args->endpoint);
  }

  free_spotify_json_response(cmd_args->search_struct->spotify_json_response);
  free_spotify_endpoint(cmd_args->endpoint);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
spotifyC(scpotify_context *cmd_args, spotify_song_query_info *req) 
{
  switch (cmd_args->spotify_command)
  {
  case SPOTIFY_RELATED_ARTISTS:
    spotify_show_related_artists(cmd_args);
    break;
  case SPOTIFY_CURRENTLY_PLAYING:
    spotify_show_current_song(cmd_args);
    break;
  case SPOTIFY_PLAY:
    spotify_http(cmd_args);
    break;
  case SPOTIFY_PLAYLIST:
    spotify_play_playlist(cmd_args);
    break;
  case SPOTIFY_MODIFY_PLAYER:
    spotify_http(cmd_args);
    break;
  case SPOTIFY_NEXT:
    spotify_http(cmd_args);
    break;
  case SPOTIFY_PREVIOUS:
    spotify_http(cmd_args);
    break;
  case SPOTIFY_SEARCH_SONGS:
    spotify_play_song(cmd_args, req);
    break;
  case SPOTIFY_SEARCH_PODCASTS:
    spotify_search_podcast(cmd_args);
    break;
  case SPOTIFY_SEARCH_ARTISTS:
    spotify_play_searched_artist(cmd_args, req);
    break;
  case SPOTIFY_HISTORY:
    spotify_play_recents(cmd_args, req);
    break;
  case SPOTIFY_QUEUE:
    spotify_add_song_queue(cmd_args, req);
    break;
  case SPOTIFY_TOPS:
    spotify_display_tops(cmd_args);
    break;
  default:
    print_help();
    exit(EXIT_SUCCESS);
    break;
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
init_search(spotify_search_context *search) 
{
  search->spotify_json_response = NULL;
  search->spotify_json_data = NULL;
  search->search_query = NULL;
  search->query_type = 0;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
init_cmd_args(scpotify_context *cmd_args) 
{
  cmd_args->endpoint = NULL;
  cmd_args->enable_write = false;
  cmd_args->http_type = 0;
  cmd_args->spotify_command = 0;

  init_search(cmd_args->search_struct);
  get_config_values(cmd_args->auth_struct);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
free_auth_payload(scpotify_context *cmd_args) 
{
  free(cmd_args->auth_struct->base64_client_secret);
  free(cmd_args->auth_struct->refresh_token);
  free(cmd_args->auth_struct->auth_token);
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
free_args_and_search(scpotify_context *cmd_args, spotify_search_context *search, spotify_auth_payload *auth) 
{

  free(cmd_args);
  free(auth);
}
