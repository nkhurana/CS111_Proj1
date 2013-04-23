// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
void execute_SimpleCommand (command_t c, bool time_travel);

int
command_status (command_t c)
{
  return c->status;
}





void
execute_Command (command_t c, bool time_travel)
{
  switch(c->type)
  {

	case AND_COMMAND:
	{
	  execute_command (c->u.command[0], time_travel);
	  if (c->u.command[0]->status == 0)
	  {
		execute_command (c->u.command[1], time_travel);
		c->status = c->u.command[1]->status;
	  }
	  else
	    c->status = c->u.command[0]->status;
	  break;
	}
	case OR_COMMAND:
	{
	  execute_command (c->u.command[0], time_travel);
	  if (c->u.command[0]->status != 0)
	  {
		execute_command (c->u.command[1], time_travel);
		c->status = c->u.command[1]->status;
	  }
	  else
	    c->status = c->u.command[0]->status;
	  break;
	}
	case SEQUENCE_COMMAND:
	{
	  execute_command (c->u.command[0], time_travel);
	  execute_command (c->u.command[1], time_travel);
	  c->status = c->u.command[1]->status;
	  break;
	}
	case PIPE_COMMAND:
	{
	  int fd[2];
	  int bp = fork();
	  if (bp == 0) // child
	  {
	    pipe(fd);
		int ap = fork();
		if (ap == 0)
		{
		  if (dup2(fd[1], 1) != 1)
		    perror("Pipe command: unable to redirect output");
		  execute_command (c->u.command[0], time_travel);
		  close(fd[1]);
		}
		else
		{
		  if (dup2(fd[0], 0) != 0)
		    perror("Pipe command: unable to redirect output");
		  execute_command (c->u.command[1], time_travel);
		  close(fd[0]);
		}
	  }
	  c->status = c->u.command[1]->status;
	  break;
	}
	case SUBSHELL_COMMAND:
	{
	  if (c->input)

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
	  execute_command(c->u.subshell_command, time_travel);
	  c->status = c->u.subshell_command->status;
	}
	case SIMPLE_COMMAND:
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
  }

}