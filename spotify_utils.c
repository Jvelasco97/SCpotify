#include "spotify_utils.h"

/**
 * adds the appropaite characters to the url
 * @param search - the url we want to encode
 * @return the encoded url
 */
char* 
url_encoder(char* search) 
{

  int url_size = 0;

  for(int i = 0; i < strlen(search); i++) {
    if(*(search + i) == ' ') {
      url_size+=3;
    } else {
      url_size++;
    }
  }

  /* the new encoded url */
  char new_str[url_size];

  /* url spaces */
  const char space[3] = {'%','2','0'};

  /* url commas */
  const char comma[3] = {'%','2','C'};

  /* url colons */
  const char apostraphe[3] = {'%','3','A'};


  ssize_t i = 0;
  ssize_t j = i;


  /* replace the spaces with the encoded ones */
  /* i used two pointers to keep track of the positions in  */
  /* both arrays */

  /* have both arrays start at 0, if we hit a space, we */
  /* append the special chars and we increment the pointer of the */
  /* new_str by 3 because thats the length of the special char arr */
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
      case '\'':
	*(new_str + j) = apostraphe[0];
	*(new_str + j+1) = apostraphe[1];
	*(new_str + j+2) = apostraphe[2];
	j+=3;
	break;
      default:
	j++;
    }
	i++;
  }


  /* &type=track */
  /* add a space so we can append the rest */
  /* we can change the number of results here */
  strcat(new_str, "&type=track&limit=5");

  /* were returning a char * */
  char *encoded_url = new_str;

  return encoded_url;
}
