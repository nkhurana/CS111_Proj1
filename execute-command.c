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
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library. 
  error (1, 0, "command execution not yet implemented");*/
 
  char *cmd = c->u.word[0];
  pid_t pid;
  int status;
 
  switch ( pid = fork() ) {
    case -1:
      perror("fork()");
      exit(EXIT_FAILURE);
    case 0: // in the child
	  if (c->input)
	  {
	    int fd = open(c->input, O_RDONLY);
		if (dup2(fd, 0) != 0);
			perror("Unable to redirect input");
		close(fd);
	  }
	  
	  if (c->output)
	  {
	    int fd = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dup2(fd, 1);
		close(fd);
	  }
	  
      status = execvp(cmd, c->u.word);
      exit(status); // only happens if execvp(2) fails
    default: // in parent
      if ( waitpid(pid, &status, 0) < 0 ) {
        perror("waitpid()");
        exit(EXIT_FAILURE);
      }

      if ( WIFEXITED(status) ) {
        // return status from child
        exit( WEXITSTATUS(status) );
      }
      exit(EXIT_FAILURE);
  }
}