#include "spotify_parsers.h"
#include "spotify_ll.h"
#include "includes.h"

void 
parse_currently_playing(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_parse_string = "name";
  bool skipped_extra = 0;

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, json_parse_string))) {

    /* adjust new starting point */
    json_position = &(json_position[strlen(json_parse_string) - 1]);

    ssize_t distance = cut(json_position);

    /* temporarely terminate the string so it can be copied in the node */
    json_position[distance - 1] = 0;

    /* add offset of 5 characters */
    char *ptr = json_position + 5;

    /* strstr gets an unwanted field, cannot be ignored because the */
    /* json response */
    if(skipped_extra)
      insert_node(ptr, distance);

    /* reset the json string */
    json_position[distance - 1] = ' ';

    /* the flag is enabled for the remainder of the loop */
    skipped_extra = 1;
  }

}

void 
parse_search_info(char *json_web_data) 
{
  /* search pattern & ignore flag */
  const char *json_album_info = "spotify:album:";
  const char *json_track_position = "\"track_number\"";
  const char *find_artist = "\"name\"";
  const char *find_song = "\"is_local\"";

  ssize_t distance = 0;

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_artist))) {
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct available_song_node *temp = 
    (struct available_song_node*) malloc(sizeof(struct available_song_node));

    /* strstr to first "name" */
    insert_parsed_data(&json_position, &find_artist, 5);

    /* printf("XXX%s\n", temp->album_name); */

    /* the reason we cut off at 36 is because that's how long the */
    /* strstr to where the  */
    /* album string is */
    /* json_position = strstr(json_position, json_album_info); */
    /* json_position = &(json_position[strlen(json_album_info) - 1]); */
    /* json_position[36] = '\0'; */
    /* temp->album_api_info = malloc(sizeof(char) * distance); */
    /* memcpy(temp->album_api_info, json_position + 5, distance); */
    /* json_position[36] = ' '; */
    /* temp->album_api_info = malloc(sizeof(char) * 36); */
    /* memcpy(temp->album_api_info, json_position, 36); */
    /* resume parsing the json response */
    /* json_position[36] = ' '; */
    /* adjust new starting point */
    /* json_position = &(json_position[strlen(json_album_info) - 1]); */

    /* strstr to the artist name */
    /* temp->artist_info = insert_parsed_data(json_position, find_artist, 5); */

    /* we have this to skip an unwanted object in the json response */
    /* json_position = strstr(json_position,find_song); */
    /* json_position = &(json_position[strlen(find_song) - 1]); */

    /* temp->song_title = insert_parsed_data(json_position, find_artist, 5); */

    /* strstr to the track position in the album */
    /* json_position = strstr(json_position, json_track_position); */
    /* json_position = &(json_position[strlen(json_track_position) - 1]); */
    /* distance = cut(json_position); */
    /* json_position[distance] = 0; */
    /* temp->album_position = malloc(sizeof(char) * distance); */
    /* memcpy(temp->album_position, json_position + 4, distance); */
    /* json_position[distance] = ' '; */
    /* temp->album_position = insert_parsed_data(json_position, json_track_position, 4); */
    /* temp->album_position = "2"; */

    /* insert_search_node(temp); */
  }
}

char *insert_parsed_data(char **json_position, const char **search, int displacement) {
  *json_position = strstr(*json_position, *search);
  /* *json_position = json_position[strlen(*search) - 1]; */
  /* *json_position = (json_position[strlen(*search)]); */
  char *ptr = &*json_position[strlen(*search)];
  /* *json_position = json_position[strlen(*search) - 1]; */


  /* ssize_t distance = cut(*json_position); */
  /* *json_position[distance] = 0; */
  printf("%s\n", ptr);

  /* char *ptr = malloc(sizeof(char) * distance); */
  /* memcpy(ptr, *&json_position + displacement, distance); */
  /* *&json_position[distance] = 0; */
  
  /* printf("%s\n", ptr); */

  return "hi";
}

/**
 * helper function, tells us when to cut garbage off
 * @param artist_info - the string i want cut
 * @return distance - how much to "cut off"
 */
ssize_t 
cut(char *artist_info) 
{
  ssize_t distance = 0;

  /* 100 because that's the biggest string i could think off, i would have to do some  */
  /* sort of recursion to get the dynamic size, which isnt worth it if you know */
  /* roughly how big its going to be */
  for(int i = 0; i < 100; i++) {
    if(artist_info[i] == '\"' && artist_info[i+1] == ',')
      break;

    distance++;
  }

  return distance;
}
