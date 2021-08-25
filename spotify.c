/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

void spotify_show_current_song(struct spotify_args *cmd_args)
{
  /* hit the endpoint, get the response and parse */
  spotify_http(cmd_args);
  parse_currently_playing(cmd_args->search_struct->spotify_json_res);

  printList();

  /* cleanup */
  free(cmd_args->search_struct->spotify_json_res);
  clear_search_list();
}

void spotify_display_tops(struct spotify_args *cmd_args)
{
  spotify_http(cmd_args);

  parse_top_tracks(cmd_args->search_struct->spotify_json_res);
  free(cmd_args->search_struct->spotify_json_res);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/top/artists?limit=10";
  spotify_http(cmd_args);
  parse_top_artists(cmd_args->search_struct->spotify_json_res);
  print_top_tracks();

  free(cmd_args->search_struct->spotify_json_res);
}

void cleanup_playlist(spotify_args *cmd_args)
{
  free(cmd_args->search_struct->spotify_json_res);
  free(cmd_args->search_struct->jsonObj);
  clear_playlist();
}

void change_playlist_to_play_command(spotify_args *cmd_args)
{
  /* since the response object gives us the endpoint in the response object,  */
  /* we can return the string to it */
  cmd_args->endpoint = parse_selected_playlist_json(cmd_args->search_struct->spotify_json_res);
  u_int8_t playlist_position = print_playlist_songs();
  cmd_args->search_struct->jsonObj = build_put_request_playlist(cmd_args->endpoint, playlist_position);

  cmd_args->http_type = PUT;
  cmd_args->http_get_write = false;

  free(cmd_args->endpoint);

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
}

void spotify_play_playlist(struct spotify_args *cmd_args)
{
  /* retrieve all playlists */
  spotify_http(cmd_args);

  parse_playlist_json(cmd_args->search_struct->spotify_json_res); 
  free(cmd_args->search_struct->spotify_json_res);

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

void build_related_artist_url(spotify_args *cmd_args)
{
  char *artist_id = parse_artist_id(cmd_args->search_struct->spotify_json_res);
  cmd_args->endpoint = "https://api.spotify.com/v1/artists/";
  cmd_args->search_struct->search_query = artist_id;
  cmd_args->endpoint = build_search_query(cmd_args);

  char* temp_endpoint = cmd_args->endpoint;

  cmd_args->search_struct->search_query = "/related-artists";
  cmd_args->endpoint = build_search_query(cmd_args);

  free(cmd_args->search_struct->spotify_json_res);
  free(temp_endpoint);
  cmd_args->endpoint[73] = 0;
  free(artist_id);
}

void cleanup_related_artists(spotify_args *cmd_args)
{
  free(cmd_args->endpoint);
  free(cmd_args->search_struct->spotify_json_res);
  clear_related_artists();
}

void spotify_show_related_artists(struct spotify_args *cmd_args)
{
  spotify_http(cmd_args);

  build_related_artist_url(cmd_args);

  spotify_http(cmd_args);

  parse_related_artists(cmd_args->search_struct->spotify_json_res);
  print_related_artists();

  cleanup_related_artists(cmd_args);
}

void spotify_play_song(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  free(cmd_args->endpoint);
  parse_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(5);
  free(cmd_args->search_struct->spotify_json_res);

  /* once we have the neccesary info, we change the endpoint and request */
  cmd_args->http_type = PUT;

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";

  cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);

  cmd_args->http_get_write = false;
  spotify_http(cmd_args);

  free(cmd_args->search_struct->jsonObj);
  clear_search_list();
}

void spotify_play_searched_artist(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  parse_related_artists(cmd_args->search_struct->spotify_json_res);
  print_related_artists(5);
  /* printf("%s\n", cmd_args->search_struct->spotify_json_res); */

  /* once we have the neccesary info, we change the endpoint and request */
  /* cmd_args->http_type = PUT; */
  /* cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play"; */
  /* cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position); */
  /* spotify_http(cmd_args); */
  /* free(cmd_args->search_struct->jsonObj); */
  /* clear_search_list(); */
}

void recents_change_to_play(spotify_args *cmd_args, search_song_request req)
{
  cmd_args->http_type = PUT;
  cmd_args->http_get_write = false;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);
}

void spotify_play_recents(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* fetch previous songs and parse */
  spotify_http(cmd_args);
  parse_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(20);
  free(cmd_args->search_struct->spotify_json_res);

  recents_change_to_play(cmd_args, req);

  /* once we have the neccesary info, we change the endpoint and request */
  spotify_http(cmd_args);

  free(cmd_args->search_struct->jsonObj);
  clear_search_list();
}

void change_queue_to_play(spotify_args *cmd_args, search_song_request req)
{
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->search_struct->search_query = req.track_info;
  cmd_args->http_type = POST;
  cmd_args->http_get_write = false;

  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/queue?uri=spotify:track:";
  cmd_args->endpoint = build_search_query(cmd_args);
}

void spotify_add_song_queue(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* fetch songs and parse */
  cmd_args->search_struct->query_type = SONG_QUERY;
  cmd_args->endpoint = build_search_query(cmd_args);

  spotify_http(cmd_args);

  parse_queue_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(5);
  free(cmd_args->endpoint);

  /* once we have the neccesary info, we change the endpoint and request */
  change_queue_to_play(cmd_args, req);

  spotify_http(cmd_args);

  free(cmd_args->search_struct->spotify_json_res);
  free(cmd_args->endpoint);
}

void spotifyC(struct spotify_args *cmd_args, struct search_song_request req)
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

void init_cmd_args(spotify_args *cmd_args)
{
  cmd_args->endpoint = NULL;
  cmd_args->http_get_write = false;
  cmd_args->http_type = 0;
  cmd_args->spotify_command = 0;
}

void init_search(spotify_search *search)
{
  search->spotify_json_res = NULL;
  search->jsonObj = NULL;
  search->search_query = NULL;
  search->query_type = 0;
}

void free_args_and_search(spotify_args *cmd_args, spotify_search *search)
{
  free(search);
  free(cmd_args);
}

int main(int argc, char **argv) {

  /* from get opt */
  extern int optind;
  extern char *optarg;

  /* instantiate the struct for the command line and search queries */
  spotify_args *cmd_args = (spotify_args*) malloc(sizeof(spotify_args));
  spotify_search *search = (spotify_search*) malloc(sizeof(spotify_search));

  init_cmd_args(cmd_args);
  init_search(search);

  /* hold the parsed song info that will be used to play song later */
  struct search_song_request req = {
    .track_info = NULL,
    .track_position = NULL
  };

  /* link our command struct to the search */
  cmd_args->search_struct = search;

  /* process the command line options */
  if(argc > 1)
  {
    process_args(argc, argv, cmd_args);

    /* call http and parse */
    spotifyC(cmd_args, req);
    free_args_and_search(cmd_args, search);
  } 
  else
  {
    print_help();
    free_args_and_search(cmd_args, search);
  }

  return 0;
}
