#include "spotify_http.h"
#include "spotify_structs.h"
#include "spotify_utils.h"
#include "spotify_command_defines.h"
#include <curl/curl.h>
#include <stdio.h>
#include <sys/types.h>

#define AUTH_TOKEN_LENGHT 307

/**
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
size_t 
StoreData(char *contents, size_t size, size_t nmemb, void *user_struct) 
{
  /* cast as our custom object */
  struct spotify_json_data *json_res = (struct spotify_json_data *)user_struct;

  /* size of the packet */
  size_t realsize = size * nmemb;

  /* basically if the max size - current size = availabke space is less than
   * than the  */
  /* new packet size, then reallocate */
  if (json_res->allocated_max_size - json_res->current_size <= realsize)
	{
    char *new_mem_ptr =
        realloc(json_res->data, json_res->allocated_max_size += realsize * 2);

    if (new_mem_ptr == NULL)
		{
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
curl_setopt(CURL *curl, struct scpotify_context *args, struct curl_slist *headers, char *req_type)
{
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req_type);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, args->endpoint);
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
spotify_http_perform(CURL *curl, struct scpotify_context *args, CURLcode res, struct curl_slist *headers)
{
  struct spotify_json_data web_data = {
    .data               = NULL,
    .current_size       = 0,
    .allocated_max_size = 0
  };

  if(args->enable_write)
  {
    web_data.data = malloc(sizeof(char) * 16);
    web_data.current_size = 0;
    web_data.allocated_max_size = 16;
  }

  switch(args->http_type)
  {
    case GET:
      curl_setopt(curl, args, headers, "GET");
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StoreData);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_data);

      res = curl_easy_perform(curl);

      args->search_struct->spotify_json_response = web_data.data;
      break;
    case POST:
      curl_setopt(curl, args, headers, "POST");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->search_struct->spotify_json_data);

      res = curl_easy_perform(curl);
      break;
    case PUT:
      curl_setopt(curl, args, headers, "PUT");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, args->search_struct->spotify_json_data);
      res = curl_easy_perform(curl);
      break;
    case DELETE:
      break;

    default:
      break;
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
long 
spotify_http(struct scpotify_context *args) {
  CURL *curl;
  CURLcode res = 0;
  struct curl_slist *headers = NULL;
  long http_code = 0;

  char *append_header = "Authorization: Bearer ";

  args->auth_struct->auth_token[AUTH_TOKEN_LENGHT] = 0;
  char *header = concat_strings(append_header, args->auth_struct->auth_token);

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");
  headers = curl_slist_append(headers, header);

  curl = curl_easy_init();

  if (curl)
	{
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    spotify_http_perform(curl, args, res, headers);

    if (CURLE_OK == res)
		{
      char *ct;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    } 
  }

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  free(header);

  return http_code;
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
spotify_auth_http(struct scpotify_context *args)
{
  CURL *curl;
  CURLcode res = 0;
  struct curl_slist *headers = NULL;
  long http_code = 0;

  headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
  char *header = concat_strings("Authorization: Basic ", args->auth_struct->base64_client_secret + 14);
  header[109] = 0;
  headers = curl_slist_append(headers, header);

  curl = curl_easy_init();

  struct spotify_json_data web_data = {
    .data               = NULL,
    .current_size       = 0,
    .allocated_max_size = 0
  };

  web_data.data = malloc(sizeof(char) * 16);
  web_data.current_size = 0;
  web_data.allocated_max_size = 16;


  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, args->endpoint);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StoreData);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_data);


  res = curl_easy_perform(curl);
  args->search_struct->spotify_json_response = web_data.data;


  if (curl)
	{
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    

    if (CURLE_OK == res)
		{
      char *ct;
      res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    } 
  }

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(header);
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
char *
build_search_query(struct scpotify_context *args)
{
  char *endpoint = NULL;

  /* if we received a search query, then we prep the url */
  /* else we just specify currenly playing */
  char *encoded_string = NULL;
  encoded_string = url_encoder(args->search_struct->search_query, args->search_struct->query_type);

  if ((endpoint = malloc(strlen(args->endpoint) + strlen(encoded_string) + 1)) != NULL)
	{
    endpoint[0] = '\0';

    strcat(endpoint, args->endpoint);
    strcat(endpoint, encoded_string);
  } 
  else 
  {
    printf("malloc failed!\n");
  }

  free(encoded_string);
  return endpoint;
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
char * 
build_put_request(char *album_info, char* album_position) 
{
  /* we the offset needs to be set to the number minus 1 */
  int8_t pos = atoi(album_position);
  pos--;
 
  /* turn the int to a char again */
  char offset[5];
  sprintf(offset, "%d", pos);

  char *jsonObj;

  char *json_obj_start = "{\"context_uri\":\"spotify:album:";
  char *json_obj_mid = "\",\"offset\":{\"position\":";
  char *json_obj_end = "},\"position_ms\":0}";

  /* malloc sufficient space for the data so we can append all the values */
  if ((jsonObj = malloc(strlen(json_obj_start) + strlen(album_info) + 
          strlen(json_obj_mid) + 1 + strlen(json_obj_end) + 1)) != NULL) 
  {
    jsonObj[0] = '\0';

    strcat(jsonObj, json_obj_start);
    strcat(jsonObj, album_info);
    strcat(jsonObj, json_obj_mid);
    strcat(jsonObj, offset);
    strcat(jsonObj, json_obj_end);
  }
	else
	{
    printf("malloc failed!\n");
  }

  return jsonObj;
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
char * 
build_put_request_playlist(char *playlist_uri, int album_position) 
{
  /* we the offset needs to be set to the number minus 1 */
 
  char *jsonObj;

  char offset[2];
  sprintf(offset, "%d", album_position);

  char *json_obj_start = "{\"context_uri\":\"spotify:playlist:";
  char *json_obj_mid = "\",\"offset\":{\"position\":";
  char *json_obj_end = "},\"position_ms\":0}";

  /* malloc sufficient space for the data so we can append all the values */
  if ((jsonObj = malloc(strlen(json_obj_start) + strlen(playlist_uri) + 
          strlen(json_obj_mid) + strlen(offset) + strlen(json_obj_end) + 1)) != NULL) 
  {
    jsonObj[0] = '\0';

    strcat(jsonObj, json_obj_start);
    strcat(jsonObj, playlist_uri);
    strcat(jsonObj, json_obj_mid);
    strcat(jsonObj, offset);
    strcat(jsonObj, json_obj_end);
  }
	else
	{
    printf("malloc failed!\n");
  }

  return jsonObj;
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
char * 
build_put_request_episode(char *show_uri, char *number_of_episodes,int show_position) 
{
  /* we the offset needs to be set to the number minus 1 */
 
  char *jsonObj;
  int num = atoi(number_of_episodes);
  num--;

  show_position = num - show_position;

  char offset[3];
  sprintf(offset, "%d", show_position);

  char *json_obj_start = "{\"context_uri\":\"spotify:show:";
  char *json_obj_mid = "\",\"offset\":{\"position\":";
  char *json_obj_end = "},\"position_ms\":0}";

  /* malloc sufficient space for the data so we can append all the values */
  if ((jsonObj = malloc(strlen(json_obj_start) + strlen(show_uri) + 
          strlen(json_obj_mid) + strlen(offset) + strlen(json_obj_end) + 1)) != NULL) 
  {
    jsonObj[0] = '\0';

    strcat(jsonObj, json_obj_start);
    strcat(jsonObj, show_uri);
    strcat(jsonObj, json_obj_mid);
    strcat(jsonObj, offset);
    strcat(jsonObj, json_obj_end);
  }
	else
	{
    printf("malloc failed!\n");
  }

  return jsonObj;
}
