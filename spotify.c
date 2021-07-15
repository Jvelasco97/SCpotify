
/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 15, 2021
 * @brief gets current playing song on spotify, more stuff soon, prob....
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

// holds the received data from the packets
struct json_data {
  char *data;
  size_t size;
  size_t realsize;
};

// put parsed song information because strings are scary in C, and it helps in the long run
struct song_info_node {
   char* data;
   struct song_info_node *next;
};

static struct song_info_node *head = NULL;

/**
 * insert the node, its a linked list, but if you dont know,
 * when we create a new node, we want the head pointer to point to it
 * so have the previous head point to head, then we have head point to the new one
 * @note, we allocate in the heap because we want each parsed bit of info
 * into its own node instead of sharing the same memory address
 */
void inser_node(char* song_info, ssize_t size) {
  struct song_info_node *node = (struct song_info_node*) malloc(sizeof(struct song_info_node));

  // allocate 40 charactes, more of less if u want
  char *dest = malloc(sizeof(char) * 40);

  // copy the info into a new address space
  memcpy(dest, song_info, size);

  // have the node point to it
  node->data = dest;

  // point to previous head
  node->next = head;

  // new head
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
void free_heap() {
  
  // we dont want to mess with the actual head
  struct song_info_node *ptr = head; 
  
  // holds the node that will be deleted
  struct song_info_node *temp; 

  // as long as we are referencing something, do it
  while(ptr) {
    // free the data, or else it just sits there
    free(ptr->data);

    // poit to be deleted node, point to the next head,
    // then delete
    temp = ptr;
    ptr = ptr->next;
    free(temp);
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
size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct)
{
  //cast as our custom object
  struct json_data *json_res = (struct json_data *)user_struct;

  // size of the packet
  size_t realsize = size * nmemb;

  // basically if the max size - current size = availabke space is less than than the 
  // new packet size, then reallocate
  if (json_res->realsize - json_res->size <= realsize) {
    char *new_mem_ptr = realloc(json_res->data, json_res->realsize += realsize * 2);

    if (new_mem_ptr == NULL) {
      fprintf(stderr, "not enough memory (realloc returned NULL)\n");
      return 0;
    }

    json_res->data = new_mem_ptr;
  }

  // slap the packet to the object
  memcpy(&(json_res->data[json_res->size]), contents, realsize);
  // update current size of the buffer for the object
  json_res->size += realsize;
  // terminate the string as defined functions dont do it for us
  json_res->data[json_res->size] = 0;

  return realsize;
}

/**
 * helper function, tells us when to cut garbage off
 * @param artist_info - the string i want cut
 * @return distance - how much to "cut off"
 */
ssize_t cut(char *artist_info) {

  ssize_t distance = 0;
  // TODO - FIX, maybe delimeter = /,"/?
  char delimeter = ',';

  while(*artist_info && *artist_info != delimeter) {
    distance++;
    artist_info++;
  }

  if(*artist_info == '\0')
    return 0;

  return distance;
}

// TODO - clean main
int main (int argc, char *argv[], char * envp[]) {

  CURL *curl;
  CURLcode res;
  struct curl_slist *headers=NULL; // init to NULL i

  struct json_data web_data;
  web_data.data = malloc(sizeof(char) * 16);
  web_data.size = 0;
  web_data.realsize = 16;

  // gets the header + token from your system enviroment variables, 
  // TODO - ADD, read from .env file instead of system
  char* auth_header = getenv("SPOTIFY_OAUTH");

  headers = curl_slist_append(headers, "Accept: application/json");  
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8"); 
  // gets refreshed like every 40 minutes lol
  headers = curl_slist_append(headers, auth_header); 
  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/currently-playing");
    curl_easy_setopt(curl, CURLOPT_HTTPGET,1); 

    // callback that handles the writing  of the data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StoreData);

    /* Set the data to pass when the function is called */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_data);
    res = curl_easy_perform(curl);

    if (CURLE_OK == res) 
    { 
      char *ct;         
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    }
  }

  // search pattern & ignore flag
  const char json_parse_string[] = "name";
  bool skipped_extra = 0;

  char *json_position = web_data.data;

  // searches the whole string and finds the sub string
  // returns pointer to the beggining of the found string
  while ((json_position = strstr(json_position, json_parse_string))) {

    // adjust new starting point
    json_position = &(json_position[sizeof(json_parse_string) - 1]);

    ssize_t distance = cut(json_position);

    // temporarely terminate the string so it can be copied in the node
    json_position[distance - 1] = 0;

    //skips over unwanted charactes, -- TODO -FIX
    char *ptr = json_position + 5;

    //strstr gets an unwanted field, cannot be ignored because the
    // json response
    if(skipped_extra)
      inser_node(ptr, distance);

    // reset the json string
    json_position[distance - 1] = ' ';

    // the flag is enabled for the remainder of the loop
    skipped_extra = 1;
  }

  curl_slist_free_all(headers);

  printList();

  free_heap();

  return 0;
}
