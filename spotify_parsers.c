#include "spotify_parsers.h"
#include "spotify_linked_lists.h"
#include "includes.h"
#include <stdio.h>
#include <string.h>


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
parse_selected_playlist_json(char *json_web_data)
{
  const char *json_anchor        = "tracks";
  const char *endpoint           = "https://api.spotify.com/v1/playlists/";
  const char *json_playlist_name = "name";
  const char *json_track_info    = "is_local";

  char *json_position = json_web_data;

  json_position = strstr(json_position, endpoint);
  json_position = &(json_position[strlen(endpoint) - 1]);

  u_int8_t distance = cut(json_position);
  json_position[distance] = 0;

  char *ptr = calloc(distance, sizeof(char));

  memcpy(ptr, json_position + 1, distance - 1);

  json_position[distance] = ' ';

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  json_position = strstr(json_position, json_anchor);
  json_position = &(json_position[strlen(json_anchor) - 1]);


  while ((json_position = strstr(json_position, json_track_info)))
	{
    struct spotify_playlist_songs *temp = 
    (struct spotify_playlist_songs*) malloc(sizeof(struct spotify_playlist_songs));

    temp->playlist_artist_name = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6, true);
    temp->playlist_album_name  = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6, true);
    insert_parsed_playlist_info(&json_position, &json_track_info, 0, false);
    temp->playlist_song_name = insert_parsed_playlist_info(&json_position, &json_playlist_name, 6, true);

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
  while ((json_position = strstr(json_position, json_parse_string)))
	{

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
  while ((json_position = strstr(json_position, find_artist)))
	{
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct spotify_song_search_node *temp = 
    (struct spotify_song_search_node*) malloc(sizeof(struct spotify_song_search_node));

    /* strstr to first "name" and get the album name*/
    temp->album_name = insert_parsed_data(&json_position, &find_artist, 5, true);

    /* strstr to album "name" and get the album api info*/
    temp->album_api_info = insert_parsed_data(&json_position, &json_album_info, 1, true);

    /* go to "name" and extract author name */
    temp->artist_info = insert_parsed_data(&json_position, &find_artist, 5, true);

    /* go to "song" and then call function again to get the "name" */
    insert_parsed_data(&json_position, &find_song, 5, false);

    /* go to "name" and get the song name */
    temp->song_title = insert_parsed_data(&json_position, &find_artist, 5, true);

    /* go to "track" and get the track position */
    temp->album_position = insert_parsed_data(&json_position, &json_track_position, 4, true); 

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
  while ((json_position = strstr(json_position, json_anchor)))
	{
    json_position = &(json_position[strlen(json_anchor) - 1]);

    struct spotify_playlist_context *temp = 
    (struct spotify_playlist_context*) malloc(sizeof(struct spotify_playlist_context));

    /* strstr to first "name" and get the album name*/
    temp->playlist_name = insert_parsed_data(&json_position, &json_playlist_name, 5, true);

    /* strstr to album "name" and get the album api info*/
    temp->playlist_owner = insert_parsed_data(&json_position, &json_display_name, 6, true);

    /* go to "name" and extract author name */
    temp->playlist_uri = insert_parsed_data(&json_position, &json_spotify_uri, 1, true);

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
  while ((json_position = strstr(json_position, find_artist)))
	{
    json_position = &(json_position[strlen(find_artist) - 1]);

    struct spotify_song_search_node *temp = 
    (struct spotify_song_search_node*) malloc(sizeof(struct spotify_song_search_node));

    temp->album_name = insert_parsed_data(&json_position, &find_artist, 5, true);
    temp->artist_info = insert_parsed_data(&json_position, &find_artist, 5, true);
    temp->song_title = insert_parsed_data(&json_position, &find_artist, 5, true);
    temp->album_api_info = insert_parsed_data(&json_position, &json_track_info, 1, true);

    insert_search_node(temp);
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
char *
insert_parsed_playlist_info(char **json_position, const char **search, int displacement, bool insert)
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

    if(insert)
    {
      ssize_t distance = cut(*json_position);

      /* plus one for the null */
      char *ptr = malloc(sizeof(char) * (distance) + 1);
      memcpy(ptr, *json_position, distance);

      /* terminate the string as there is no previous terminator */
      ptr[distance] = 0;

      return ptr;
    }

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
void 
parse_top_tracks(char *json_web_data)
{
  /* search pattern & ignore flag */
  const char *anchor = "https://api.spotify.com/v1/artists/";
  /* gets resued alot because alot of attributes have "name" in them */
  const char *find_name = "\"name\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, anchor)))
	{
    json_position = &(json_position[strlen(anchor) - 1]);
    (json_position = strstr(json_position, anchor));
    json_position = &(json_position[strlen(anchor) - 1]);

    struct spotify_top_tracks *temp = 
    (struct spotify_top_tracks*) malloc(sizeof(struct spotify_top_tracks));

    temp->artist_name = insert_parsed_data(&json_position, &find_name, 5, true);
    temp->song_name = insert_parsed_data(&json_position, &find_name, 5, true);

    insert_top_track_node(temp);
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
char * 
parse_artist_id(char *json_web_data)
{
  /* search pattern & ignore flag */
  const char *anchor = "\"id\" : \"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  
  json_position = strstr(json_position, anchor);
  json_position = json_position + strlen(anchor);

  ssize_t distance = cut(json_position);

  /* plus one for the null */
  char *ptr = malloc(sizeof(char) * (distance) + 1);
  memcpy(ptr, json_position, distance);

  /* terminate the string as there is no previous terminator */
  ptr[distance] = 0;

  return ptr;
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
parse_related_artists(char *json_web_data)
{
  /* search pattern & ignore flag */
  const char *find_genre = "\"genres\" : ";
  /* gets resued alot because alot of attributes have "name" in them */
  const char *find_name = "\"name\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_genre)))
	{
    json_position = &(json_position[strlen(find_genre) - 1]);

    struct spotify_related_artists *temp = 
    (struct spotify_related_artists*) malloc(sizeof(struct spotify_related_artists));

    temp->genre_array = insert_parsed_data(&json_position, &find_genre, 2, true);
    temp->artist_name = insert_parsed_data(&json_position, &find_name, 5, true);

    insert_related_artist_node(temp);
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
parse_searched_artists(char *json_web_data)
{
  /* search pattern & ignore flag */
  const char *anchor = "\"followers\" : ";
  const char *find_genre = "\"genres\" : ";
  /* gets resued alot because alot of attributes have "name" in them */
  const char *find_name = "\"name\"";
  const char *find_id = "\"id\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, anchor)))
	{
    json_position = &(json_position[strlen(anchor) - 1]);

    struct spotify_artists_search_node *temp = 
    (struct spotify_artists_search_node*) malloc(sizeof(struct spotify_artists_search_node));

    temp->genre_array = insert_parsed_data(&json_position, &find_genre, 2, true);
    temp->artists_id = insert_parsed_data(&json_position, &find_id, 5, true);
    temp->artist_name = insert_parsed_data(&json_position, &find_name, 5, true);

    insert_artist_search_node(temp);
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
parse_top_artists(char *json_web_data)
{
  const char *find_name = "\"name\"";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_name)))
	{
    json_position = &(json_position[strlen(find_name) - 1]);

    struct spotify_top_artists *temp = 
    (struct spotify_top_artists*) malloc(sizeof(struct spotify_top_artists));

    temp->artist_name = insert_top_artist(&json_position, &find_name);

    insert_top_artist_node(temp);
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
parse_albums(char *json_web_data)
{
  const char *find_images = "images";
  const char *find_name = "name";
  const char *find_uri = "uri";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, find_images)))
	{
    json_position = &(json_position[strlen(find_images) - 1]);

    struct spotify_album_search_node *temp = 
    (struct spotify_album_search_node*) malloc(sizeof(struct spotify_album_search_node));

    temp->album_name = insert_parsed_data(&json_position, &find_name, 6, true);
    temp->album_id = insert_parsed_data(&json_position, &find_uri, 6, true);

    insert_album_search_node(temp);
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
parse_shows(char *json_web_data)
{
  const char *hook = "copyrights";
  const char *find_description = "\"description\"";
  const char *find_id = "id";
  const char *find_name = "name";
  const char *find_publisher = "publisher";
  const char *find_total_episodes = "total_episodes";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, hook))) 
  {
    json_position = &(json_position[strlen(hook) - 1]);

    struct spotify_podcast_search_node *temp = 
    (struct spotify_podcast_search_node*) malloc(sizeof(struct spotify_podcast_search_node));

    temp->description = insert_parsed_data(&json_position, &find_description, 5, true);
    temp->podcast_id = insert_parsed_data(&json_position, &find_id, 6, true);
    temp->name = insert_parsed_data(&json_position, &find_name, 6, true);
    temp->publisher = insert_parsed_data(&json_position, &find_publisher, 6, true);
    temp->number_of_episodes = insert_parsed_data(&json_position, &find_total_episodes, 5, true);

    insert_show_search_node(temp);
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
parse_device_list(char *json_web_data)
{
  const char *hook = "{";
  const char *find_id = "id";
  const char *find_name = "name";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, hook))) 
  {
    json_position = &(json_position[strlen(hook) - 1]);


    struct spotify_device_list_node *temp =
    (struct spotify_device_list_node*) malloc(sizeof(struct spotify_device_list_node));

    temp->device_id = insert_parsed_data(&json_position, &find_id, 6, true);
    temp->device_name = insert_parsed_data(&json_position, &find_name, 6, true);

    insert_device_list_node(temp);
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
parse_podcast_episodes(char *json_web_data)
{
  const char *hook = "audio_preview";
  const char *find_description = "\"description\"";
  const char *find_duration = "duration";
  const char *find_id = "id";
  const char *find_name = "name";
  const char *find_release_date = "release_date";

  char *json_position = json_web_data;

  /* searches the whole string and finds the sub string
   returns pointer to the beggining of the found string */
  while ((json_position = strstr(json_position, hook))) 
  {
    json_position = &(json_position[strlen(hook) - 1]);

    struct spotify_podcast_episode_node *temp = 
    (struct spotify_podcast_episode_node*) malloc(sizeof(struct spotify_podcast_episode_node));

    temp->description = insert_parsed_data(&json_position, &find_description, 5, true);
    temp->duration_ms = insert_parsed_data(&json_position, &find_duration, 6, true);
    temp->episode_id = insert_parsed_data(&json_position, &find_id, 6, true);
    temp->name = insert_parsed_data(&json_position, &find_name, 6, true);
    temp->release_date = insert_parsed_data(&json_position, &find_release_date, 5, true);

    insert_episode_search_node(temp);
  }
}

/**
 * calls the cut function so that we can extract the data
 * we want and allocate it for further use
 * @param json_position the json that we are traversing
 * @searh the search query we want for the json object
 * @displacement skip over unwanted characters
 * @return heap allocated char * of the data we wanted
 */
char *
insert_parsed_data(char **json_position, const char **search, int displacement, bool insert) 
{
  /* move the pointer to desired value in json obj */
  *json_position = strstr(*json_position, *search);
  if(*json_position == NULL){
    return NULL;
  } 
  else 
  {
    *json_position = *json_position + (strlen(*search) - 1) + displacement;
  }
  /* add displacement to skipp unwanted garbage */

  /* use this function to search for certain delimiters and 'cut' */
  /* the string later */
  if(insert)
  {
    ssize_t distance = cut(*json_position);

    /* plus one for the null */
    char *ptr = malloc(sizeof(char) * (distance) + 1);
    memcpy(ptr, *json_position, distance);

    /* terminate the string as there is no previous terminator */
    ptr[distance] = 0;

    return ptr;
  }

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
char *
insert_top_artist(char **json_position, const char **search) 
{
  /* add displacement to skipp unwanted garbage */
  *json_position = *json_position + (strlen(*search) - 1);

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
 * this function actually gets called multiple times, it gets called
 * as many times as there are still packets to be received. 
 * @param contents - the actual data, can be jsom or html
 * @param size - idk actually lol, but its always 1
 * @param nmemb - the size of the received packet
 * @param user_struct - the struct that we will write to eventually
 * @return size of the packet
 */
char *
parse_auth_token(char **json_res)
{

  //point to start, then point to end
  //use malloc and memcpy to it
  
  char *ptr = malloc(sizeof(char) * 307);
  memcpy(ptr, *json_res + 17, 307);
  ptr[306] = 0;


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
  const u_int16_t MAX_STRING = 800;
  ssize_t distance = 0;

  /* 100 because that's the biggest string i could think off, i would have to do some  */
  /* sort of recursion to get the dynamic size, which isnt worth it if you know */
  /* roughly how big its going to be */
  for(int i = 0; i < MAX_STRING; i++) 
  {
    if(artist_info[i + 1] == '\n')
		{
      return distance;
    }
		else if(artist_info[i + 1] == ',' && artist_info[i + 2] == '\n')
		{
      /* pre increment as we want the return value incremented before hand */
      return ++distance;
    }
		else if(artist_info[i + 1] == ']' && artist_info[i + 2] == ',' && artist_info[i+3] == '\n')
		{
      /* pre increment as we want the return value incremented before hand */
      return ++distance;
    }
		else if(artist_info[i + 1] == '\"' && artist_info[i + 2] == ',' && artist_info[i+3] == '\n')
		{
      /* pre increment as we want the return value incremented before hand */
      return ++distance;
    }

    distance++;
  }

  /* something went wrong */
  return 0;
}
