/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

/**
 * process the arguements passed so that the proper 
 * http request can be performed
 * @param argc - number of arguements
 * @param argv - the values in the command line
 * @param args - the stated of the player that we eish to manipulate
 * @param put_post - since the player api has a POST or PUT, i use a 
 * boolean value to determine which one is being used
 */
char* 
process_args(int argc, char **argv, struct args_type *args, bool *put_post) 
{
  int choice;
  
  while ((choice = getopt(argc, argv, "prnbs:")) != EOF) {
    switch(choice) {
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
        return optarg;
	break;
      /* unknown opt, just default to display */
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

  /* from get opt */
  extern int optind;
  extern char *optarg;

  struct args_type cmd_args;
  bool req_type = false;

  /* opt returns the endpoint */
  char *opt = process_args(argc, argv, &cmd_args, &req_type);
  /* hold the json response */
  char *json_res;

  if(cmd_args.modify_change_song) {
    /* perform curl with endpoint @ search */
    json_res = get_json_from_server(opt); 
    parse_search_info(json_res);
    print_avaible_songs();
    clear_search_list ();
  } else if (cmd_args.modify_player) {
    /* perform curl with enndpoint to modify player */
    change_player_status(opt, req_type); 
  } else {
    /* perform curl with endpoint @ currently-playing */
    json_res = get_json_from_server(opt); 
    parse_currently_playing(json_res);
    printList();
    clear_linked_list();
  }

  return 0;
}
