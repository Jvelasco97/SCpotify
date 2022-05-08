#include "spotify_linked_lists.h"
#include "spotify_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static struct 
song_info_node *song_info_head = NULL;

static struct 
spotify_song_search_node *available_song_search_head = NULL;

static struct 
spotify_album_search_node *available_head_album_search_head = NULL;

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

static struct
spotify_artists_search_node *spotify_artists_search_node_head = NULL;

static struct
spotify_podcast_search_node *spotify_podcast_search_node_head = NULL;

static struct
spotify_podcast_episode_node *spotify_podcast_episode_search_node_head = NULL;

static struct
spotify_device_list_node *spotify_device_list_node_head = NULL;

int
get_user_choice(int counter)
{
	unsigned long choice = -1;
	
  char buffer[4];
	
  printf("\nPlay which song? ");

  while(true)
  {
    scanf("%4s", buffer);
		choice = strtoul(buffer, NULL, 0);
    if((choice > -1 && choice < counter )) {
      printf("\nplease enter the a valid range: ");
			printf("%d\n", counter);
			printf("%lu\n", choice);
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
insert_song_info_node(char* song_info, ssize_t size) 
{
  song_info_node *node = (struct song_info_node*) malloc(sizeof(struct song_info_node));

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
insert_search_node(spotify_song_search_node *node) 
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
insert_playlist_node(spotify_playlist_context *node) 
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
insert_playlist_song_node(spotify_playlist_songs *node) 
{
  /* point to previous head */
  node->next = available_playlist_song_head;

  /* new head */
  available_playlist_song_head = node;
}

void 
insert_related_artist_node(spotify_related_artists *node) 
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
insert_top_track_node(spotify_top_tracks *node) 
{
  /* point to previous head */
  node->next_spotify_top_tracks = spotify_top_track_head;

  /* new head */
  spotify_top_track_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
insert_top_artist_node(spotify_top_artists *node) 
{
  /* point to previous head */
  node->next_spotify_top_artists = spotify_top_artist_head;

  /* new head */
  spotify_top_artist_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
insert_artist_search_node(spotify_artists_search_node *node)
{
  /* point to previous head */
  node->spotify_artists_search_node = spotify_artists_search_node_head;

  /* new head */
  spotify_artists_search_node_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
insert_album_search_node(spotify_album_search_node *node)
{
  /* point to previous head */
  node->next = available_head_album_search_head;

  /* new head */
  available_head_album_search_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
insert_show_search_node(struct spotify_podcast_search_node *node)
{
  /* point to previous head */
  node->next = spotify_podcast_search_node_head;

  /* new head */
  spotify_podcast_search_node_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
insert_episode_search_node(spotify_podcast_episode_node *node)
{
  /* point to previous head */
  node->next = spotify_podcast_episode_search_node_head;

  /* new head */
  spotify_podcast_episode_search_node_head = node;
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
insert_device_list_node(spotify_device_list_node *node)
{
  /* point to previous head */
  node->spotify_device_list_node = spotify_device_list_node_head;

  /* new head */
  spotify_device_list_node_head = node;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void
spotify_now_playing_list()
{
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
void
print_artists_search_list()
{
  spotify_related_artists *ptr = spotify_related_artists_head;

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
void
print_related_artists_list()
{
  spotify_related_artists *ptr = spotify_related_artists_head;

  printf("Here some artists that you may like based of this song.\n\n");

  while(ptr != NULL)
	{
    printf("%s\n",ptr->artist_name);
    printf("[%s]\n\n",ptr->genre_array);
    ptr = ptr->spotify_related_artists_node;
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
spotify_episode_query_info
print_show_search_results_list(spotify_episode_query_info episode_info)
{
  spotify_podcast_search_node *ptr = spotify_podcast_search_node_head;

  u_int8_t counter = 0;

  while(ptr != NULL) 
  {
    printf("%s[%d]%s Podcast name: %s\n","\x1B[36m",counter++,"\x1B[0m",ptr->name);
    printf("\nNumber of episodes: %s\n",ptr->number_of_episodes);
    printf("\nPublisher: %s\n",ptr->publisher);
    printf("\nDescription: %s\n\n",ptr->description);
    ptr = ptr->next;
  }

  ptr = spotify_podcast_search_node_head;

  u_int8_t choice = get_user_choice(counter);
  counter = 0;

  while(ptr != NULL) 
  {

    if(counter == choice)
    {
      episode_info.number_of_episodes = ptr->number_of_episodes;
      episode_info.podcast_id = ptr->podcast_id;
      break;
    }
    ptr = ptr->next;
    counter++;
  }

  return episode_info;
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
char *
print_searched_artists_list() {
  spotify_artists_search_node *ptr = spotify_artists_search_node_head;

  u_int8_t counter = 0;

  while(ptr != NULL) 
  {
    printf("[%d] %s\n",counter++,ptr->artist_name);
    printf("[%s]\n\n",ptr->genre_array);
    ptr = ptr->spotify_artists_search_node;
  }

  ptr = spotify_artists_search_node_head;
  u_int8_t choice = get_user_choice(counter);
  counter = 0;

  while(ptr != NULL) 
  {

    if(counter == choice)
    {
      return ptr->artists_id;
    }
    ptr = ptr->spotify_artists_search_node;
    counter++;
  }

  return NULL;
}

/**
* prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void
print_top_tracks_list()
{
  spotify_top_tracks *track_ptr = spotify_top_track_head;
  spotify_top_artists *artist_ptr = spotify_top_artist_head;

  spotify_top_tracks *track_tmp = spotify_top_track_head;
  spotify_top_artists *artist_tmp = spotify_top_artist_head;

  reverse_top_tracks_list(&track_ptr);
  reverse_top_artists_list(&artist_ptr);

  printf("\t\tYour most played songs of 2022\t\t\t\t\t  Your most played artists of 2022\n");
  printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
  while(track_ptr && artist_ptr)  {
    printf("%-100s",track_ptr->song_name);
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

  printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
void
reverse_playlist_songs_list(spotify_playlist_songs** head_ref)
{
  spotify_playlist_songs* prev = NULL;
  spotify_playlist_songs* current = *head_ref;
  spotify_playlist_songs* next = NULL;

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
void
reverse_top_tracks_list(spotify_top_tracks** head_ref)
{
  spotify_top_tracks* prev = NULL;
  spotify_top_tracks* current = *head_ref;
  spotify_top_tracks* next = NULL;

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
reverse_top_artists_list(spotify_top_artists** spotify_top_artist_head)
{
  spotify_top_artists* prev = NULL;
  spotify_top_artists* current = *spotify_top_artist_head;
  spotify_top_artists* next = NULL;

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
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void 
reverse_podcast_episodes_list(spotify_podcast_episode_node** spotify_podcast_episode_node_head)
{
  spotify_podcast_episode_node *prev = NULL;
  spotify_podcast_episode_node *current = *spotify_podcast_episode_node_head;
  spotify_podcast_episode_node* next = NULL;

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

  *spotify_podcast_episode_node_head = prev;
}
/**
 * prints all the nodes strings, has the parantheses wrapping
 * around them
 */
u_int8_t
print_playlist_songs_list()
{
  spotify_playlist_songs *ptr = available_playlist_song_head;
  spotify_playlist_songs *temp = NULL;

  reverse_playlist_songs_list(&ptr);

  u_int16_t counter = 0;

  while(ptr != NULL)
	{
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
char *
print_playlist_list()
{
  spotify_playlist_context *ptr = available_playlist_head; 

  u_int8_t counter = 0;

  while(ptr != NULL)
	{
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
  while(ptr != NULL)
	{
    if(counter == choice)
		{
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
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
u_int16_t
print_podcast_episodes_list()
{
  struct spotify_podcast_episode_node *ptr = spotify_podcast_episode_search_node_head; 

  reverse_podcast_episodes_list(&ptr);

  u_int8_t counter = 0;

  while(ptr != NULL)
	{
    printf("%s[%d]%s Episode title: %s\n\n Release date: %s\n\n Description: %s\n\n",
	  "\x1B[36m",counter++,"\x1B[0m",
	  ptr->name,
	  ptr->release_date,
	  ptr->description);
    ptr = ptr->next;
  }

  /* ask what song we want to play*/
  int choice;

  printf("\nWhich episode would you like to listen to? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < counter ))
		{
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }

  return choice;
}

/**
 * prints all the available songs and then asks the user what
 * song to play based on index
 * around them
 */

void
print_available_songs_list(u_int8_t MAX_SEARCH, spotify_song_query_info* req_node) 
{
  spotify_song_search_node *ptr = available_song_search_head;

  /* used to point to the node that we want album info from */
  u_int8_t counter = 0;

  /* params for play_song */
  /* char *album_id; */
  /* char *album_position; */

  /* display available songs */
  while(ptr != NULL)
	{
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

  while(ptr)
	{
    if(counter == choice)
		{
      req_node->track_info = ptr->album_api_info;
      req_node->track_position = ptr->album_position;
      break;
    }

    counter++;
    ptr = ptr->next;
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
print_searched_artist_results_list(u_int8_t MAX_SEARCH, spotify_song_query_info *req_node) 
{
  spotify_song_search_node *song_ptr = available_song_search_head;
  spotify_album_search_node *album_ptr = available_head_album_search_head;

  /* used to point to the node that we want album info from */
  u_int8_t counter = 0;

  /* params for play_song */
  /* char *album_id; */
  /* char *album_position; */

  printf("\nHere are the top tracks for this artist!\n");
  /* display available songs */
  while(song_ptr != NULL)
	{
    printf("%s[%d]%s %s%s%s (%s%s%s) by %s%s%s\n",
	   "\x1B[36m",counter++,"\x1B[0m",
	   "\x1B[31m",song_ptr->song_title,"\x1B[0m",
	   "\x1B[34m",song_ptr->album_name,"\x1B[0m",
	   "\x1B[33m",song_ptr->artist_info,"\x1B[0m");
    song_ptr = song_ptr->next;
  }

  printf("\nHere are albums associated with this artist!\n");

  while(album_ptr != NULL)
	{
    printf("%s%s%s\n",
	   "\x1B[31m",album_ptr->album_name,"\x1B[0m");
    album_ptr = album_ptr->next;
  }

  /* reset so we can fetch the index position */
  song_ptr = available_song_search_head;
  counter = 0;
  
  /* ask what song we want to play*/
  int choice;
  printf("\nPlay which song? ");

  while(true)
  {
    scanf("%d", &choice);
    if(!(choice > -1 && choice < MAX_SEARCH ))
		{
      printf("\nplease enter the a valid range: ");
    } else
      break;
  }

  while(song_ptr)
	{
    if(counter == choice)
		{
      req_node->track_info = song_ptr->album_api_info;
      req_node->track_position = song_ptr->album_position;
      break;
    }

    counter++;
    song_ptr = song_ptr->next;
  }
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
print_available_devices()
{
  spotify_device_list_node *ptr = spotify_device_list_node_head;

  while(ptr != NULL)
	{
    printf("(%s) \n",ptr->device_id);
    printf("(%s) \n",ptr->device_name);
    ptr = ptr->spotify_device_list_node;
  }
	
   printf("\n");
}

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
void
clear_playlist_list()
{
  spotify_playlist_context *ptr = available_playlist_head; 
  spotify_playlist_context *temp = NULL; 

  while(ptr != NULL)
	{
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
clear_song_info_list() 
{
  /* we dont want to mess with the actual head */
  song_info_node *ptr = song_info_head; 
  
  /* holds the node that will be deleted */
  song_info_node *temp = NULL; 

  /* as long as we are referencing something, do it */
  while(ptr)
	{
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
clear_song_search_list() 
{
  
  /* we dont want to mess with the actual head */
  spotify_song_search_node *ptr = available_song_search_head; 
  
  /* holds the node that will be deleted */
  spotify_song_search_node *temp; 

  /* as long as we are referencing something, do it */
  while(ptr)
	{
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
void
clear_related_artists_list() {
  spotify_related_artists *ptr = spotify_related_artists_head;
  spotify_related_artists *temp = NULL;

  while(ptr != NULL)
	{
    temp = ptr;
    ptr = ptr->spotify_related_artists_node;
    free(temp->artist_name);
    free(temp->genre_array);
    free(temp);
  }
}
