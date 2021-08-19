/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

void spotify_play_playlist(struct spotify_args *cmd_args)
{
  /* retrieve all playlists */
  spotify_http(cmd_args);
  parse_playlist_json(cmd_args->search_struct->spotify_json_res); 

  /* get the playlist uri for the playlist endpoint */
  cmd_args->search_struct->search_query = print_playlist();
  cmd_args->endpoint = "https://api.spotify.com/v1/playlists/";

  /* append the search query and endpoint */
  cmd_args->endpoint = build_search_query(cmd_args);

  /* retrieve all songs from specific playlist */
  spotify_http(cmd_args);

  /* since the response object gives us the endpoint in the response object,  */
  /* we can return the string to it */
  cmd_args->endpoint = parse_selected_playlist_json(cmd_args->search_struct->spotify_json_res);
  u_int8_t playlist_position = print_playlist_songs();
  cmd_args->search_struct->jsonObj = build_put_request_playlist(cmd_args->endpoint, playlist_position);

  /* change http type to PUT as we are going to play a song */
  cmd_args->http_type = PUT;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  spotify_http(cmd_args);
}

void spotify_show_current_song(struct spotify_args *cmd_args)
{
  spotify_http(cmd_args);
  parse_currently_playing(cmd_args->search_struct->spotify_json_res);
  printList();
  clear_search_list();
}

void spotify_play_song(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* build endpoint for search & parse needed info*/
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  parse_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(5);

  /* once we have the neccesary info, we change the endpoint and request */
  cmd_args->http_type = PUT;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);
  spotify_http(cmd_args);
  clear_search_list();
}

void spotify_play_recents(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* fetch previous songs and parse */
  spotify_http(cmd_args);
  parse_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(20);

  /* once we have the neccesary info, we change the endpoint and request */
  cmd_args->http_type = PUT;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
  cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);
  spotify_http(cmd_args);
  clear_search_list();
}

void spotify_add_song_queue(struct spotify_args *cmd_args, struct search_song_request req)
{
  /* fetch songs and parse */
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  parse_queue_search_info(cmd_args->search_struct->spotify_json_res);
  req = print_avaible_songs(5);
  free(cmd_args->endpoint);

  /* once we have the neccesary info, we change the endpoint and request */
  cmd_args->search_struct->is_query = false;
  cmd_args->search_struct->search_query = req.track_info;
  cmd_args->http_type = POST;
  cmd_args->endpoint = "https://api.spotify.com/v1/me/player/queue?uri=spotify:track:";
  cmd_args->endpoint = build_search_query(cmd_args);
  spotify_http(cmd_args);
  free(cmd_args->endpoint);
  clear_search_list();
}

void spotifyC(struct spotify_args *cmd_args, struct search_song_request req)
{
  switch(cmd_args->spotify_command)
  {
    case SPOTIFY_CURRENTLY_PLAYING:
      spotify_show_current_song(cmd_args);
      break;
    case SPOTIFY_PLAY:
      spotify_http(cmd_args);
      break;
    case SPOTIFY_PLAYLIST:
      spotify_play_playlist(cmd_args);
      break;
    case SPOTIFY_PAUSE:
      spotify_http(cmd_args);
      break;
    case SPOTIFY_NEXT:
      spotify_http(cmd_args);
      break;
    case SPOTIFY_PREVIOUS:
      spotify_http(cmd_args);
      break;
    case SPOTIFY_SEARCH:
      spotify_play_song(cmd_args, req);
      break;
    case SPOTIFY_HISTORY:
      spotify_play_recents(cmd_args, req);
      break;
    case SPOTIFY_QUEUE:
      spotify_add_song_queue(cmd_args, req);
      break;
    default:
      break;
  }
}

int main(int argc, char **argv) {

  /* from get opt */
  extern int optind;
  extern char *optarg;

  /* instantiate the struct for the command line and search queries */
  struct spotify_args *cmd_args = (struct spotify_args*) malloc(sizeof(struct spotify_args));
  struct spotify_search *search = (struct spotify_search*) malloc(sizeof(struct spotify_search));

  /* hold the parsed song info that will be used to play song later */
  struct search_song_request req;

  /* link our command struct to the search */
  cmd_args->search_struct = search;

  /* process the command line options */
  process_args(argc, argv, cmd_args);

  /* call http and parse */
  spotifyC(cmd_args, req);

  free(search);
  free(cmd_args);

  return 0;
}
