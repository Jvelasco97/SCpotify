
/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 15, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

enum request_type{PUT, POST};

char* process_args(int argc, char **argv, int *flag) {
	int choice;

	while ((choice = getopt(argc, argv, "prnb")) != EOF) {
		switch(choice) {
    /* pause player */
		case 'p':
      printf("Now pausing\n");
      *flag = PUT;
      return "pause";
			break;
    /* resume player */
		case 'r':
      printf("Now resuming\n");
      *flag = PUT;
      return "play";
			break;
    /* next track */
		case 'n':
      printf("Playing next track\n");
      *flag = POST;
      return "next";
			break;
    /* prev track */
		case 'b':
      printf("Playing previous track\n");
      *flag = POST;
      return "previous";
			break;
    /* unknown opt */
		default:
			return NULL;
		}
	}

	return NULL;
}

int main (int argc, char **argv) {


  int request_flag;

  char *opt = process_args(argc, argv, &request_flag);

  if(opt)
    change_player_status(opt, &request_flag);

  char *current_song_info = display_currently_playing(); 
  parse_currently_playing(current_song_info);

  printList();

  return 0;
}
