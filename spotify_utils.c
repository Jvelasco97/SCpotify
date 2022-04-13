#include "spotify_utils.h"

/**
 * @brief an optimzed version of strcat
 * @param dest
 * @param src
 * @return char - the concatinated string
 */
char *
mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while ((*dest++ = *src++));
     return --dest;
}

/**
 * @brief gets the length based on url encoded characters
 * @param search - the string from the JSON response
 * @return the size of the url
 */
u_int8_t
get_url_size(char *search)
{
  u_int8_t url_size = 0;

  size_t search_size = 0;
  search_size = strlen(search);

  /* create str size based on special chars found */
  for (u_int8_t i = 0; i < search_size; i++)
	{
    if (*(search + i) == ' ' || *(search + i) == ':' || *(search + i) == '\'') {
      url_size += 3;
    } else {
      url_size++;
    }
  }

  return url_size;
}

/* replace the spaces with the encoded ones */
/* i used two pointers to keep track of the positions in  */
/* both arrays */

/* have both arrays start at 0, if we hit a space, we */
/* append the special chars and we increment the pointer of the */
/* new_str by 3 because thats the length of the special char arr */

/* add a space so we can append the rest */
/* we can change the number of results here */

void
build_url(char *new_str, const char *search, u_int8_t url_size)
{
  ssize_t i = 0;
  ssize_t j = i;

  /* url spaces */
  const char space[3] = {'%', '2', '0'};

  /* url commas */
  const char comma[3] = {'%', '2', 'C'};

  /* url colons */
  const char colon[3] = {'%', '3', 'A'};

  while (i < url_size)
	{
    *(new_str + j) = *(search + i);

    switch (*(search + i))
		{
    case ' ':
      *(new_str + j) = space[0];
      *(new_str + j + 1) = space[1];
      *(new_str + j + 2) = space[2];
      j += 3;
      break;
    case ',':
      *(new_str + j) = comma[0];
      *(new_str + j + 1) = comma[1];
      *(new_str + j + 2) = comma[2];
      j += 3;
      break;
    case ':':
      *(new_str + j) = colon[0];
      *(new_str + j + 1) = colon[1];
      *(new_str + j + 2) = colon[2];
      j += 3;
      break;
    default:
      j++;
    }
    i++;
  }
}

/**
 * @brief memcopy the all parts from the encode url
 * @param new_str
 * @return encoded_url
 */
char *
perform_memcopy(char *new_str)
{
  char *encoded_url = NULL;
  size_t new_str_len = strlen(new_str); // added this line to prevent multiple unnecessary calls to strlen

  if ((encoded_url = malloc(new_str_len + 1)) != NULL)
	{
    encoded_url[new_str_len] = '\0'; // changed this line so it sets the end char to 0 instead of the first char to 0

    memcpy(encoded_url, new_str, new_str_len);
  }
	else
	{
    printf("malloc failed!\n");
  }
  return encoded_url;
}

/**
 * @brief adds the query parameters
 * @param search - the url we want to encode
 * @param searc_type
 * @return encoded_url
 */
char *
url_encoder(char *search, const int search_type)
{
  ssize_t url_size = get_url_size(search);

  /* the new encoded url */

  char new_str[url_size+ 20];
  memset(new_str, 0, url_size + 20);

  build_url(new_str, search, url_size);

  if(search_type == SONG_QUERY)
    strcat(new_str, "&type=track&limit=5");
  if(search_type == ARTIST_QUERY)
    strcat(new_str, "&type=artist&limit=5");
  if(search_type == PODCAST_QUERY)
    strcat(new_str, "&type=show&limit=5");

  char *encoded_url = perform_memcopy(new_str);

  return encoded_url;
}

/**
 * @brief concats the strings
 * @param dest
 * @param to_append
 * @return concat_string
 */
char *
concat_strings(char *dest, char *to_append)
{
  char *concat_string = NULL;

  if ((concat_string = malloc(strlen(dest) + strlen(to_append) + 1)) != NULL)
	{
    concat_string[0] = '\0';
    mystrcat(concat_string, dest);
    mystrcat(concat_string, to_append);
  }
	else
	{
    printf("malloc failed!\n");
  }

  return concat_string;
}
