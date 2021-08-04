#include <getopt.h> /* getopt */
#include <stdlib.h> /* exit   */
#include <stdio.h>  /* printf */

int main(int argc, char **argv)
{
  int c;

  while (1) 
  {
      int option_index = 0;
      static struct option long_options[] = 
      {
          {"add",     required_argument, NULL,  0 },
          {"append",  no_argument,       NULL,  0 },
          {"delete",  required_argument, NULL,  0 },
          {"verbose", no_argument,       NULL,  0 },
          {"repeat",     no_argument,       NULL,  0 },
          {"create",  required_argument, NULL,  0 },
          {"file",    optional_argument, NULL,  0 },
          {NULL,      0,                 NULL,  0 }
      };

      c = getopt_long(argc, argv, "-:abc:d::", long_options, &option_index);
      if (c == -1)
           break;

      switch (c) 
      {
        case 0:
          printf("long option %s", long_options[option_index].name);
          if (optarg)
             printf(" with arg %s", optarg);
          printf("\n");
          break;

        case 1:
          printf("regular argument '%s'\n", optarg); /* non-option arg */
          break;

        case 'a':
          printf("option a\n");
          break;

       case 'b':
          printf("option b\n");
          break;

        case 'c':
          printf("option c with value '%s'\n", optarg);
          break;

        case 'd':
          printf("option d with value '%s'\n", optarg ? optarg : "NULL");
          break;

        case '?':
          printf("Unknown option %c\n", optopt);
          break;

        case ':':
          printf("Missing option for %c\n", optopt);
          break;

        default:
          printf("?? getopt returned character code 0%o ??\n", c);
       }
  }

  return 0;
}
