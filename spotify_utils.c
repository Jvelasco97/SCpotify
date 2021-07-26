#include "spotify_utils.h"

char* 
url_encoder(char* search) 
{

  const char space[3] = {'%','2','0'};

/*   const char comma[3] = {'%','2','C'}; */
/*  */
/*   const char colon[3] = {'%','3','A'}; */

  char new_str[50];

  ssize_t i = 0;
  ssize_t j = i;
  while(i < 30) {
    *(new_str + j) = *(search + i);

    if(*(search + i) == ' ') {
      *(new_str + j) = space[0];
      *(new_str + j+1) = space[1];
      *(new_str + j+2) = space[2];
      j+=3;
    } else {
      j++;
    }
    i++;
  }

  /* &type=track */
  new_str[j-2] = ' ';
  strcat(new_str, "&type=track&limit=5");

  char *encoded_url = new_str;

  return encoded_url;
}
