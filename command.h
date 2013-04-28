// UCLA CS 111 Lab 1 command interface

#include <stdbool.h>
#include <stdio.h>

typedef struct command *command_t;
typedef struct tlc_wrapper* tlc_wrapper_t;
typedef struct command_stream *command_stream_t;
typedef struct token token;
typedef struct token_node token_node;
typedef struct dependency_token dependency_token;
typedef struct read_dependency_node read_dependency_node;
typedef struct write_dependency_node write_dependency_node;

struct command_stream
{
  tlc_wrapper_t* commands;
  tlc_wrapper_t* it;
  int size;
};

struct dependency_token
{
    tlc_wrapper_t tlc;
    dependency_token* next;
};


struct tlc_wrapper
{
    command_t command;
    int nDependsOn;
    dependency_token* head;
};


typedef struct 
{
  token_node* head;
  token_node* tail;
} top_level_command;

typedef struct
{
  top_level_command* commands;
  int size;
} top_level_command_t;

command_t CreateCommand(token_node* head, token_node* tail);

bool isValidCharacterForWordToken(char character);

char* ReadFileIntoCharacterBuffer (int (*get_next_byte) (void *), void *get_next_byte_argument, int* bufferEndIndex);

top_level_command_t isSanitized_token_stream (token_node* head);

void remove_newline_tokens(top_level_command c);

void output_read_error(int line, token node);

/* Create a command stream from GETBYTE and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
tlc_wrapper_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t);

/* Execute a command.  Use "time travel" if the flag is set.  */
void execute_command (command_t, bool);

/* Return the exit status of a command, which must have previously
   been executed.  Wait for the command, if it is not already finished.  */
int command_status (command_t);
