#include "spotify_utils.h"

u_int8_t get_url_size(char *search)
{
  u_int8_t url_size = 0;

  /* create str size based on special chars found */
  for(u_int8_t i = 0; i < strlen(search); i++) {
    if(*(search + i) == ' '|| *(search + i) == ':' || *(search + i) == '\'') {
      url_size+=3;
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

void build_url(char *new_str, char* search, u_int8_t url_size)
{
  ssize_t i = 0;
  ssize_t j = i;

  /* url spaces */
  const char space[3] = {'%','2','0'};

  /* url commas */
  const char comma[3] = {'%','2','C'};

  /* url colons */
  const char colon[3] = {'%','3','A'};

  while(i < url_size) {
    *(new_str + j) = *(search + i);

    switch(*(search + i)) {
      case ' ': 
	*(new_str + j) = space[0];
	*(new_str + j+1) = space[1];
	*(new_str + j+2) = space[2];
	j+=3;
	break;
      case ',':
	*(new_str + j) = comma[0];
	*(new_str + j+1) = comma[1];
	*(new_str + j+2) = comma[2];
	j+=3;
	break;
      case ':':
	*(new_str + j) = colon[0];
	*(new_str + j+1) = colon[1];
	*(new_str + j+2) = colon[2];
	j+=3;
	break;
      default:
	j++;
    }
	i++;
  }
}


char *perform_memcopy(char *new_str)
{
  char *encoded_url;

  if ((encoded_url = malloc(strlen(new_str) + 1)) != NULL) {
    encoded_url[0] = '\0';

    memcpy(encoded_url, new_str, strlen(new_str));
  } 
  else 
  {
    printf("malloc failed!\n");
  }

  return encoded_url;
}

/**
 * adds the appropaite characters to the url
 * @param search - the url we want to encode
 * @return the encoded url
 */
char* 
url_encoder(char* search, bool query) 
{

  u_int8_t url_size = get_url_size(search);

  /* the new encoded url */
  char new_str[url_size];

  build_url(new_str, search, url_size);

  if(query) {
    strcat(new_str, "&type=track&limit=5");
  }

  char *encoded_url = perform_memcopy(new_str);

  return encoded_url;
}

char* 
concat_strings(char *dest, char* to_append)
{
  char *concat_string;

  if ((concat_string = malloc(strlen(dest) + strlen(to_append) + 1)) != NULL) {
    concat_string[0] = '\0';

    strcat(concat_string, dest);
    strcat(concat_string, to_append);
  } 
  else 
  {
    printf("malloc failed!\n");
  }

  return concat_string;
}
