// UCLA CS 111 Lab 1 command internals

enum command_type
  {
    AND_COMMAND,         // A && B
    SEQUENCE_COMMAND,    // A ; B
    OR_COMMAND,          // A || B
    PIPE_COMMAND,        // A | B
    SIMPLE_COMMAND,      // a simple command
    SUBSHELL_COMMAND,    // ( A )
  };

// Data associated with a command.
struct command
{
  enum command_type type;

  // Exit status, or -1 if not known (e.g., because it has not exited yet).
  int status;

  // I/O redirections, or null if none.
  char *input;
  char *output;

  union
  {
    // for AND_COMMAND, SEQUENCE_COMMAND, OR_COMMAND, PIPE_COMMAND:
    struct command *command[2];

    // for SIMPLE_COMMAND:
    char **word;

    // for SUBSHELL_COMMAND:
    struct command *subshell_command;
  } u;
};





typedef enum {
    SEMICOLON_TOKEN,
    NEWLINE_TOKEN,
    AND_TOKEN,
    OR_TOKEN,
    GREATER_TOKEN,
    WORD_TOKEN,
    LEFT_PAREN_TOKEN,
    RIGHT_PAREN_TOKEN,
    LESS_TOKEN,
    COMMENT_TOKEN,
    PIPE_TOKEN,
} token_type;

typedef struct token{
    token_type type;
    char* word; //NULL if not WORD_TOKEN
} token;

typedef struct token_node{
    token m_token;
    struct token_node* next;
    struct token_node* previous;
} token_node;
