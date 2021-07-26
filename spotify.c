/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

bool put_post = false;

char* 
process_args(int argc, char **argv, struct args_type *args) 
{
  int choice;
  
  while ((choice = getopt(argc, argv, "prnbs:")) != EOF) {
    switch(choice) {
      /* pause player */
      case 'p':
	printf("Now pausing\n");
	args->modify_player = true;
	put_post = true;
	return "pause";
	break;
      /* resume player */
      case 'r':
	printf("Now resuming\n");
	put_post = true;
	args->modify_player = true;
	return "play";
	break;
      /* next track */
      case 'n':
	printf("Playing next track\n");
	put_post = false;
	args->modify_player = true;
	return "next";
	break;
      /* prev track */
      case 'b':
        printf("Playing previous track\n");
	put_post = false;
	args->modify_player = true;
        return "previous";
	break;
      case 's':
        printf("Searching...\n");
	args->modify_change_song = true;
        return optarg;
	break;
      /* unknown opt */
      default:
	args->display_song = true;
        return NULL;
    }
  }

  return NULL;
}

int 
main (int argc, char **argv) 
{

  extern int optind;
  extern char *optarg;

  struct args_type cmd_args;

  char *opt = process_args(argc, argv, &cmd_args);
  char *json_res;

  if(cmd_args.modify_change_song) {
    /* perform curl with endpoint @ search */
    json_res = get_json_from_server(opt); 
    parse_search_info(json_res);
    json_res = print_avaible_songs();
  } else if (cmd_args.modify_player) {
    /* perform curl with enndpoint to modify player */
    change_player_status(opt, put_post); 
  } else {
    /* perform curl with endpoint @ currently-playing */
    json_res = get_json_from_server(opt); 
    parse_currently_playing(json_res);
    printList();
  }

  return 0;
}
