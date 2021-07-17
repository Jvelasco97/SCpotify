
/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 15, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include "main_includes.h"

/* static struct song_info_node *head = NULL; */

int main (int argc, char *argv[], char * envp[]) {

  char *current_song_info = GET_currently_playing(); 

  parse_currently_playing(current_song_info);

  printList();

  return 0;
}
