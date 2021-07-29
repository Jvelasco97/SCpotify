#include "spotify_parsers.h"
#include "spotify_structs.h"
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
  while ((json_position = strstr(json_position, json_album_info))) {

    /* the reason we cut off at 36 is because that's how long the */
    /* album string is */
    json_position[36] = '\0';
    char* node_album_info = malloc(sizeof(char) * 36);
    memcpy(node_album_info, json_position, 36);
    /* resume parsing the json response */
    json_position[36] = ' ';
    /* adjust new starting point */
    json_position = &(json_position[strlen(json_album_info) - 1]);

    char* node_artist_info;
    while((json_position = strstr(json_position,find_artist))) {
      json_position = &(json_position[strlen(find_artist) - 1]);
      distance = cut(json_position);
      node_artist_info = malloc(sizeof(char) * distance);
      json_position[distance] = 0;
      /* offset of 4 because of some garbage chars we don't want */
      memcpy(node_artist_info, json_position+4, distance);
      json_position[distance] = ' ';
      break;
    }

    /* we have this to skip an unwanted object in the json response */
    while((json_position = strstr(json_position,find_song))) {
      json_position = &(json_position[strlen(find_song) - 1]);
      break;
    }

    char* node_song_title;
    while((json_position = strstr(json_position,find_artist))) {
      json_position = &(json_position[strlen(find_artist) - 1]);
      distance = cut(json_position);
      node_song_title = malloc(sizeof(char) * distance);
      json_position[distance] = 0;
      /* offset of 4 because of some garbage chars we don't want */
      memcpy(node_song_title, json_position+4, distance);
      json_position[distance] = ' ';
      break;
    }

    char* node_track_position;
    while((json_position = strstr(json_position, json_track_position))) {
      json_position = &(json_position[strlen(json_track_position) - 1]);
      distance = cut(json_position);
      json_position[distance] = 0;
      node_track_position = malloc(sizeof(char) + distance);
      memcpy(node_track_position, json_position+4, distance);
      json_position[distance] = ' ';
      break;
    }

    insert_search_node(node_album_info, node_artist_info, node_track_position, node_song_title);
  }
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
