/**
 * @file spotify.c
 * @author Juan Velasco
 * @date July 26, 2021
 */
#include "main_includes.h"
#include "spotify_structs.h"
#include <stdio.h>

int main(int argc, char **argv)
{


  /* instantiate the struct for the command line and search queries */
  scpotify_context *cmd_args     = (scpotify_context*) malloc(sizeof(scpotify_context));
  spotify_search_context *search = (spotify_search_context*) malloc(sizeof(spotify_search_context));
  spotify_auth_payload *auth     = (spotify_auth_payload*) malloc(sizeof(spotify_auth_payload));
  spotify_song_query_info *req   = (spotify_song_query_info*) malloc(sizeof(spotify_song_query_info));

  cmd_args->search_struct = search;
  cmd_args->auth_struct   = auth;

	req->track_info     = NULL;
	req->track_position = NULL;

  init_cmd_args(cmd_args);

  /* process the command line options */
  if(argc > 1)
  {
		transfer_playback(cmd_args);
    process_args(argc, argv, cmd_args);

    /* call http and parse */
    spotifyC(cmd_args, req);

		free_auth_payload(cmd_args); 
		free_args_and_search(cmd_args, search, auth); 
  } 
  else
  {
    print_help();
    free_args_and_search(cmd_args, search, auth);
  }

  return 0;
}
