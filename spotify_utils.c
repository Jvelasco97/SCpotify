#include "spotify_utils.h"

/**
 * adds the appropaite characters to the url
 * @param search - the url we want to encode
 * @return the encoded url
 */
char* 
url_encoder(char* search) 
{

  /* url spaces */
  const char space[3] = {'%','2','0'};

/*   const char comma[3] = {'%','2','C'}; */
/*  */
/*   const char colon[3] = {'%','3','A'}; */

  /* TODO - make it dynamic */
  char new_str[50];

  ssize_t i = 0;
  ssize_t j = i;

  /* replace the spaces with the encoded ones */
  /* i used two pointers to keep track of the positions in  */
  /* both arrays */

  /* have both arrays start at 0, if we hit a space, we */
  /* append the special chars and we increment the pointer of the */
  /* new_str by 3 because thats the length of the special char arr */
  while(i < 50) {
    *(new_str + j) = *(search + i);

    if(*(search + i) == ' ') {
      *(new_str + j) = space[0];
      *(new_str + j+1) = space[1];
      *(new_str + j+2) = space[2];
      j+=3;
    } else {
      /* else we just go next by one */
      j++;
    }
    i++;
  }

  /* &type=track */
  /* add a space so we can append the rest */
  new_str[j-2] = ' ';
  /* we can change the number of results here */
  strcat(new_str, "&type=track&limit=5");

  /* were returning a char * */
  char *encoded_url = new_str;

  return encoded_url;
}
