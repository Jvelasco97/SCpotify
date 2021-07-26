#include "spotify_ll.h"
#include "spotify_http.h"
#include "spotify_structs.h"

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

void 
insert_search_node(char *album_info, char *artist_info, char *track_position, char *song_title) 
{
  struct available_song_node *node = (struct available_song_node*) malloc(sizeof(struct available_song_node));

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
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */

char * 
print_avaible_songs() 
{
  struct available_song_node *ptr = available_node;

  int counter = 0;
  char *album_id;
  char *album_position;

  while(ptr != NULL) {
    printf("[%d] %s by %s\n", counter++,ptr->song_title, ptr->artist_info);
    ptr = ptr->next;
  }

  ptr = available_node;
  counter = 0;
  
  int choice;
  printf("Play which song? ");
  scanf("%d", &choice);

  while(ptr != NULL) {
    if(counter == choice) {
      album_id = ptr->album_api_info;
      album_position = ptr->album_position;
      break;
    }

    counter++;
    ptr = ptr->next;
  }

  play_song(album_id, album_position);

  return album_id;
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
