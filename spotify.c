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


int main(int argc, char **argv) {

  /* from get opt */
  extern int optind;
  extern char *optarg;

  struct spotify_args *cmd_args = (struct spotify_args*) malloc(sizeof(struct spotify_args));
  struct spotify_search *search = (struct spotify_search*) malloc(sizeof(struct spotify_search));

  struct search_song_request req;

  cmd_args->search_struct = search;

  /* opt returns the endpoint */
  /* cmd_args->search_struct = search_args; */
  process_args(argc, argv, cmd_args);
  /* hold the json response */
  switch(cmd_args->spotify_command)
  {
    case SPOTIFY_CURRENTLY_PLAYING:
      spotify_http(cmd_args);
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
      cmd_args->endpoint = build_search_query(cmd_args);
      spotify_http(cmd_args);
      parse_search_info(cmd_args->search_struct->spotify_json_res);
      req = print_avaible_songs();
      cmd_args->http_type = 2;
      cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
      cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);
      spotify_http(cmd_args);
      clear_search_list();
      break;
    case SPOTIFY_HISTORY:
      spotify_http(cmd_args);
      parse_search_info(cmd_args->search_struct->spotify_json_res);
      req = print_avaible_songs();
      cmd_args->http_type = 2;
      cmd_args->endpoint = "https://api.spotify.com/v1/me/player/play";
      cmd_args->search_struct->jsonObj = build_put_request(req.track_info, req.track_position);
      spotify_http(cmd_args);
      clear_search_list();
      break;
    case SPOTIFY_QUEUE:
      cmd_args->endpoint = build_search_query(cmd_args);
      spotify_http(cmd_args);
      parse_queue_search_info(cmd_args->search_struct->spotify_json_res);
      req = print_avaible_songs();
      free(cmd_args->endpoint);
      cmd_args->search_struct->is_query = false;
      cmd_args->search_struct->search_query = req.track_info;
      cmd_args->http_type = 1;
      cmd_args->endpoint = "https://api.spotify.com/v1/me/player/queue?uri=spotify:track:";
      cmd_args->endpoint = build_search_query(cmd_args);
      spotify_http(cmd_args);
      /* clear_search_list(); */
      break;
    default:
      break;
  }

  return 0;
}
