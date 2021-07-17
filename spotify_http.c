#include "spotify_structs.h"
#include "spotify_http.h"
#include "token.h"

char *GET_currently_playing() {
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers=NULL;

  struct json_data web_data;
  web_data.data = malloc(sizeof(char) * 16);
  web_data.current_size = 0;
  web_data.allocated_max_size = 16;

  /* gets the header + token from your system enviroment variables,  */
  /* TODO - ADD, read from .env file instead of system */
  /* char* auth_header = getenv("SPOTIFY_OAUTH"); */

  headers = curl_slist_append(headers, "Accept: application/json");  
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8"); 

  /* gets refreshed like every 40 minutes lol */
  headers = curl_slist_append(headers, token); 
  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.spotify.com/v1/me/player/currently-playing");
    curl_easy_setopt(curl, CURLOPT_HTTPGET,1); 

    /* callback that handles the writing  of the data */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StoreData);

    /* Set the data to pass when the function is called */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_data);
    res = curl_easy_perform(curl);

    if (CURLE_OK == res) { 
      char *ct;         
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    }
  }

  curl_slist_free_all(headers);

  return web_data.data;
}

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct) {
  /* cast as our custom object */
  struct json_data *json_res = (struct json_data *)user_struct;

  /* size of the packet */
  size_t realsize = size * nmemb;

  /* basically if the max size - current size = availabke space is less than than the  */
  /* new packet size, then reallocate */
  if (json_res->allocated_max_size - json_res->current_size <= realsize) {
    char *new_mem_ptr = realloc(json_res->data, json_res->allocated_max_size += realsize * 2);

    if (new_mem_ptr == NULL) {
      fprintf(stderr, "not enough memory (realloc returned NULL)\n");
      return 0;
    }

    json_res->data = new_mem_ptr;
  }

  /* slap the packet to the object */
  memcpy(&(json_res->data[json_res->current_size]), contents, realsize);
  /* update current size of the buffer for the object */
  json_res->current_size += realsize;
  /* terminate the string as defined functions dont do it for us */
  json_res->data[json_res->current_size] = 0;

  return realsize;
}
