// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "command-internals.h"

void *free_list[3];

static char const *program_name;
static char const *script_name;

static void
usage (void)
{
  error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

void
free_token_stream()
{
  token_node* it = free_list[TOKEN_STREAM];
  while(it->next != NULL)
  {
    free(it->m_token.word);
    it = it->next;
	free(it->previous);
  }
  free(it->m_token.word);
  free(it);
}

void
free_top_level_command()
{
  top_level_command *it = free_list[TOP_LEVEL_COMMAND];
  free(it);
}

void
free_command(command_t c)
{
  switch(c->type)
  {
    case AND_COMMAND:
	case SEQUENCE_COMMAND:
	case OR_COMMAND:
	case PIPE_COMMAND:
	{
	  free_command(c->u.command[0]);
	  free_command(c->u.command[1]);
	  break;
	}
	case SIMPLE_COMMAND:
	{
	  free(c->u.word);
	  break;
	}
	case SUBSHELL_COMMAND:
	{
	 free(c->u.subshell_command);
	 break;
	}
  }
}

void
free_command_stream()
{
  command_stream_t it = free_list[COMMAND_STREAM];
  command_t *c = it->commands;
  int i;
  for (i = 0; i < it->size; i++)
  {
    free_command(c[i]);
  }
  free(c);
}

int
main (int argc, char **argv)
{
  int command_number = 1;
  bool print_tree = false;
  bool time_travel = false;
  program_name = argv[0];

  for (;;)
    switch (getopt (argc, argv, "pt"))
      {
      case 'p': print_tree = true; break;
      case 't': time_travel = true; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();
    //hi!

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream); //PART 1A
 
  command_t last_command = NULL;
  command_t command;
  while ((command = read_command_stream (command_stream))) //PART 1A
    {
    if (print_tree)
	{
	  printf ("# %d\n", command_number++);
	  print_command (command);
	}
      else
	{
	  last_command = command;
	  execute_command (command, time_travel);
	}
    }

  //free_token_stream();
  //free_top_level_command();
  // free_command_stream();
  
  return print_tree || !last_command ? 0 : command_status (last_command);
}
