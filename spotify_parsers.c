#include "spotify_parsers.h"
#include "spotify_ll.h"
#include "includes.h"

char *parse_selected_playlist_json(char *json_web_data)
{
  const char *json_anchor        = "tracks";
  const char *endpoint           = "https://api.spotify.com/v1/playlists/";
  const char *json_playlist_name = "name";
  const char *json_track_info 	 = "is_local";

  char *json_position = json_web_data;
  json_position = strstr(json_position, endpoint);
  json_position = &(json_position[strlen(endpoint) - 1]);
  u_int8_t distance = cut(json_position);
  
  json_position[distance] = 0;
  char *ptr = malloc(sizeof(char) * (distance) + 1);
  memcpy(ptr, json_position + 1, distance - 1);
  ptr[distance] = 0;
  json_position[distance] = ' ';

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  json_position = strstr(json_position, json_anchor);
  json_position = &(json_position[strlen(json_anchor) - 1]);


  while ((json_position = strstr(json_position, json_track_info))) {
    struct spotify_playlist_songs *temp = 
    (struct spotify_playlist_songs*) malloc(sizeof(struct spotify_playlist_songs));

    temp->playlist_artist_name = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6);
    temp->playlist_song_album  = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6);
    insert_parsed_playlist_info(&json_position, &json_track_info, 0);
    temp->playlist_song_name = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6);

    insert_playlist_song_node(temp);
  }

  return ptr;
}

/**
 * gets info about currently playing song
 * @param json_web_data - the json response from the 
 * /currently-playing endpoint
 * @return none
 * @note honestly i can get rid of the bool
 */
void 
parse_currently_playing(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_parse_string = "name";

  char *json_position = json_web_data;

  /* do a prime read to skip a name attribute that we dont want */
  json_position = strstr(json_position, json_parse_string);
  json_position = &(json_position[strlen(json_parse_string) - 1]);

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, json_parse_string))) {

    /* adjust new starting point */
    json_position = &(json_position[strlen(json_parse_string) - 1]);

    ssize_t distance = cut(json_position);

    /* temporarely terminate the string so it can be copied in the node */
    json_position[distance] = 0;

    /* add offset of 5 characters */
    char *ptr = json_position + 6;

    insert_node(ptr, distance);

    /* reset the json string */
    json_position[distance] = ' ';
  }

}

/**
 * parses all the info we want about the song that we want
 * to search and possibly play
 * @param json_web_data - the json response from the 
 * /search endpoint
 * @return none
 */
void 
parse_search_info(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_album_info = "spotify:album:";
  const char *json_track_position = "\"track_number\"";
  /* gets resued alot because alot of attributes have "name" in them */
  const char *find_artist = "\"name\"";
  const char *find_song = "\"is_local\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_artist))) {
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct available_song_node *temp = 
    (struct available_song_node*) malloc(sizeof(struct available_song_node));

    /* strstr to first "name" and get the album name*/
    temp->album_name = insert_parsed_data(&json_position, &find_artist, 5);

    /* strstr to album "name" and get the album api info*/
    temp->album_api_info = insert_parsed_data(&json_position, &json_album_info, 1);

    /* go to "name" and extract author name */
    temp->artist_info = insert_parsed_data(&json_position, &find_artist, 5);

    /* go to "song" and then call function again to get the "name" */
    insert_parsed_data(&json_position, &find_song, 5);

    /* go to "name" and get the song name */
    temp->song_title = insert_parsed_data(&json_position, &find_artist, 5);

    /* go to "track" and get the track position */
    temp->album_position = insert_parsed_data(&json_position, &json_track_position, 4); 

    insert_search_node(temp);
  }
}

/**
 * parses all the info we want about playlists available
 * to search and possibly play
 * @param json_web_data - the json response from the 
 * /search endpoint
 * @return none
 */
void 
parse_playlist_json(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_anchor        = "images";
  const char *json_playlist_name = "name";
  const char *json_display_name  = "display_name";
  const char *json_spotify_uri   = "spotify:playlist:";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, json_anchor))) {
    json_position = &(json_position[strlen(json_anchor) - 1]);

    struct spotify_playlist *temp = 
    (struct spotify_playlist*) malloc(sizeof(struct spotify_playlist));

    /* strstr to first "name" and get the album name*/
    temp->playlist_name = insert_parsed_data(&json_position, &json_playlist_name, 5);

    /* strstr to album "name" and get the album api info*/
    temp->playlist_owner = insert_parsed_data(&json_position, &json_display_name, 6);

    /* go to "name" and extract author name */
    temp->playlist_uri = insert_parsed_data(&json_position, &json_spotify_uri, 1);

    insert_playlist_node(temp);
  }
}

/**
 * parses all the info we want about the song that we want
 * to search and possibly play
 * @param json_web_data - the json response from the 
 * /search endpoint
 * @return none
 */
void 
parse_queue_search_info(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_track_info = "spotify:track:";
  /* gets resued alot because alot of attributes have "name" in them */
  const char *find_artist = "\"name\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_artist))) {
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct available_song_node *temp = 
    (struct available_song_node*) malloc(sizeof(struct available_song_node));

    temp->song_title = insert_parsed_data(&json_position, &find_artist, 5);
    temp->album_name = insert_parsed_data(&json_position, &find_artist, 5);
    temp->artist_info = insert_parsed_data(&json_position, &find_artist, 5);
    temp->album_api_info = insert_parsed_data(&json_position, &json_track_info, 1);

    insert_search_node(temp);
  }
}

char *insert_parsed_playlist_info(char **json_position, const char **search, int displacement)
{
    *json_position = strstr(*json_position, *search);

    /* if we dont return NULL then a garbage value will be */
    /* and cause a segfault */
    if(!(*json_position))
    {
      return NULL;
    }
    else
    {
      *json_position = *json_position + (strlen(*search) - 1) + displacement;
    }

    ssize_t distance = cut(*json_position);

    /* plus one for the null */
    char *ptr = malloc(sizeof(char) * (distance) + 1);
    memcpy(ptr, *json_position, distance);

    /* terminate the string as there is no previous terminator */
    ptr[distance] = 0;

    return ptr;
}
/**
 * calls the cut function so that we can extract the data
 * we want and allocate it for further use
 * @param json_position the json that we are traversing
 * @searh the search query we want for the json object
 * @displacement skip over unwanted characters
 * @return heap allocated char * of the data we wanted
 */
char *insert_parsed_data(char **json_position, const char **search, int displacement) {
  /* move the pointer to desired value in json obj */
  *json_position = strstr(*json_position, *search);
  *json_position = *json_position + (strlen(*search) - 1) + displacement;
  /* add displacement to skipp unwanted garbage */

  /* use this function to search for certain delimiters and 'cut' */
  /* the string later */
  ssize_t distance = cut(*json_position);

  /* plus one for the null */
  char *ptr = malloc(sizeof(char) * (distance) + 1);
  memcpy(ptr, *json_position, distance);

  /* terminate the string as there is no previous terminator */
  ptr[distance] = 0;

  return ptr;
}

/**
 * helper function, tells us when to cut garbage off
 * @param artist_info - the string i want cut
 * @return distance - how much to "cut off"
 */
ssize_t 
cut(char *artist_info) 
{
  const u_int8_t MAX_STRING = 100;
  ssize_t distance = 0;

  /* 100 because that's the biggest string i could think off, i would have to do some  */
  /* sort of recursion to get the dynamic size, which isnt worth it if you know */
  /* roughly how big its going to be */
  for(int i = 0; i < MAX_STRING; i++) {
    if(artist_info[i + 1] == '\n') {
      return distance;
    } else if(artist_info[i + 1] == ',' && artist_info[i + 2] == '\n') {
      /* pre increment as we want the return value incremented before hand */
      return ++distance;
    } else if(artist_info[i + 1] == '\"' && artist_info[i + 2] == ',') {
      /* pre increment as we want the return value incremented before hand */
      return ++distance;
    }
    distance++;
  }

  /* something went wrong */
  return 0;
}
