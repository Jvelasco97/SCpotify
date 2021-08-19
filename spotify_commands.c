#include "spotify_commands.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include "includes.h"
#include "spotify_command_defines.h"
#include <getopt.h> 

void init_spotify_arg(struct spotify_args *args, char *endpoint, int command, int http_type)
{
  args->endpoint = endpoint;
  args->search_struct->jsonObj = "{}";
  args->spotify_command = command;
  args->http_type = http_type;
}

void enable_json_write(struct spotify_args *args)
{
  args->http_get_write = true;
  args->search_struct->is_query = true;
  args->search_struct->search_query = optarg;
}

static void change_player_status(struct spotify_args *args)
{
  args->spotify_command = SPOTIFY_PAUSE;
  args->search_struct->jsonObj = "{}";
  args->http_type = PUT;
}

void handle_case_zero(const char * option, struct spotify_args *args )
{
  if(strcmp("repeat", option) == 0) 
  {
    if(strcmp("track", optarg) == 0)
    {
      args->endpoint = "https://api.spotify.com/v1/me/player/repeat?state=track";
    }
    else if(strcmp("context", optarg) == 0)
    {
      args->endpoint = "https://api.spotify.com/v1/me/player/repeat?state=context";
    }
    else if(strcmp("off", optarg) == 0)
    {
      args->endpoint = "https://api.spotify.com/v1/me/player/repeat?state=off";
    }
    change_player_status(args);
  } 
  else if(strcmp("shuffle", option) == 0)
  {
    if(strcmp("t", optarg) == 0)
    {
      args->endpoint = "https://api.spotify.com/v1/me/player/shuffle?state=true";
    }
    else if(strcmp("f", optarg) == 0)
    {
      args->endpoint = "https://api.spotify.com/v1/me/player/shuffle?state=false";
    }
    change_player_status(args);
  }
  else if(strcmp("seek", option) == 0)
  {
    args->endpoint = "https://api.spotify.com/v1/me/player/seek?position_ms=";
    args->endpoint = concat_strings(args->endpoint, optarg);
    change_player_status(args);
  }
  else if(strcmp("vol", option) == 0)
  {
    args->endpoint = "https://api.spotify.com/v1/me/player/volume?volume_percent=";
    args->endpoint = concat_strings(args->endpoint, optarg);
    change_player_status(args);
  }
  else if(strcmp("plist", option) == 0)
  {
    args->endpoint = "https://api.spotify.com/v1/me/playlists?limit=30";
    args->spotify_command = SPOTIFY_PLAYLIST;
    args->http_type = GET;
    args->http_get_write = true;
  }
  else if(strcmp("now", option) == 0)
  {
    args->endpoint = "https://api.spotify.com/v1/me/player/currently-playing";
    args->spotify_command = SPOTIFY_CURRENTLY_PLAYING;
    args->http_type = GET;
    args->http_get_write = true;
  }
}
/**
 * process the arguements passed so that the proper
 * http request can be performed
 * @param argc - number of arguements
 * @param argv - the values in the command line
 * @param args - the stated of the player that we eish to manipulate
 * @param put_post - since the player api has a POST or PUT, i use a
 * boolean value to determine which one is being used
 */
void process_args(int argc, char **argv, struct spotify_args *args)
{
  int choice;
  int option_index = 0;

  static struct option long_options[] = 
  {
      {"now",        no_argument, 	NULL,  0  },
      {"help",       no_argument, 	NULL,  'h'},
      {"repeat",     required_argument, NULL,  0  },
      {"shuffle",    required_argument, NULL,  0  },
      {"seek",       required_argument, NULL,  0  },
      {"vol",        required_argument, NULL,  0  },
      {"plist",      no_argument,       NULL,  0  },
      {NULL,         0,                 NULL,  0  }
  };

  while ((choice = getopt_long(argc, argv, "prnbs:tq:", long_options, &option_index)) != EOF) 
  {
    switch (choice) 
    {
    case 0:
      handle_case_zero(long_options[option_index].name, args);
      break;
    /* pause player  -- PUT*/
    case 'p':
      printf("\nNow pausing\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/me/player/pause", SPOTIFY_PAUSE, PUT);
      break;
    /* resume player -- PUT*/
    case 'r':
      printf("\nNow resuming\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/me/player/play", SPOTIFY_PLAY, PUT);
      break;
    /* next track -- POST*/
    case 'n':
      printf("\nPlaying next track\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/me/player/next", SPOTIFY_NEXT, POST);
      break;
    /* prev track -- POST*/
    case 'b':
      printf("\nPlaying previous track\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/me/player/previous", SPOTIFY_PREVIOUS, POST);
      break;
    /* search track -- GET */
    case 's':
      printf("\nSearching...\n\n");
      args->endpoint = "https://api.spotify.com/v1/search?q=";
      init_spotify_arg(args, "https://api.spotify.com/v1/search?q=", SPOTIFY_SEARCH, GET);
      enable_json_write(args);
      break;
    case 't':
      printf("\nfetching past songs...\n\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/me/player/recently-played", SPOTIFY_HISTORY, GET);
      args->http_get_write = true;
      break;
    case 'q':
      printf("\nSearching for a song that you want to add to the player...\n\n");
      init_spotify_arg(args, "https://api.spotify.com/v1/search?q=", SPOTIFY_QUEUE, GET);
      enable_json_write(args);
      break;
    /* unknown opt, just default to display */
    default:
      break;
    }
  }
}
