/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"
#include <getopt.h> /* getopt */

/**
 * process the arguements passed so that the proper
 * http request can be performed
 * @param argc - number of arguements
 * @param argv - the values in the command line
 * @param args - the stated of the player that we eish to manipulate
 * @param put_post - since the player api has a POST or PUT, i use a
 * boolean value to determine which one is being used
 */
char *process_args(int argc, 
		   char **argv, 
		   struct args_type *args,
		   struct get_url *url_args,
                   bool *put_post) 
{
  int choice;
  int option_index = 0;

  static struct option long_options[] = 
  {
      {"show",     no_argument, 	NULL,  0 },
      {"repeat",     required_argument, NULL,  0 },
      {"shuffle",    required_argument, NULL,  0 },
      {"seek",       required_argument, NULL,  0 },
      {"vol",        required_argument, NULL,  0 },
      {"recent",     no_argument,       NULL,  0 },
      {NULL,         0,                 NULL,  0 }
  };

  while ((choice = getopt_long(argc, argv, "prnbs:t", long_options, &option_index)) != EOF) 
  {
    switch (choice) 
    {
    case 0:
      args->modify_player = true;
      /* printf("long option %s", long_options[option_index].name); */
      if(strcmp("repeat", long_options[option_index].name) == 0) 
      {
	*put_post = true;

	if(strcmp("track", optarg) == 0)
	{
	  return "repeat?state=track";
	}
	else if(strcmp("context", optarg) == 0)
	{
	  return "repeat?state=context";
	}
	else if(strcmp("off", optarg) == 0)
	{
	  return "repeat?state=off";
	}
      } 
      else if(strcmp("shuffle", long_options[option_index].name) == 0)
      {
	*put_post = true;
	if(strcmp("t", optarg) == 0)
	{
	  return "shuffle?state=true";
	}
	else if(strcmp("f", optarg) == 0)
	{
	  return "shuffle?state=false";
	}
      }
      else if(strcmp("seek", long_options[option_index].name) == 0)
      {
	*put_post = true;
	char * endpoint;
	const char * url = "seek?position_ms=";

	if ((endpoint = malloc(strlen(url) + strlen(optarg) + 1)) != NULL) {
	  endpoint[0] = '\0';

	  strcat(endpoint, url);

	  strcat(endpoint, optarg);
	} else {
	  printf("malloc failed!\n");
	}

	return endpoint;
      }
      else if(strcmp("vol", long_options[option_index].name) == 0)
      {
	*put_post = true;
	char * endpoint;
	const char * url = "volume?volume_percent=";

	if ((endpoint = malloc(strlen(url) + strlen(optarg) + 1)) != NULL) {
	  endpoint[0] = '\0';

	  strcat(endpoint, url);

	  strcat(endpoint, optarg);
	} else {
	  printf("malloc failed!\n");
	}

	return endpoint;
      }
      else if(strcmp("show", long_options[option_index].name) == 0)
      {
	args->modify_player   = false;
	args->display_song    = true;
	url_args->display_url = true;
	return NULL;
	break;
      }
      break;
    /* pause player  -- PUT*/
    case 'p':
      printf("\nNow pausing\n");
      args->modify_player = true;
      *put_post = true;
      return "pause";
      break;
    /* resume player -- PUT*/
    case 'r':
      printf("\nNow resuming\n");
      *put_post = true;
      args->modify_player = true;
      return "play";
      break;
    /* next track -- POST*/
    case 'n':
      printf("\nPlaying next track\n");
      put_post = false;
      args->modify_player = true;
      return "next";
      break;
    /* prev track -- POST*/
    case 'b':
      printf("\nPlaying previous track\n");
      put_post = false;
      args->modify_player = true;
      return "previous";
      break;
    /* search track -- GET */
    case 's':
      printf("\nSearching...\n\n");
      args->modify_change_song = true;
      url_args->search_url = true;
      return optarg;
      break;
    case 't':
      printf("\nfetching past songs...\n\n");
      args->show_recently_played = true;
      url_args->recent_url = true;
      return "recently-played?limit=9" ;
      break;
    /* unknown opt, just default to display */
    default:
      args->display_song = true;
      url_args->display_url = true;
      return NULL;
      break;
    }
  }

  return NULL;
}

int main(int argc, char **argv) 
{

  /* from get opt */
  extern int optind;
  extern char *optarg;

  struct args_type cmd_args;
  cmd_args.modify_player = false;
  cmd_args.modify_change_song = false;
  cmd_args.display_song = false;
  cmd_args.show_recently_played = false;
  
  struct get_url url_args;
  url_args.display_url = false;
  url_args.recent_url  = false;
  url_args.search_url  = false;

  bool req_type = false;

  /* opt returns the endpoint */
  char *opt = process_args(argc, argv, &cmd_args, &url_args, &req_type);
  /* hold the json response */
  char *json_res;

  if (cmd_args.modify_change_song) 
  {
    /* perform curl with endpoint @ search */
    json_res = get_json_from_server(opt, url_args);
    parse_search_info(json_res);
    struct search_song_request req = print_avaible_songs();
    play_song(req.track_info, req.track_position);
    clear_search_list();
    /* free the json response from get_json_from_server */
    free(json_res);
  } 
  else if(cmd_args.show_recently_played) 
  {
    json_res = get_json_from_server(opt, url_args);
    parse_search_info(json_res);
    struct search_song_request req = print_avaible_songs();
    play_song(req.track_info, req.track_position);
    clear_search_list();
    free(json_res);
  } 
  else if(cmd_args.modify_player) 
  {
    /* perform curl with enndpoint to modify player */
    change_player_status(opt, req_type);
  } 
  else if(cmd_args.display_song)
  {
    /* perform curl with endpoint @ currently-playing */
    json_res = get_json_from_server(opt, url_args);
    parse_currently_playing(json_res);
    printList();
    clear_linked_list();
    /* free the json response from get_json_from_server */
    free(json_res);
  }

  return 0;
}
