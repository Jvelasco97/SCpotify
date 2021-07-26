#include "spotify_http.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include "token.h"

/**
 * since i am going a GET req that requires me to 
 * parse the data at somepoint, i use this function for
 * the display functionality and search song.
 * @param search_parameter - the song we want, can be null.
 */
char *get_json_from_server(char *search_parameter) {
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  headers = curl_slist_append(headers, token);
  curl = curl_easy_init();

  struct json_data web_data;
  web_data.data = malloc(sizeof(char) * 16);
  web_data.current_size = 0;
  web_data.allocated_max_size = 16;

  char *SPOTIFY_SEARCH_URL = "https://api.spotify.com/v1/search?q=";
  char *SPOTIFY_CURRENTLY_PLAYING =
      "https://api.spotify.com/v1/me/player/currently-playing";
  char *endpoint;

  /* if we received a search query, then we prep the url */
  /* else we just specify currenly playing */
  if (search_parameter) {
    char *encoded_string = url_encoder(search_parameter);

    if ((endpoint = malloc(strlen(SPOTIFY_SEARCH_URL) + strlen(encoded_string) + 1)) != NULL) {
      endpoint[0] = '\0';

      strcat(endpoint, SPOTIFY_SEARCH_URL);

      strcat(endpoint, encoded_string);
    } else {
      printf("malloc failed!\n");
    }
  } else {
    endpoint = SPOTIFY_CURRENTLY_PLAYING;
  }

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, endpoint);
    /* curl_easy_setopt(curl, CURLOPT_HTTPGET,1);  */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

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

  if (search_parameter)
    free(endpoint);

  curl_slist_free_all(headers);

  return web_data.data;
}

/**
 * modifies the state of the player. 
 * @param opt - the endpoint that will be hit
 * @param req_type - wether we use PUT or POST
 */
void change_player_status(char *opt, bool req_type) {
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  char *SPOTIFY_PLAYER_URL = "https://api.spotify.com/v1/me/player/";

  char *endpoint;

  /* make sure we have enough to malloc the string */
  if ((endpoint = malloc(strlen(SPOTIFY_PLAYER_URL) + strlen(opt) + 1)) != NULL) {
    endpoint[0] = '\0';

    strcat(endpoint, SPOTIFY_PLAYER_URL);

    strcat(endpoint, opt);
  } else {
    printf("malloc failed!\n");
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  /* curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj); */
  headers = curl_slist_append(headers, token);
  curl = curl_easy_init();

  char *req = "PUT";

  if (!req_type) {
    req = "POST";
  }

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, endpoint);

    res = curl_easy_perform(curl);

    if (CURLE_OK == res) {
      char *ct;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    }
  }

  free(endpoint);
  curl_slist_free_all(headers);
}

/**
 * performs a PUT request to play the song
 * @param album_info - the album id for the data
 * @param album_position - offset in the album
 */
void 
play_song(char *album_info, char* album_position) {
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  char *SPOTIFY_PLAYER_URL = "https://api.spotify.com/v1/me/player/play";

  /* we the offset needs to be set to the number minus 1 */
  int pos = atoi(album_position);
  pos--;
 
  /* turn the int to a char again */
  char offset[2];
  sprintf(offset, "%d", pos);

  /* char *jsonObj = "{\"context_uri\":\"spotify:album:5YKqfiQdPYWJ0kZ5pttY5o\",
  * \"offset\":{\"position\":0},\"position_ms\":0}"; */

  char *jsonObj;

  char *json_obj_start = "{\"context_uri\":\"";
  char *json_obj_mid = "\",\"offset\":{\"position\":";
  char *json_obj_end = "},\"position_ms\":0}";

  /* malloc sufficient space for the data so we can append all the values */
  if ((jsonObj = malloc(strlen(json_obj_start) + strlen(album_info) + strlen(json_obj_mid) + 1 + strlen(json_obj_end) + 1)) != NULL) {
    jsonObj[0] = '\0';

    strcat(jsonObj, json_obj_start);
    strcat(jsonObj, album_info);
    strcat(jsonObj, json_obj_mid);
    strcat(jsonObj, offset);
    strcat(jsonObj, json_obj_end);
  } else {
    printf("malloc failed!\n");
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  /* token comes from toekn.h */
  headers = curl_slist_append(headers, token);
  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj);
    curl_easy_setopt(curl, CURLOPT_URL, SPOTIFY_PLAYER_URL);

    res = curl_easy_perform(curl);

    if (CURLE_OK == res) {
      char *ct;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    }
  }

  free(jsonObj);
  curl_slist_free_all(headers);
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
size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct) {
  /* cast as our custom object */
  struct json_data *json_res = (struct json_data *)user_struct;

  /* size of the packet */
  size_t realsize = size * nmemb;

  /* basically if the max size - current size = availabke space is less than
   * than the  */
  /* new packet size, then reallocate */
  if (json_res->allocated_max_size - json_res->current_size <= realsize) {
    char *new_mem_ptr =
        realloc(json_res->data, json_res->allocated_max_size += realsize * 2);

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
