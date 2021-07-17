#include "spotify_ll.h"
#include "spotify_structs.h"

static struct song_info_node *head = NULL;

/**
 * insert the node, its a linked list, but if you dont know,
 * when we create a new node, we want the head pointer to point to it
 * so have the previous head point to head, then we have head point to the new one
 * @param song_info - the parsed string of the song info
 * @param size - the size of the string, used for malloc size
 * @note, we allocate in the heap because we want each parsed bit of info
 * into its own node instead of sharing the same memory address
 */
void insert_node(char* song_info, ssize_t size) {
  struct song_info_node *node = (struct song_info_node*) malloc(sizeof(struct song_info_node));

  /* allocate size charactes*/
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
 * frees all heap memory
 */
void clear_linked_list() {
  
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
