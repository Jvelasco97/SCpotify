#include "spotify_ll.h"
#include "spotify_structs.h"

static struct 
song_info_node *song_info_head = NULL;

static struct 
spotify_song_search_node *available_song_search_head = NULL;

static struct 
spotify_playlist_context *available_playlist_head = NULL;

static struct 
spotify_playlist_songs *available_playlist_song_head = NULL;

static struct 
spotify_top_tracks *spotify_top_track_head = NULL;

static struct 
spotify_top_artists *spotify_top_artist_head = NULL;

static struct 
spotify_related_artists *spotify_related_artists_head = NULL;

int get_user_choice(int counter)
{
  int choice; 
  printf("\nPlay which song? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < counter )) {
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }

  return choice;
}

 /*
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
  node->next = song_info_head;

  /* new head */
  song_info_head = node;
}

/**
 * insert into the linked list of available songs
 * @param album_info - the id of the album
 * @param artist_info - artist name for display purposes
 * @param track_position - the position of the song in the album
 * @param song_info - song title for display purposes
 */
void 
insert_search_node(struct spotify_song_search_node *node) 
{
  /* point to previous head */
  node->next = available_song_search_head;

  /* new head */
  available_song_search_head = node;
}

/**
 * insert into the linked list of available songs
 * @param album_info - the id of the album
 * @param artist_info - artist name for display purposes
 * @param track_position - the position of the song in the album
 * @param song_info - song title for display purposes
 */
void 
insert_playlist_node(struct spotify_playlist_context *node) 
{
  /* point to previous head */
  node->next = available_playlist_head;

  /* new head */
  available_playlist_head = node;
}

/**
 * insert into the linked list of available songs
 * @param album_info - the id of the album
 * @param artist_info - artist name for display purposes
 * @param track_position - the position of the song in the album
 * @param song_info - song title for display purposes
 */
void 
insert_playlist_song_node(struct spotify_playlist_songs *node) 
{
  /* point to previous head */
  node->next = available_playlist_song_head;

  /* new head */
  available_playlist_song_head = node;
}

void 
insert_related_artist_node(struct spotify_related_artists *node) 
{
  /* point to previous head */
  node->spotify_related_artists_node = spotify_related_artists_head;

  /* new head */
  spotify_related_artists_head = node;
}

/**
 * insert into the linked list of available songs
 * @param album_info - the id of the album
 * @param artist_info - artist name for display purposes
 * @param track_position - the position of the song in the album
 * @param song_info - song title for display purposes
 */
void 
insert_top_track_node(struct spotify_top_tracks *node) 
{
  /* point to previous head */
  node->next_spotify_top_tracks = spotify_top_track_head;

  /* new head */
  spotify_top_track_head = node;
}

void 
insert_top_artist_node(struct spotify_top_artists *node) 
{
  /* point to previous head */
  node->next_spotify_top_artists = spotify_top_artist_head;

  /* new head */
  spotify_top_artist_head = node;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void printList() {
  struct song_info_node *ptr = song_info_head;

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
void print_artists_search() {
  struct spotify_related_artists *ptr = spotify_related_artists_head;

  u_int8_t counter = 0;

  while(ptr != NULL) {
    printf("%s[%d]%s","\x1B[36m",counter++,"\x1B[0m");
    printf("%s\n",ptr->artist_name);
    printf("[%s]\n\n",ptr->genre_array);
    ptr = ptr->spotify_related_artists_node;
  }

  /* ask what song we want to play*/
  int choice;

  printf("\nPlay which song? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < counter )) {
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void print_related_artists() {
  struct spotify_related_artists *ptr = spotify_related_artists_head;

  printf("Here some artists that you may like based of this song.\n\n");

  while(ptr != NULL) {
    printf("%s\n",ptr->artist_name);
    printf("[%s]\n\n",ptr->genre_array);
    ptr = ptr->spotify_related_artists_node;
  }
}

/**
* prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void print_top_tracks() {
  struct spotify_top_tracks *track_ptr = spotify_top_track_head;
  struct spotify_top_artists *artist_ptr = spotify_top_artist_head;

  struct spotify_top_tracks *track_tmp = spotify_top_track_head;
  struct spotify_top_artists *artist_tmp = spotify_top_artist_head;

  reverse_top_tracks(&track_ptr);
  reverse_top_artists(&artist_ptr);

  printf("\t\tYour most played songs of 2021\t\t\t  Your most played artists of 2021\n");
  printf("--------------------------------------------------------------------------------------------------\n");
  while(track_ptr && artist_ptr)  {
    printf("%-35s",track_ptr->song_name);
    printf("%-30s",track_ptr->artist_name);
    printf("| %-30s|\n",artist_ptr->artist_name);

    track_tmp = track_ptr;
    artist_tmp = artist_ptr;

    track_ptr = track_ptr->next_spotify_top_tracks;
    artist_ptr = artist_ptr->next_spotify_top_artists;


    free(track_tmp->song_name);
    free(track_tmp->artist_name);
    free(artist_tmp->artist_name);
    free(track_tmp);
    free(artist_tmp);
  }
  printf("--------------------------------------------------------------------------------------------------\n");
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
static void reverse(struct spotify_playlist_songs** head_ref)
{
  struct spotify_playlist_songs* prev = NULL;
  struct spotify_playlist_songs* current = *head_ref;
  struct spotify_playlist_songs* next = NULL;

  while (current != NULL) 
  {
    /* Store next */
    next = current->next;

    /* Reverse current node's pointer */
    current->next = prev;

    /* Move pointers one position ahead. */
    prev = current;
    current = next;
  }

  *head_ref = prev;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void reverse_top_tracks(struct spotify_top_tracks** head_ref)
{
  struct spotify_top_tracks* prev = NULL;
  struct spotify_top_tracks* current = *head_ref;
  struct spotify_top_tracks* next = NULL;

  while (current != NULL) 
  {
    /* Store next */
    next = current->next_spotify_top_tracks;

    /* Reverse current node's pointer */
    current->next_spotify_top_tracks = prev;

    /* Move pointers one position ahead. */
    prev = current;
    current = next;
  }

  *head_ref = prev;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void 
reverse_top_artists(struct spotify_top_artists** spotify_top_artist_head)
{
  struct spotify_top_artists* prev = NULL;
  struct spotify_top_artists* current = *spotify_top_artist_head;
  struct spotify_top_artists* next = NULL;

  while (current != NULL) 
  {
    /* Store next */
    next = current->next_spotify_top_artists;

    /* Reverse current node's pointer */
    current->next_spotify_top_artists = prev;

    /* Move pointers one position ahead. */
    prev = current;
    current = next;
  }

  *spotify_top_artist_head = prev;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
u_int8_t print_playlist_songs() {
  struct spotify_playlist_songs *ptr = available_playlist_song_head;
  struct spotify_playlist_songs *temp = NULL;
  reverse(&ptr);

  u_int8_t counter = 0;

  while(ptr != NULL) {
    printf("%s[%d]%s %s%s%s (%s%s%s) by %s%s%s\n",
	   "\x1B[36m",counter++,"\x1B[0m",
	   "\x1B[31m",ptr->playlist_song_name,"\x1B[0m",
	   "\x1B[34m",ptr->playlist_album_name,"\x1B[0m",
	   "\x1B[33m",ptr->playlist_artist_name,"\x1B[0m");
    temp = ptr;
    ptr = ptr->next;
    free(temp->playlist_artist_name);
    free(temp->playlist_album_name);
    free(temp->playlist_song_name);
    free(temp);
  }

  /* ask what song we want to play*/
  int choice = get_user_choice(counter);

  return choice;
}
/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
char *print_playlist() {
  struct spotify_playlist_context *ptr = available_playlist_head; 

  u_int8_t counter = 0;

  while(ptr != NULL) {
    printf("%s[%d]%s (%s) by %s\n",
	   "\x1B[36m",counter++,"\x1B[0m",
	   ptr->playlist_name,
	   ptr->playlist_owner);
    ptr = ptr->next;
  }

  /* ask what song we want to play*/
  int choice;

  printf("\nwhich playlist? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < counter )) {
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }

  counter = 0;
  ptr = available_playlist_head;
  while(ptr != NULL) {
    if(counter == choice) {
      return ptr->playlist_uri;
      break;
    }

    counter++;
    ptr = ptr->next;
  }
	
   printf("\n");
   return NULL;
}

/**
 * prints all the available songs and then asks the user what
 * song to play based on index
 * around them
 */

struct spotify_song_query_info 
print_avaible_songs(u_int8_t MAX_SEARCH) 
{
  struct spotify_song_search_node *ptr = available_song_search_head;

  /* used to point to the node that we want album info from */
  u_int8_t counter = 0;

  /* params for play_song */
  /* char *album_id; */
  /* char *album_position; */

  struct spotify_song_query_info req_node = {
    .track_info = NULL,
    .track_position = NULL
  };

  /* display available songs */
  while(ptr != NULL) {
    printf("%s[%d]%s %s%s%s (%s%s%s) by %s%s%s\n",
	   "\x1B[36m",counter++,"\x1B[0m",
	   "\x1B[31m",ptr->song_title,"\x1B[0m",
	   "\x1B[34m",ptr->album_name,"\x1B[0m",
	   "\x1B[33m",ptr->artist_info,"\x1B[0m");
    ptr = ptr->next;
  }

  /* reset so we can fetch the index position */
  ptr = available_song_search_head;
  counter = 0;
  
  /* ask what song we want to play*/
  int choice;
  printf("\nPlay which song? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < MAX_SEARCH )) {
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }

  while(ptr) {
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

void
clear_playlist()
{
  struct spotify_playlist_context *ptr = available_playlist_head; 
  struct spotify_playlist_context *temp = NULL; 

  while(ptr != NULL) {
    temp = ptr;
    ptr = ptr->next;
    free(temp->playlist_name);
    free(temp->playlist_owner);
    free(temp->playlist_uri);
    free(temp);
  }
}

/**
 * frees all heap memory
 */

void 
clear_linked_list() 
{
  /* we dont want to mess with the actual head */
  struct song_info_node *ptr = song_info_head; 
  
  /* holds the node that will be deleted */
  struct song_info_node *temp = NULL; 

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

  song_info_head = NULL;
}

/**
 * frees all heap memory
 */
void 
clear_search_list() 
{
  
  /* we dont want to mess with the actual head */
  struct spotify_song_search_node *ptr = available_song_search_head; 
  
  /* holds the node that will be deleted */
  struct spotify_song_search_node *temp; 

  /* as long as we are referencing something, do it */
  while(ptr) {
    /* free the data, or else it just sits there */
    free(ptr->album_api_info);
    free(ptr->album_name);
    free(ptr->album_position);
    free(ptr->artist_info);
    free(ptr->song_title);

    /* poit to be deleted node, point to the next head, */
    /* then delete */
    temp = ptr;
    ptr = ptr->next;
    free(temp);
  }

  available_song_search_head = NULL;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void clear_related_artists() {
  struct spotify_related_artists *ptr = spotify_related_artists_head;
  struct spotify_related_artists *temp = NULL;

  while(ptr != NULL) {
    temp = ptr;
    ptr = ptr->spotify_related_artists_node;
    free(temp->artist_name);
    free(temp->genre_array);
    free(temp);
  }
}
