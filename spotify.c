/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

#define SPOTIFY_CURRENTLY_PLAYING 0
#define SPOTIFY_PAUSE             1
#define SPOTIFY_PLAY              2
#define SPOTIFY_NEXT              3
#define SPOTIFY_PREVIOUS          4
#define SPOTIFY_SEARCH            5
#define SPOTIFY_HISTORY           6
#define SPOTIFY_QUEUE             7

#define  GET    0
#define  POST   1
#define  PUT    2
#define  DELETE 3

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
