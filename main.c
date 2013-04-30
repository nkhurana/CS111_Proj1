// UCLA CS 111 Lab 1 main program

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

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
	 free_command(c->u.subshell_command);
	 break;
	}
  }
  if (read_head)
  {
    read_dependency_node* itr = read_head;
	while (itr != NULL)
	{
	  read_dependency_node* temp = itr;
	  itr = itr->next;
	  free(temp);
	}
  }
  if (write_head)
  {
    write_dependency_node* itr = write_head;
	while (itr != NULL)
	{
	  write_dependency_node* temp = itr;
	  itr = itr->next;
	  free(temp);
	}
  }
}

void
free_tlc_dependencies(tlc_wrapper_t tlc)
{
  free_command(tlc->command);
  dependency_token *itr = tlc->head;
  while(itr != NULL)
  {
    dependency_token* temp = itr;
	itr = itr->next;
	free (temp);
  }
}

void
free_command_stream()
{
  command_stream_t s = free_list[COMMAND_STREAM];
  tlc_wrapper_t *c = s->commands;
  int i;
  for (i = 0; i < s->size; i++)
  {
    free_tlc_dependencies(c[i]);
  }
  free(c);
  free(s);
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


  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream); //PART 1A
 
  command_t last_command = NULL;
  command_t command;
  tlc_wrapper_t tlc;
  
    
  while ((tlc = read_command_stream (command_stream))) //PART 1A
  {
    command = tlc->command;
    if (print_tree)
	{
	  printf ("# %d\n", command_number++);
	  print_command (command);
    }
    else
	{
	  
      last_command = command;
	  if (!time_travel)
	    execute_command (command, time_travel);
	  else // time_travel!
	  {
	    if (tlc->nDependsOn == 0) // run if no dependencies
		{
		  pid_t pid = fork();
		  if (pid == 0) // child
		  {
		    execute_command (command, time_travel);
			exit(command->status); // done executing
		  } 
		  else
		    tlc->pid = pid; // assign pid to recover
		}
	  }
	}
  }
  
  if (time_travel)
  {
	pid_t child_pid;
	int status;
	int n_processes_returned = 0;
	while(((child_pid = waitpid(-1, &status, 0)) != -1)) // wait for all children
	{
	  reset_command_stream_itr(command_stream); // processes don't run in order
	  tlc_wrapper_t match;
	  while((match = read_command_stream(command_stream)))
	  {
	    if (match->pid == child_pid) // find tlc whose command was run
	    {
		  dependency_token *itr = match->head; // decrement its dependents
		  while(itr != NULL)
		  {
		    tlc_wrapper_t dependent = itr->tlc;
		    dependent->nDependsOn--;
		    if (dependent->nDependsOn == 0) // dependent is free to run
		    {
			  pid_t new = fork();
		   	  if (new == 0)
			  {
			    last_command = dependent->command;
			    execute_command (dependent->command, time_travel);
			    exit(dependent->command->status);
			  }
			  else
			    dependent->pid = new;
			}
		    itr = itr->next;
		  }
		  match->pid = 0; // done
		  n_processes_returned++;
		}
	  }
	}
	if (n_processes_returned != command_stream->size)
	  error(1, 0, "Not all processes returned");
  }

  free_token_stream();
  free_top_level_command();
  free_command_stream();

  return print_tree || !last_command ? 0 : command_status (last_command);
}
