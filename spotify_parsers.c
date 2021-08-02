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

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_artist))) {
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct available_song_node *temp = 
    (struct available_song_node*) malloc(sizeof(struct available_song_node));

    /* strstr to first "name" */
    temp->album_name = insert_parsed_data(&json_position, &find_artist, 5);

    temp->album_api_info = insert_parsed_data(&json_position, &json_album_info, 1);

    temp->artist_info = insert_parsed_data(&json_position, &find_artist, 5);

    insert_parsed_data(&json_position, &find_song, 5);

    temp->song_title = insert_parsed_data(&json_position, &find_artist, 5);

    temp->album_position = insert_parsed_data(&json_position, &json_track_position, 4); 

    insert_search_node(temp);
  }
}

char *insert_parsed_data(char **json_position, const char **search, int displacement) {
  *json_position = strstr(*json_position, *search);
  *json_position = *json_position + (strlen(*search) - 1);

  ssize_t distance = cut(*json_position + 1);

  char *ptr = malloc(sizeof(char) * (distance));
  memcpy(ptr, *json_position + displacement, (distance));

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
  ssize_t distance = 0;

  /* 100 because that's the biggest string i could think off, i would have to do some  */
  /* sort of recursion to get the dynamic size, which isnt worth it if you know */
  /* roughly how big its going to be */
  for(int i = 0; i < 100; i++) {
    if(artist_info[i + 1] == '\n') {
      return distance;
    } else if(artist_info[i + 1] == ',' && artist_info[i + 2] == '\n') {
      return distance;
    } else if(artist_info[i + 1] == '\"' && artist_info[i + 2] == ',') {
      return distance - 3;
    }
    distance++;
  }

  /* something went wrong */
  return 0;
}
