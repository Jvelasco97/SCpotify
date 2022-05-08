#include "spotify_commands.h"
#include "includes.h"
#include "spotify_command_defines.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include <getopt.h>
#include <stdlib.h>

void 
print_help() 
{
  fprintf(
    stderr, "spotify cli tool.\n"
    "Usage: spotifyC [options] ...\n"
    "   --now               \t\t displays currently-playing song.\n"
    "   --now=a             \t\t displays related artists.\n"
    "   --plist             \t\t Will prompt all your saved playlists and will then ask you which one you would like, to listen from. \
        \n                      \t\t you will then be asked what song you want to play.\n"
    "   -h 		  \t\t Show this help message and quit.\n"
    "   -p 		  \t\t pauses current player\n"
    "   -r 		  \t\t resumes current player if paused.\n"
    "   -n  		  \t\t skips to next available track.\n"
    "   -b                  \t\t goes back to previous track.\n"
    "   -e                  \t\t fetch podcast episodes.\n"
    "   -s \"example song\" \t\t displays top 5 songs from query.\n"
    "   -a \"example artist\" \t\t displays top 5 artists from query.\n"
    "   -q \"example song\" \t\t displays top 5 songs from query & adds it "
    "to your player.\n"
    "   -t                  \t\t displays previous 20 songs & prompts choice "
    "for player.\n"
    "   --vol <0-100>       \t\t sets the volume of player. (--vol 80)\n"
    "   --seek [option]     \t\t moves the player to specifc time stamp in "
    "ms. (--seek 23000)\n"
    "   --repeat [option]   \t\t sets the repeat state, must be value of, "
    "context, track, or off. (--repeat track)\n"
    "   --shuffle [option]  \t\t sets the shuffle option, must be either "
    "true or false. (--shuffle true)\n"
    "   --top               \t\t fetches your top 10 artists and tracks.\n"
    "\nIssue are welcomed @ https://github.com/Jvelasco97/SCpotify\n");
}

void 
set_spotify_context(struct scpotify_context *args, char *endpoint, int command, int http_type) 
{
  args->endpoint = endpoint;
  args->search_struct->spotify_json_data = "{}";
  args->spotify_command = command;
  args->http_type = http_type;
}

void 
enable_write_to_context(struct scpotify_context *args) 
{
  args->enable_write = true;
  args->search_struct->search_query = optarg;
}

static 
void change_player_state(struct scpotify_context *args) 
{
  args->spotify_command = SPOTIFY_MODIFY_PLAYER;
  args->search_struct->spotify_json_data = "{}";
  args->http_type = PUT;
}

void 
handle_long_options(const char *option, struct scpotify_context *args) 
{
  if (strcmp("repeat", option) == 0) 
  {
    if (strcmp("track", optarg) == 0) {
      args->endpoint =
          "https://api.spotify.com/v1/me/player/repeat?state=track";
    } else if (strcmp("context", optarg) == 0) {
      args->endpoint =
          "https://api.spotify.com/v1/me/player/repeat?state=context";
    } else if (strcmp("off", optarg) == 0) {
      args->endpoint = 
	  "https://api.spotify.com/v1/me/player/repeat?state=off";
    }
    change_player_state(args);
  } 

  else if (strcmp("shuffle", option) == 0) 
  {
    if (strcmp("t", optarg) == 0) 
    {
      args->endpoint =
          "https://api.spotify.com/v1/me/player/shuffle?state=true";
    }
    else if (strcmp("f", optarg) == 0) 
    {
      args->endpoint =
          "https://api.spotify.com/v1/me/player/shuffle?state=false";
    }

    change_player_state(args);
  }

  else if (strcmp("seek", option) == 0) 
  {
    args->endpoint = "https://api.spotify.com/v1/me/player/seek?position_ms=";
    args->endpoint = concat_strings(args->endpoint, optarg);
    change_player_state(args);
  }

  else if (strcmp("vol", option) == 0) 
  {
    args->endpoint =
        "https://api.spotify.com/v1/me/player/volume?volume_percent=";
    args->endpoint = concat_strings(args->endpoint, optarg);
    change_player_state(args);
  } 

  else if (strcmp("plist", option) == 0) 
  {
    args->endpoint = "https://api.spotify.com/v1/me/playlists?limit=30";
    args->spotify_command = SPOTIFY_PLAYLIST;
    args->http_type = GET;
    args->enable_write = true;
  } 

  else if (strcmp("now", option) == 0) 
  {
    args->endpoint = "https://api.spotify.com/v1/me/player/currently-playing";
    args->spotify_command = SPOTIFY_CURRENTLY_PLAYING;

    if (optarg) 
    {
      if (strcmp("a", optarg) == 0) 
      {
        args->spotify_command = SPOTIFY_RELATED_ARTISTS;
      }
    }

    args->http_type = GET;
    args->enable_write = true;
  } 

  else if (strcmp("top", option) == 0) 
  {
    args->endpoint = "https://api.spotify.com/v1/me/top/tracks?limit=10";
    args->spotify_command = SPOTIFY_TOPS;
    args->http_type = GET;
    args->enable_write = true;
  }
}

/**
 * process the arguements passed so that the proper
 * http request can be performed
 * @param argc - number of arguements
 * @param argv - the values in the command line
 * @param args - the stated of the player that we eish to manipulate
 * boolean value to determine which one is being used
 */
void 
process_args(int argc, char **argv, struct scpotify_context *args) 
{
  int choice;
  int option_index = 0;

  static struct option long_options[] = 
  {
      {"now",     optional_argument, NULL, 0  },
      {"help",    no_argument,       NULL, 'h'},
      {"repeat",  required_argument, NULL, 0  },
      {"shuffle", required_argument, NULL, 0  },
      {"seek",    required_argument, NULL, 0  },
      {"vol",     required_argument, NULL, 0  },
      {"plist",   no_argument,       NULL, 0  },
      {"top",     no_argument,       NULL, 0  },
      {NULL,      0,                 NULL, 0  }
  };

  while ((choice = getopt_long(argc, argv, "prnbs:a:e:tq:", long_options, &option_index)) != EOF) 
  {
    switch (choice) 
    {
    case -1:
      exit(EXIT_FAILURE);
      break;
    case 0:
      handle_long_options(long_options[option_index].name, args);
      break;
    /* pause player  -- PUT*/
    case 'p':
      printf("\nNow pausing\n");
      set_spotify_context(args, "https://api.spotify.com/v1/me/player/pause",
                       SPOTIFY_MODIFY_PLAYER, PUT);
      break;
    /* resume player -- PUT*/
    case 'r':
      printf("\nNow resuming\n");
      set_spotify_context(args, "https://api.spotify.com/v1/me/player/play",
                       SPOTIFY_PLAY, PUT);
      break;
    /* next track -- POST*/
    case 'n':
      printf("\nPlaying next track\n");
      set_spotify_context(args, "https://api.spotify.com/v1/me/player/next",
                       SPOTIFY_NEXT, POST);
      break;
    /* prev track -- POST*/
    case 'b':
      printf("\nPlaying previous track\n");
      set_spotify_context(args, "https://api.spotify.com/v1/me/player/previous",
                       SPOTIFY_PREVIOUS, POST);
      break;
    /* search track -- GET */
    case 's':
      printf("\nSearching for song...\n\n");
      args->endpoint = "https://api.spotify.com/v1/search?q=";

      set_spotify_context(
          args, "https://api.spotify.com/v1/search?q=", SPOTIFY_SEARCH_SONGS, GET);

      enable_write_to_context(args);
      args->search_struct->query_type = SONG_QUERY;
      break;
    case 'a':
      printf("\nSearching for artist...\n\n");
      args->endpoint = "https://api.spotify.com/v1/search?q=";
      set_spotify_context(args, "https://api.spotify.com/v1/search?q=", SPOTIFY_SEARCH_ARTISTS, GET);
      enable_write_to_context(args);
      args->search_struct->query_type = ARTIST_QUERY;
      break;
    case 't':
      printf("\nfetching past songs...\n\n");
      set_spotify_context(args,
                       "https://api.spotify.com/v1/me/player/recently-played",
                       SPOTIFY_HISTORY, GET);
      args->enable_write = true;
      break;
    case 'e':
      printf("\nSearching for podcast episodes...\n\n");

      set_spotify_context(
          args, "https://api.spotify.com/v1/search?q=", SPOTIFY_SEARCH_PODCASTS, GET);

      enable_write_to_context(args);
      args->search_struct->query_type = PODCAST_QUERY;
      break;
    case 'q':
      printf(
          "\nSearching for a song that you want to add to the player...\n\n");
      set_spotify_context(
          args, "https://api.spotify.com/v1/search?q=", SPOTIFY_QUEUE, GET);
      enable_write_to_context(args);
      args->search_struct->query_type = DEFAULT_QUERY;
      break;
    case 'h':
      print_help();
      exit(EXIT_SUCCESS);
      break;

    /* unknown opt, just default to display */
    default:
      print_help();
      exit(EXIT_SUCCESS);
      break;
    }
  }
}
