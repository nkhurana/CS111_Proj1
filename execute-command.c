// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"

#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
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
			int bpstatus;
			//fprintf(stderr, "MAIN PID: %d\n", getpid());
            if (bp == 0) // child
            {
                pipe(fd);
                int ap = fork();
                if (ap == -1)
                {
                    perror("Unable to fork");
                    exit(EXIT_FAILURE);
                }
                if (ap == 0)
                {
					//fprintf(stderr, "ap process ID: %d\n", getpid());
                    if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
                        perror("Pipe command: unable to redirect output");
				    close(fd[0]);
                    execute_command (c->u.command[0], time_travel);
					//fprintf(stderr, "ap process has returned\n");
					exit(c->u.command[0]->status);
                }
                else
                {
					//fprintf(stderr, "bp process ID: %d spawned %d\n", getpid(), ap);
                    if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                        perror("Pipe command: unable to read input");
					close(fd[1]);
                    execute_command (c->u.command[1], time_travel);
					//fprintf(stderr, "bp process has returned\n");
					exit(c->u.command[1]->status);
                }
            }
			else
			{
				if(waitpid(bp, &bpstatus, 0) < 0)
				{
					perror("Waiting on child produced error");
					exit(EXIT_FAILURE);
				}
				if ( WIFEXITED(bpstatus) )
                {
					c->status = WEXITSTATUS(bpstatus);
                }
			}
            break;
        }
        case SUBSHELL_COMMAND:
        {
            if (c->input)
            {
                int fdi = open(c->input, O_RDONLY);
                if (dup2(fdi, STDIN_FILENO) != STDIN_FILENO)
                    perror("Unable to redirect input");
                close(fdi);
            }
            if (c->output)
            {
                int fdo = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (dup2(fdo, STDOUT_FILENO) != STDOUT_FILENO)
                    perror("Unable to redirect output");
                close(fdo);
            }
            execute_command(c->u.subshell_command, time_travel);
            c->status = c->u.subshell_command->status;
            break;
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
                        if (dup2(fdi, STDIN_FILENO) != STDIN_FILENO)
                            perror("Unable to redirect input");
                        close(fdi);
                    }
                    
                    if (c->output)
                    {
                        int fdo = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        if (dup2(fdo, STDOUT_FILENO) != STDOUT_FILENO)
                            perror("Unable to redirect output");
                        close(fdo);
                    }
					
					//fprintf(stderr, "Executing command %s in process %d\n", cmd, getpid());
                    status = execvp(cmd, c->u.word);
					perror(cmd);
                    exit(status); // only happens if execvp(2) fails
                default: // in parent
                    if (waitpid(pid, &status, 0) < 0)
                    {
                        perror("Waiting on child produced error");
                        exit(EXIT_FAILURE);
                    }
                    
                    if ( WIFEXITED(status) )
                    {
						//fprintf(stderr, "Parent process %d: Finished executing %s in child %d\n", getpid(), cmd, pid);
                        // return status from child
                        c->status = WEXITSTATUS(status);
                    }
            }
        }
    }
    
}