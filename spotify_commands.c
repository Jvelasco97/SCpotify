#include "spotify_commands.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include "includes.h"
#include <getopt.h> 

#define SPOTIFY_CURRENTLY_PLAYING 0
#define SPOTIFY_PAUSE             1
#define SPOTIFY_PLAY              2
#define SPOTIFY_NEXT              3
#define SPOTIFY_PREVIOUS          4
#define SPOTIFY_SEARCH            5
#define SPOTIFY_HISTORY           6
#define SPOTIFY_QUEUE             7

#define GET    0
#define POST   1
#define PUT    2
#define DELETE 3

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
      {"show",       no_argument, 	NULL,  0  },
      {"help",       no_argument, 	NULL,  'h'},
      {"repeat",     required_argument, NULL,  0  },
      {"shuffle",    required_argument, NULL,  0  },
      {"seek",       required_argument, NULL,  0  },
      {"vol",        required_argument, NULL,  0  },
      {NULL,         0,                 NULL,  0  }
  };

  while ((choice = getopt_long(argc, argv, "prnbs:tq:", long_options, &option_index)) != EOF) 
  {
    switch (choice) 
    {
    case 0:
      if(strcmp("repeat", long_options[option_index].name) == 0) 
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

	args->search_struct->jsonObj = "{}";
	args->http_type = PUT;
      } 
      else if(strcmp("shuffle", long_options[option_index].name) == 0)
      {
	if(strcmp("t", optarg) == 0)
	{
	  args->endpoint = "https://api.spotify.com/v1/me/player/repeat?state=off";
	}
	else if(strcmp("f", optarg) == 0)
	{
	  args->endpoint = "https://api.spotify.com/v1/me/player/repeat?state=off";
	}

	args->search_struct->jsonObj = "{}";
	args->spotify_command = SPOTIFY_PAUSE;
	args->http_type = PUT;
      }
      else if(strcmp("seek", long_options[option_index].name) == 0)
      {
	  args->endpoint = "https://api.spotify.com/v1/me/player/seek?position_ms=";
	  args->endpoint = concat_strings(args->endpoint, optarg);
	  args->search_struct->jsonObj = "{}";
	  args->spotify_command = SPOTIFY_PAUSE;
	  args->http_type = PUT;
      }
      else if(strcmp("vol", long_options[option_index].name) == 0)
      {
	  args->endpoint = "https://api.spotify.com/v1/me/player/volume?volume_percent=";
	  args->endpoint = concat_strings(args->endpoint, optarg);
	  args->search_struct->jsonObj = "{}";
	  args->spotify_command = SPOTIFY_PAUSE;
	  args->http_type = PUT;
      }
      break;
    /* pause player  -- PUT*/
    case 'p':
      printf("\nNow pausing\n");
      args->endpoint = "https://api.spotify.com/v1/me/player/pause";
      args->search_struct->jsonObj = "{}";
      args->spotify_command = SPOTIFY_PAUSE;
      args->http_type = PUT;
      break;
    /* resume player -- PUT*/
    case 'r':
      printf("\nNow resuming\n");
      args->endpoint = "https://api.spotify.com/v1/me/player/play";
      args->search_struct->jsonObj = "{}";
      args->spotify_command = SPOTIFY_PLAY;
      args->http_type = PUT;
      break;
    /* next track -- POST*/
    case 'n':
      printf("\nPlaying next track\n");
      args->endpoint = "https://api.spotify.com/v1/me/player/next";
      args->spotify_command = SPOTIFY_NEXT;
      args->http_type = POST;
      break;
    /* prev track -- POST*/
    case 'b':
      printf("\nPlaying previous track\n");
      args->endpoint = "https://api.spotify.com/v1/me/player/previous";
      args->spotify_command = SPOTIFY_PREVIOUS;
      args->http_type = POST;
      break;
    /* search track -- GET */
    case 's':
      printf("\nSearching...\n\n");
      args->endpoint = "https://api.spotify.com/v1/search?q=";
      args->spotify_command = SPOTIFY_SEARCH;
      args->http_type = GET;
      args->http_get_write = true;
      args->search_struct->is_query = true;
      args->search_struct->search_query = optarg;
      break;
    case 't':
      printf("\nfetching past songs...\n\n");
      args->endpoint = "https://api.spotify.com/v1/me/player/recently-played";
      args->spotify_command = SPOTIFY_HISTORY;
      args->http_type = GET;
      args->http_get_write = true;
      break;
    case 'q':
      printf("\nSearching for a song that you want to add to the player...\n\n");
      args->endpoint = "https://api.spotify.com/v1/search?q=";
      args->search_struct->is_query = true;
      args->spotify_command = SPOTIFY_QUEUE;
      args->http_type = GET;
      args->http_get_write = true;
      args->search_struct->search_query = optarg;
      break;
    /* unknown opt, just default to display */
    default:
      args->http_get_write = true;
      break;
    }
  }
}
