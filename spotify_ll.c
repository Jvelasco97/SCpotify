#include "spotify_ll.h"
#include "spotify_structs.h"
#include "colors.h"

static struct 
song_info_node *head = NULL;

static struct 
available_song_node *available_node = NULL;

/**
 * insert the node, its a linked list, but if you dont know,
 * when we create a new node, we want the head pointer to point to it
 * so have the previous head point to head, then we have head point to the new one
 * @param song_info - the parsed string of the song info
 * @param size - the size of the string, used for malloc size
 * @note, we allocate in the heap because we want each parsed bit of info
 * into its own node instead of sharing the same memory address
 */
void 
insert_node(char* song_info, ssize_t size) 
{
  struct song_info_node *node = (struct song_info_node*) malloc(sizeof(struct song_info_node));

  char *dest = malloc(sizeof(char) * size);

  /* copy the info into a new address space */
  memcpy(dest, song_info, size);

  /* have the node point to it */
  node->data = dest;

  /* point to previous head */
  node->next = head;

  /* new head */
  head = node;
}

/**
 * insert into the linked list of available songs
 * @param album_info - the id of the album
 * @param artist_info - artist name for display purposes
 * @param track_position - the position of the song in the album
 * @param song_info - song title for display purposes
 */
void 
insert_search_node(char *album_info, char *artist_info, char *track_position, char *song_title) 
{
  struct available_song_node *node = 
  (struct available_song_node*) malloc(sizeof(struct available_song_node));

  node->album_api_info = album_info;
  node->artist_info = artist_info;
  node->album_position = track_position;
  node->song_title = song_title;

  /* point to previous head */
  node->next = available_node;

  /* new head */
  available_node = node;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void printList() {
  struct song_info_node *ptr = head;

  while(ptr != NULL) {
    printf("(%s) ",ptr->data);
    ptr = ptr->next;
  }
	
   printf("\n");
}


/**
 * prints all the available songs and then asks the user what
 * song to play based on index
 * around them
 */

struct search_song_request 
print_avaible_songs() 
{
  struct available_song_node *ptr = available_node;

  /* used to point to the node that we want album info from */
  u_int8_t counter = 0;

  /* params for play_song */
  /* char *album_id; */
  /* char *album_position; */

  struct search_song_request req_node;

  /* display available songs */
  while(ptr != NULL) {
    printf("%s[%d]%s %s%s%s by %s%s%s\n",
	   "\x1B[36m",counter++,"\x1B[0m",
	   "\x1B[31m",ptr->song_title,"\x1B[0m",
	   "\x1B[33m",ptr->artist_info,"\x1B[0m");
    ptr = ptr->next;
  }

  /* reset so we can fetch the index position */
  ptr = available_node;
  counter = 0;
  
  /* ask what song we want to play*/
  int choice;
  printf("\nPlay which song? ");

  do{
    scanf("%d", &choice);
    if(!(choice > 0 && choice < 5 )) {
      printf("\nplease enter the a valid range: ");
    }
  } while(choice < 0 || choice > 4);

  while(ptr != NULL) {
    if(counter == choice) {
      req_node.track_info = ptr->album_api_info;
      req_node.track_position = ptr->album_position;
      break;
    }

    counter++;
    ptr = ptr->next;
  }

  return req_node;
}

/**
 * frees all heap memory
 */

void 
clear_linked_list() 
{
  /* we dont want to mess with the actual head */
  struct song_info_node *ptr = head; 
  
  /* holds the node that will be deleted */
  struct song_info_node *temp; 

  /* as long as we are referencing something, do it */
  while(ptr) {
    /* free the data, or else it just sits there */
    free(ptr->data);

    /* poit to be deleted node, point to the next head, */
    /* then delete */
    temp = ptr;
    ptr = ptr->next;
    free(temp);
  }

  head = NULL;
}

/**
 * frees all heap memory
 */

void 
clear_search_list() 
{
  
  /* we dont want to mess with the actual head */
  struct available_song_node *ptr = available_node; 
  
  /* holds the node that will be deleted */
  struct available_song_node *temp; 

  /* as long as we are referencing something, do it */
  while(ptr) {
    /* free the data, or else it just sits there */
    free(ptr->album_api_info);
    free(ptr->album_position);
    free(ptr->artist_info);
    free(ptr->song_title);

    /* poit to be deleted node, point to the next head, */
    /* then delete */
    temp = ptr;
    ptr = ptr->next;
    free(temp);
  }

  available_node = NULL;
}
