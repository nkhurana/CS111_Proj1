// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, bool time_travel)
{
  char *cmd = c->u.word[0];
  pid_t pid;
  int status;
 
  switch ( pid = fork() )
  {
    case -1:
      perror("Unable to fork");
      exit(EXIT_FAILURE);
    case 0: // in the child
	  if (c->input)
	  {
	    int fdi = open(c->input, O_RDONLY);
		if (dup2(fdi, 0) != 0)
		  perror("Unable to redirect input");
		close(fdi);
	  }
	  
	  if (c->output)
	  {
	    int fdo = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (dup2(fdo, 1) != 1)
		  perror("Unable to redirect output");
		close(fdo);
	  }
	  
      status = execvp(cmd, c->u.word);
      exit(status); // only happens if execvp(2) fails
    default: // in parent
      if ( waitpid(pid, &status, 0) < 0 )
	  {
        perror("Waiting on child produced error");
        exit(EXIT_FAILURE);
      }

      if ( WIFEXITED(status) )
	  {
        // return status from child
        c->status = WEXITSTATUS(status);
      }
  }
}