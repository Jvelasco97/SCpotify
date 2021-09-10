#include "spotify_client_functions.h"
#include "spotify_http.h"
#include "spotify_parsers.h"
#include "spotify_ll.h"
#include "spotify_structs.h"
#include "spotify_commands.h"
#include "spotify_command_defines.h"
#include "config.h"
#include <stdio.h>

#define AUTH_TOKEN_LENGTH 322
#define TOKEN_URL_LENGHT 209

void get_config_values(spotify_auth_payload *payload)
{
  FILE* in;

  char buffer_in[324];

  in = fopen(token_location, "r");

  if (in == NULL) {
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

void free_previous_context(scpotify_context **cmd_args)
{
  free((*cmd_args)->auth_struct->auth_token);
  free((*cmd_args)->auth_struct->refresh_token);
  free((*cmd_args)->auth_struct->base64_client_secret);
  free((*cmd_args)->search_struct->spotify_json_response);
  free((*cmd_args)->endpoint);
}

void set_refresh_token_context(scpotify_context *cmd_args)
{
  /* TODO call concat striong instead of build_search */
  cmd_args->http_type = POST;
  cmd_args->endpoint = "https://accounts.spotify.com/api/token?grant_type=refresh_token&refresh_token=";
  cmd_args->search_struct->search_query = cmd_args->auth_struct->refresh_token+14;
  cmd_args->endpoint = build_search_query(cmd_args);
  cmd_args->search_struct->spotify_json_data = "{}";
  cmd_args->enable_write = true;

  cmd_args->endpoint[TOKEN_URL_LENGHT] = 0;
}

void write_new_token_to_config(char *token)
{
  FILE* in;

  char buffer_in[256];

  in = fopen(token_location, "r+");

  if (in == NULL) {
      exit(EXIT_FAILURE);
  }

  int counter = 0;
  
  while(fgets(buffer_in, 256, in) != NULL)
  {
    counter++;
    if(counter == 2)
    {
      snprintf(buffer_in, 256, "%s\n", token);
      fwrite(token, sizeof(char), strlen(token), in);
      /* size_t bytesWrote = fwrite(token, sizeof(char), strlen(token), in); */
    }
  }

  fclose(in);
}

void regenerate_token(scpotify_context *cmd_args)
{
  set_refresh_token_context(cmd_args);

  spotify_auth_http(cmd_args);
  char* token = parse_auth_token(&cmd_args->search_struct->spotify_json_response);
  free_previous_context(&cmd_args);
  write_new_token_to_config(token);
  free(token);
}

void handle_token_regen(scpotify_context *cmd_args, u_int8_t http_type, char *endpoint)
{
  regenerate_token(cmd_args);

  /* reset the context of the command */
  get_config_values(cmd_args->auth_struct);
  cmd_args->http_type = http_type;
  cmd_args->endpoint = endpoint;

  spotify_http(cmd_args);
}

void spotify_show_current_song(struct scpotify_context *cmd_args)
{
  /* hit the endpoint, get the response and parse */
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  /* spotify_http(cmd_args); */
  parse_currently_playing(cmd_args->search_struct->spotify_json_response);

  printList();

  /* cleanup */
  free(cmd_args->search_struct->spotify_json_response);
  clear_search_list();
}

void spotify_display_tops(struct scpotify_context *cmd_args)
{
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  parse_top_tracks(cmd_args->search_struct->spotify_json_response);
  free(cmd_args->search_struct->spotify_json_response);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/top/artists?limit=10";
  spotify_http(cmd_args);
  parse_top_artists(cmd_args->search_struct->spotify_json_response);
  print_top_tracks();

  free(cmd_args->search_struct->spotify_json_response);
}

void cleanup_playlist(scpotify_context *cmd_args)
{
  free(cmd_args->search_struct->spotify_json_response);
  free(cmd_args->search_struct->spotify_json_data);
  clear_playlist();
}

void change_playlist_to_play_command(scpotify_context *cmd_args)
{
  /* since the response object gives us the endpoint in the response object,  */
  /* we can return the string to it */
  cmd_args->endpoint = parse_selected_playlist_json(cmd_args->search_struct->spotify_json_response);
  u_int8_t playlist_position = print_playlist_songs();
  cmd_args->search_struct->spotify_json_data = build_put_request_playlist(cmd_args->endpoint, playlist_position);

  cmd_args->http_type = PUT;
  cmd_args->enable_write = false;

  free(cmd_args->endpoint);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play?device_id=b49a5780a99ea81284fc0746a78f84a30e4d5c73";
}

void spotify_play_playlist(struct scpotify_context *cmd_args)
{
  /* retrieve all playlists */
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  parse_playlist_json(cmd_args->search_struct->spotify_json_response); 
  free(cmd_args->search_struct->spotify_json_response);

  /* get the playlist uri for the playlist endpoint */
  cmd_args->search_struct->search_query = print_playlist();
  cmd_args->endpoint = "https://api.spotify.com/v1/playlists/";

  /* append the search query and endpoint */
  cmd_args->endpoint = build_search_query(cmd_args);

  /* retrieve all songs from specific playlist */
  spotify_http(cmd_args);
  free(cmd_args->endpoint);

  change_playlist_to_play_command(cmd_args);

  spotify_http(cmd_args);

  cleanup_playlist(cmd_args);
}

void build_related_artist_url(scpotify_context *cmd_args)
{
  char *artist_id = parse_artist_id(cmd_args->search_struct->spotify_json_response);
  cmd_args->endpoint = "https://api.spotify.com/v1/artists/";
  cmd_args->search_struct->search_query = artist_id;
  cmd_args->endpoint = build_search_query(cmd_args);

  char* temp_endpoint = cmd_args->endpoint;

  cmd_args->search_struct->search_query = "/related-artists";
  cmd_args->endpoint = build_search_query(cmd_args);

  free(cmd_args->search_struct->spotify_json_response);
  free(temp_endpoint);
  cmd_args->endpoint[73] = 0;
  free(artist_id);
}

void cleanup_related_artists(scpotify_context *cmd_args)
{
  free(cmd_args->endpoint);
  free(cmd_args->search_struct->spotify_json_response);
  clear_related_artists();
}

void spotify_show_related_artists(struct scpotify_context *cmd_args)
{
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  build_related_artist_url(cmd_args);

  spotify_http(cmd_args);

  parse_related_artists(cmd_args->search_struct->spotify_json_response);
  print_related_artists();

  cleanup_related_artists(cmd_args);
}

void spotify_play_song(struct scpotify_context *cmd_args, struct spotify_song_query_info req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  parse_search_info(cmd_args->search_struct->spotify_json_response);
  req = print_avaible_songs(5);
  free(cmd_args->search_struct->spotify_json_response);

  /* once we have the neccesary info, we change the endpoint and request */
  cmd_args->http_type = PUT;

  free(cmd_args->endpoint);
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play?device_id=b49a5780a99ea81284fc0746a78f84a30e4d5c73";

  cmd_args->search_struct->spotify_json_data = build_put_request(req.track_info, req.track_position);

  cmd_args->enable_write = false;
  spotify_http(cmd_args);

  free(cmd_args->search_struct->spotify_json_data);
  clear_search_list();
}

void spotify_play_searched_artist(scpotify_context *cmd_args, spotify_song_query_info req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  parse_related_artists(cmd_args->search_struct->spotify_json_response);
  print_related_artists(5);
  req = print_avaible_songs(20);

  /* if(req.track_info == NULL) */
  /* { */
  /*   exit(EXIT_FAILURE); */
  /* } */
  /* printf("%s\n", cmd_args->search_struct->spotify_json_response); */

  /* once we have the neccesary info, we change the endpoint and request */
  /* cmd_args->http_type = PUT; */
  /* cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play"; */
  /* cmd_args->search_struct->spotify_json_data = build_put_request(req.track_info, req.track_position); */
  /* spotify_http(cmd_args); */
  /* free(cmd_args->search_struct->spotify_json_data); */
  /* clear_search_list(); */
}

void recents_change_to_play(scpotify_context *cmd_args, spotify_song_query_info req)
{
  cmd_args->http_type = PUT;
  cmd_args->enable_write = false;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play?device_id=b49a5780a99ea81284fc0746a78f84a30e4d5c73";
  cmd_args->search_struct->spotify_json_data = build_put_request(req.track_info, req.track_position);
}


void spotify_play_recents(struct scpotify_context *cmd_args, struct spotify_song_query_info req)
{
  /* build endpoint for search & parse needed info*/
  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  parse_search_info(cmd_args->search_struct->spotify_json_response);
  req = print_avaible_songs(20);
  free(cmd_args->search_struct->spotify_json_response);

  recents_change_to_play(cmd_args, req);

  /* once we have the neccesary info, we change the endpoint and request */
  spotify_http(cmd_args);

  free(cmd_args->search_struct->spotify_json_data);
  clear_search_list();
}

void change_queue_to_play(scpotify_context *cmd_args, spotify_song_query_info req)
{
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->search_struct->search_query = req.track_info;
  cmd_args->http_type = POST;
  cmd_args->enable_write = false;

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/queue?uri=spotify:track:";
  cmd_args->endpoint = build_search_query(cmd_args);
}

void spotify_add_song_queue(struct scpotify_context *cmd_args, struct spotify_song_query_info req)
{
  /* fetch songs and parse */
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->endpoint = build_search_query(cmd_args);

  if(spotify_http(cmd_args) != 200)
  {
    /* modify auth struct */
    free(cmd_args->search_struct->spotify_json_response);
    handle_token_regen(cmd_args, GET, cmd_args->endpoint);
  } 

  parse_queue_search_info(cmd_args->search_struct->spotify_json_response);
  req = print_avaible_songs(5);
  free(cmd_args->endpoint);

  /* once we have the neccesary info, we change the endpoint and request */
  change_queue_to_play(cmd_args, req);

  spotify_http(cmd_args);

  free(cmd_args->search_struct->spotify_json_response);
  free(cmd_args->endpoint);
}

void spotifyC(struct scpotify_context *cmd_args, struct spotify_song_query_info req)
{
  switch(cmd_args->spotify_command)
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
    case SPOTIFY_SEARCH_ARTISTS:
      spotify_play_searched_artist(cmd_args,req);
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

void init_search(spotify_search_context *search)
{
  search->spotify_json_response = NULL;
  search->spotify_json_data = NULL;
  search->search_query = NULL;
  search->query_type = 0;
}

void init_cmd_args(scpotify_context *cmd_args)
{
  cmd_args->endpoint = NULL;
  cmd_args->enable_write = false;
  cmd_args->http_type = 0;
  cmd_args->spotify_command = 0;

  init_search(cmd_args->search_struct);
  get_config_values(cmd_args->auth_struct);
}

void free_auth_payload(scpotify_context *cmd_args)
{
  free(cmd_args->auth_struct->base64_client_secret);
  free(cmd_args->auth_struct->refresh_token);
  free(cmd_args->auth_struct->auth_token);
}

void free_args_and_search(scpotify_context *cmd_args, spotify_search_context *search, spotify_auth_payload *auth)
{
  free(search);
  free(cmd_args);
  free(auth);
}
