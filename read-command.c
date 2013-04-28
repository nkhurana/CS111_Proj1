// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool isValidCharacterForWordToken(char c)
{
    if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' || c == '.' || c == '/' || c == ':' || c == '@' || c == '^' || c == '_')
        return true;
    return false;
}

char* ReadFileIntoCharacterBuffer (int (*get_next_byte) (void *), void *get_next_byte_argument, int* bufferEndIndex)
{
    size_t bufferSize = 512; //set initial size to 512 bytes
    *bufferEndIndex = 0;
    char value;
    char* buffer = (char*) checked_malloc(bufferSize);
    
    //store every character into buffer
    while ((value = get_next_byte(get_next_byte_argument)) != EOF) 
    {
        buffer[*bufferEndIndex] = value;
        (*bufferEndIndex)++;
        if (bufferSize == *((size_t*) bufferEndIndex))
        {
            bufferSize*=2;
            buffer =  (char*)checked_realloc(buffer, bufferSize);
        }
    }
    return buffer;
}

command_stream_t make_command_stream (int (*get_next_byte) (void *),void *get_next_byte_argument)
{
    //READ FROM FILE INTO CHARACTER BUFFER
    int bufferEndIndex=0;
    char* buffer = ReadFileIntoCharacterBuffer(get_next_byte, get_next_byte_argument,&bufferEndIndex); 
    
    //TOKENIZE THE CHARACTER BUFFER AND CONSTRUCT TOKEN_NODE
    
    command_stream_t cstream =(command_stream_t) checked_malloc(sizeof(struct command_stream));
	free_list[COMMAND_STREAM] = cstream;
    cstream->size = 0;
    //ensure file wasn't empty
    if(bufferEndIndex == 0)
    {
		cstream->commands = NULL;
		cstream->it = NULL;
        return cstream; //REPLACE W/ PROPER CODE
    }
    
    token_node *head=NULL;
    token_node* tail=NULL;
    int lineNumberCounter = 1; //used for error messages
    token_type typeOfToken;
    int i;
    for (i=0; i<bufferEndIndex;i++) //iterate through character buffer
    {
        if ((buffer[i] == ' ') || (buffer[i]== '\t'))
        {
            continue;
        }
        char* word = NULL;
        if (buffer[i] == '|')
        {
            if (((i+1)!= bufferEndIndex) && (buffer[i+1] == '|'))
            {
                typeOfToken=OR_TOKEN;
                i++;
            }
            else
                typeOfToken=PIPE_TOKEN;
        }        
        else if (buffer[i] == '&')
        {
            if (((i+1)== bufferEndIndex) || buffer[i+1] != '&')
            {
                error(1, 0, "Line %d: Incorrect AND command'", lineNumberCounter);
            }
            typeOfToken=AND_TOKEN;
            i++;
        }        
        else if (buffer[i] == '#') //comment
        {
            if (i!=0)
            {
                if (!(isspace(buffer[i-1])))
                {
                     error(1, 0, "Line %d: Improper comment declaration'", lineNumberCounter);
                }
             }
           
            while(((i+1)!=bufferEndIndex) && (buffer[i+1] != '\n')){
                i++;}
  
            continue;
        }        
        else if (buffer[i] == '\n')
        {
            typeOfToken = NEWLINE_TOKEN;
            lineNumberCounter++;
        }        
        else if (buffer[i] == ')')
        {
            typeOfToken = RIGHT_PAREN_TOKEN;
        }       
        else if (buffer[i] == '<')
        {
            //puts("HI");
            typeOfToken = LESS_TOKEN;
        }        
        else if (buffer[i] == ';')
        {
            typeOfToken = SEMICOLON_TOKEN;
        }       
        else if (buffer[i] == '(')
        {
            typeOfToken = LEFT_PAREN_TOKEN;
        }             
        else if (buffer[i] == '>')
        {
            typeOfToken = GREATER_TOKEN;
        }        
        else if (isValidCharacterForWordToken(buffer[i]))
        {
            int length = 1;
            while (((i+length)!=bufferEndIndex) && isValidCharacterForWordToken(buffer[i+length])) //get adjacent characters
            {
                length++;
            }
            word = (char*) checked_malloc ((sizeof(char)*length)+1);
            int x;
            //printf("length: %i \n", length);
            for (x= 0; x <length; x++) {
                word[x] = buffer[i+x];}
            i += (length - 1);
            word[length] = '\0';
            typeOfToken=WORD_TOKEN;
        }
        else
		{
		  free(word);
		  error(1, 0, "Line %d: Unrecognizable character'", lineNumberCounter);
		}
        
        token current_token;
        current_token.type=typeOfToken;
        current_token.word=word;
        
        token_node* current_token_node = (token_node*) checked_malloc(sizeof(token_node));
        if (head==NULL)
            head=current_token_node;
        
        current_token_node->m_token=current_token;
        current_token_node->next=NULL;
        current_token_node->previous=tail;
        if (tail!=NULL)
            tail->next=current_token_node;
        tail = current_token_node;
        
    }
	head->previous = NULL;

	//==============Sanitize token stream===========//
    free_list[TOKEN_STREAM] = head;
	top_level_command_t c = isSanitized_token_stream(head);
    
    int max_numberOfCommands = 100;
	cstream->commands = (tlc_wrapper *) checked_malloc(max_numberOfCommands*sizeof(tlc_wrapper *));
	
	//=========Changes the tokens into commands============//
    for (i = 0; i < c.size; i++)
	{
	  top_level_command t = c.commands[i];
	  remove_newline_tokens(t);
	  token_node *itr = t.head;
	  while(itr != t.tail)
	  {
		itr = itr->next;
	  }
	  
	  command_t command = CreateCommand(t.head, t.tail);
	  tlc_wrapper *new = (tlc_wrapper *) checked_malloc (sizeof(tlc_wrapper));
	  new->command = command;
	  new->nDependsOn = 0;
	  new->head = NULL;
	  (cstream->commands)[cstream->size] = new;

	  cstream->size++;  
      if (cstream->size == max_numberOfCommands) 
      {
          max_numberOfCommands*=2;
          cstream->commands = checked_realloc(cstream->commands, (max_numberOfCommands*sizeof(tlc_wrapper*)));
      }
	}
	(cstream->commands)[cstream->size] = NULL;
	cstream->it = cstream->commands;
    
    return cstream;
}





command_t CreateCommand(token_node* head, token_node* tail)
{
    //BASE CASE: simple command is one word
    if (head == tail)
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SIMPLE_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
        command->u.word = checked_malloc(sizeof(char*)*2);
        *(command->u.word) = head->m_token.word;
        (command->u.word)[1] = NULL;
        //puts(head->m_token.word);
        return command;
    }
    
    token_node* itr = head;
    int totalNodes=0;
    int numWordNodes=0;
    int numWordsBeforeRedirection = 0;
    int AND_index_placeholder = 0;
    int OR_index_placeholder = 0;
    
    token_node* ptr_to_AND_Token = NULL;
    token_node* ptr_to_OR_Token = NULL;
    token_node* ptr_to_PIPE_Token = NULL;
    
    token_node* ptr_to_LESSTHAN_Token = NULL;
    token_node* ptr_to_GREATERTHAN_Token = NULL;
    token_node* ptr_to_SEMICOLON_Token=NULL;
    
    bool SubshellSpotted = false;
    int Subshell_balancing = 0; //+ for every ( ; - for every )
    
    
    
    
    while ((itr != tail->next))
    {
        
        if (itr->m_token.type ==LEFT_PAREN_TOKEN)
        {
            SubshellSpotted=true;
            Subshell_balancing++;
        }
        if (itr->m_token.type ==RIGHT_PAREN_TOKEN)
        {
            SubshellSpotted=true;
            Subshell_balancing--;
        }
        
        
        
        if (Subshell_balancing==0) //not within subshell
        {
            if ((!ptr_to_GREATERTHAN_Token) && (!ptr_to_LESSTHAN_Token))
            {
                numWordsBeforeRedirection++;
            }
            
            if (itr->m_token.type==SEMICOLON_TOKEN)
            {
                ptr_to_SEMICOLON_Token=itr;
            }
        
        
            if (itr->m_token.type== LESS_TOKEN)
            {
                ptr_to_LESSTHAN_Token = itr;
            }
        
        
            if (itr->m_token.type== GREATER_TOKEN)
            {
                ptr_to_GREATERTHAN_Token = itr;
            }
        
            if (itr->m_token.type == PIPE_TOKEN)
            {
                ptr_to_PIPE_Token = itr;
            }   
        
            if (itr->m_token.type == AND_TOKEN)
            {
            
                ptr_to_AND_Token = itr;
                AND_index_placeholder=totalNodes;
            }
        
            if (itr->m_token.type == OR_TOKEN)
            {
            
                ptr_to_OR_Token = itr;
                OR_index_placeholder=totalNodes;
            }
        
            if (itr->m_token.type == WORD_TOKEN)
            {
                numWordNodes++;
            }
        

            totalNodes++;
        }
        itr = itr->next;
    }
    
    //SUBSHELL case
    if ((SubshellSpotted) && (!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&& (!ptr_to_LESSTHAN_Token) && (!ptr_to_GREATERTHAN_Token))
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SUBSHELL_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
        command->u.subshell_command = CreateCommand(head->next,tail->previous);
        return command;
    }
    
    
    //BASE CASE: simple command is multiple words
    if (totalNodes==(numWordNodes))
    {
        //printf("total #:%i \n", totalNodes);
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SIMPLE_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
		
        command->u.word = (char**)checked_malloc((sizeof(char*))*(numWordNodes+1));
        itr = head;
        int index = 0;
    
        while ((itr != tail->next))
        {
            if (itr->m_token.type == WORD_TOKEN)
            {
                (command->u.word)[index++] = itr->m_token.word;
                //puts(itr->m_token.word);
                //puts("\n");
            }
            itr = itr->next;
        }
        //printf("index #:%i \n", index);
        (command->u.word)[index] = NULL;
        
        
        
        return command;
    }
    
    
    
    //basic redirection w/ only <
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_SEMICOLON_Token) && (!ptr_to_PIPE_Token)&&(ptr_to_LESSTHAN_Token) && (!ptr_to_GREATERTHAN_Token))
    {
        
        if (ptr_to_LESSTHAN_Token->previous->m_token.type == RIGHT_PAREN_TOKEN)
        {
            command_t command = checked_malloc(sizeof(struct command));
            command->type = SUBSHELL_COMMAND;
            command->status = -1;
            int subshell_balancing_internal = 1;
            token_node *itr_To_Left_Paren = (ptr_to_LESSTHAN_Token->previous->previous);
            while (itr_To_Left_Paren!=NULL && subshell_balancing_internal!=0)
            {
                if (itr_To_Left_Paren->m_token.type == RIGHT_PAREN_TOKEN)
                    subshell_balancing_internal++;
                if (itr_To_Left_Paren->m_token.type == LEFT_PAREN_TOKEN)
                    subshell_balancing_internal--;
                if (subshell_balancing_internal!=0)
                    itr_To_Left_Paren=itr_To_Left_Paren->previous;
            }
            command->u.subshell_command=CreateCommand(itr_To_Left_Paren->next, ptr_to_LESSTHAN_Token->previous->previous);
            command->output = NULL;
            command->input = ptr_to_LESSTHAN_Token->next->m_token.word;
            return command;
            
        }
        else
        {        
            command_t command = checked_malloc(sizeof(struct command));
            command->type = SIMPLE_COMMAND;
            command->status = -1;
        
            command->u.word = (char**)checked_malloc((sizeof(char*))*(numWordsBeforeRedirection+1));
            itr = head;
            int index = 0;
        
            while ((itr != ptr_to_LESSTHAN_Token))
            {
                if (itr->m_token.type == WORD_TOKEN)
                {
                    (command->u.word)[index++] = itr->m_token.word;
                    //puts(itr->m_token.word);
                    //puts("\n");
                }
                itr = itr->next;
            }
            //printf("index #:%i \n", index);
            (command->u.word)[index] = NULL;
            command->output = NULL;
        
            command->input = ptr_to_LESSTHAN_Token->next->m_token.word;
            return command;
        }
    }
    
    //basic redirection w/ only >
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_SEMICOLON_Token) && (!ptr_to_PIPE_Token)&&(!ptr_to_LESSTHAN_Token) && (ptr_to_GREATERTHAN_Token))
    {
        if (ptr_to_GREATERTHAN_Token->previous->m_token.type == RIGHT_PAREN_TOKEN)
        {
            command_t command = checked_malloc(sizeof(struct command));
            command->type = SUBSHELL_COMMAND;
            command->status = -1;
            int subshell_balancing_internal = 1;
            token_node *itr_To_Left_Paren = (ptr_to_GREATERTHAN_Token->previous->previous);
            while (itr_To_Left_Paren!=NULL && subshell_balancing_internal!=0)
            {
                if (itr_To_Left_Paren->m_token.type == RIGHT_PAREN_TOKEN)
                    subshell_balancing_internal++;
                if (itr_To_Left_Paren->m_token.type == LEFT_PAREN_TOKEN)
                    subshell_balancing_internal--;
                if (subshell_balancing_internal!=0)
                    itr_To_Left_Paren=itr_To_Left_Paren->previous;
            }
            command->u.subshell_command=CreateCommand(itr_To_Left_Paren->next, ptr_to_GREATERTHAN_Token->previous->previous);
            command->output = ptr_to_GREATERTHAN_Token->next->m_token.word;
            command->input = NULL;
            return command;
            
        }
        
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SIMPLE_COMMAND;
        command ->status = -1;
        
        command->u.word = (char**)checked_malloc((sizeof(char*))*(numWordsBeforeRedirection+1));
        itr = head;
        int index = 0;
        
        while ((itr != ptr_to_GREATERTHAN_Token))
        {
            if (itr->m_token.type == WORD_TOKEN)
            {
                (command->u.word)[index++] = itr->m_token.word;
                //puts(itr->m_token.word);
                //puts("\n");
            }
            itr = itr->next;
        }
        //printf("index #:%i \n", index);
        (command->u.word)[index] = NULL;
        command ->input = NULL;
        command->output = ptr_to_GREATERTHAN_Token->next->m_token.word;
        return command;
        
    }
    
    //redirection w/ (command) < word > word
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&&(ptr_to_LESSTHAN_Token) && (!ptr_to_SEMICOLON_Token) && (ptr_to_GREATERTHAN_Token))
    {
        if (ptr_to_LESSTHAN_Token->previous->m_token.type == RIGHT_PAREN_TOKEN)
        {
            command_t command = checked_malloc(sizeof(struct command));
            command->type = SUBSHELL_COMMAND;
            command->status = -1;
            int subshell_balancing_internal = 1;
            token_node *itr_To_Left_Paren = (ptr_to_LESSTHAN_Token->previous->previous);
            while (itr_To_Left_Paren!=NULL && subshell_balancing_internal!=0)
            {
                if (itr_To_Left_Paren->m_token.type == RIGHT_PAREN_TOKEN)
                    subshell_balancing_internal++;
                if (itr_To_Left_Paren->m_token.type == LEFT_PAREN_TOKEN)
                    subshell_balancing_internal--;
                if (subshell_balancing_internal!=0)
                    itr_To_Left_Paren=itr_To_Left_Paren->previous;
            }
            command->u.subshell_command=CreateCommand(itr_To_Left_Paren->next, ptr_to_LESSTHAN_Token->previous->previous);
            command->input = ptr_to_LESSTHAN_Token->next->m_token.word;
            command->output = ptr_to_GREATERTHAN_Token->next->m_token.word;
            return command;
            
        }
        
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SIMPLE_COMMAND;
        command ->status = -1;
        
        command->u.word = (char**)checked_malloc((sizeof(char*))*(numWordsBeforeRedirection+1));
        itr = head;
        int index = 0;
        
        while ((itr != ptr_to_LESSTHAN_Token))
        {
            if (itr->m_token.type == WORD_TOKEN)
            {
                (command->u.word)[index++] = itr->m_token.word;
                //puts(itr->m_token.word);
                //puts("\n");
            }
            itr = itr->next;
        }
        //printf("index #:%i \n", index);
        (command->u.word)[index] = NULL;
        command->input = ptr_to_LESSTHAN_Token->next->m_token.word;
        command->output = ptr_to_GREATERTHAN_Token->next->m_token.word;
        return command;
        
    }
    
    //SEMICOLON
    
    if ((ptr_to_SEMICOLON_Token))
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = SEQUENCE_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
        command->u.command[0] = CreateCommand(head, ptr_to_SEMICOLON_Token->previous);
        command->u.command[1] = CreateCommand(ptr_to_SEMICOLON_Token->next,tail);
        return command;
        
    }
    
    //PIPE COMMAND
    
    if ((ptr_to_PIPE_Token) && (!ptr_to_AND_Token) && (!ptr_to_OR_Token) && 
        (!ptr_to_SEMICOLON_Token))
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = PIPE_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
        command->u.command[0] = CreateCommand(head, ptr_to_PIPE_Token->previous);
        command->u.command[1] = CreateCommand(ptr_to_PIPE_Token->next,tail);
        return command;
        
        
    }

    //AND COMMAND
    
    if ((ptr_to_AND_Token) && (!ptr_to_SEMICOLON_Token) && (AND_index_placeholder>OR_index_placeholder))
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = AND_COMMAND;
        command->status=-1;
        command->input = NULL;
        command->output=NULL;
        
        
        
        command->u.command[0] = CreateCommand(head, ptr_to_AND_Token->previous);
        command->u.command[1] = CreateCommand(ptr_to_AND_Token->next,tail);
        return command;
    }
    
    //OR COMMAND
    if ((ptr_to_OR_Token) && (!ptr_to_SEMICOLON_Token) && (OR_index_placeholder>AND_index_placeholder))
    {
        command_t command = checked_malloc(sizeof(struct command));
        command->type = OR_COMMAND;
        command->status=-1;
        command->input =NULL;
        command->output=NULL;
        
        command->u.command[0] = CreateCommand(head, ptr_to_OR_Token->previous);
        command->u.command[1] = CreateCommand(ptr_to_OR_Token->next,tail);
        return command;
    }
    return NULL;
}

top_level_command_t
isSanitized_token_stream (token_node* head)
{
  int max_commands = 50;
  top_level_command_t c;
  c.commands = (top_level_command *) checked_malloc(max_commands*sizeof(top_level_command));
  free_list[TOP_LEVEL_COMMAND] = c.commands;
  c.size = 0;
  
  // stack substitution for now
  int paren_count = 0;
  
  // for error output
  int line = 1;
  
  // for checking pipes, ors, ands have two args
  bool req_args = false;
  
  // for building correct complete commands
  bool top_level = true;
  
  bool redirection = false;

  token_type first = head->m_token.type;
  if (first != WORD_TOKEN && first != NEWLINE_TOKEN
       && first != LEFT_PAREN_TOKEN)
	output_read_error(line, head->m_token);
	
  token_node *it = head;
  
  while (it->m_token.type == NEWLINE_TOKEN)
	it = it->next;
	
  token_node *command_begin = it;
  while (it->next != NULL)
  {
    token next_token = it->next->m_token;
    token_type next_type = next_token.type;
	// printf("Current token type = %d\n", it->m_token.type);
    switch (it->m_token.type)
	{
	  case (WORD_TOKEN):
	  {
	    if (req_args)
		  req_args = false;
		if (it != head && redirection &&
			 (it->previous->m_token.type == GREATER_TOKEN
			 || (it->previous->m_token.type == LESS_TOKEN
			      && next_type != GREATER_TOKEN)))
		  redirection = false;
		  
	    if (next_type == LEFT_PAREN_TOKEN)
		  output_read_error(line, next_token);
		break;
	  }
	  case (PIPE_TOKEN):
	  case (AND_TOKEN):
	  case (OR_TOKEN):
	  {
	    req_args = true;
	  }
	  case (SEMICOLON_TOKEN):
	  {
	    if (next_type != WORD_TOKEN && next_type != LEFT_PAREN_TOKEN
		      && next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
		  
		if (top_level && it->m_token.type == SEMICOLON_TOKEN)
		{
		  top_level_command new;
		  new.head = command_begin;
		  new.tail = it->previous;
		  
		  if (c.size == max_commands)
		  {
		    max_commands *= 2;
			c.commands = (top_level_command *) checked_realloc(c.commands, (max_commands)*sizeof(top_level_command));
		  }
		  
		  c.commands[c.size] = new;
		  c.size++;

		  command_begin = it->next;
		  while (command_begin->m_token.type == NEWLINE_TOKEN)
		    command_begin = command_begin->next;
		}
		break;
	  }
	  case (LEFT_PAREN_TOKEN):
	  {
	    if (req_args)
		  req_args = false;
		  
	    paren_count++;
		top_level = false;
		
		if (next_type != WORD_TOKEN && next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
		break;
	  }
	  case (RIGHT_PAREN_TOKEN):
	  {
	    if (paren_count == 0 || next_type == WORD_TOKEN)
		  output_read_error(line, next_token);
		  
	    paren_count--;
		if (paren_count == 0)
		  top_level = true;
		
		break;
	  }
	  case (LESS_TOKEN):
	  {
	    // greater than followed by word followed by less than is invalid
	    if (redirection || (it->previous != head
		     && it->previous->previous->m_token.type == GREATER_TOKEN))
		  output_read_error(line, next_token);
	  }
	  case (GREATER_TOKEN):
	  {
	    if (next_type != WORD_TOKEN 
			 || (redirection && it->previous != head
			      && it->previous->previous->m_token.type != LESS_TOKEN))
		  output_read_error(line, next_token);
		redirection = true;
		break;
	  }
	  case (NEWLINE_TOKEN):
	  {
	    if (next_type != LEFT_PAREN_TOKEN && next_type != RIGHT_PAREN_TOKEN
		      && next_type != WORD_TOKEN && next_type != COMMENT_TOKEN
			  && next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
		
		token_type prev_type;
		if (it->previous)
          prev_type = it->previous->m_token.type;
		else
		  prev_type = NEWLINE_TOKEN;
		  
		if (top_level && !req_args && (prev_type != SEMICOLON_TOKEN && prev_type != NEWLINE_TOKEN))
		{
		  top_level_command new;
		  new.head = command_begin;
		  new.tail = it->previous;
		  
		  if (c.size == max_commands)
		  {
		    max_commands *= 2;
			c.commands = (top_level_command *) checked_realloc(c.commands, (max_commands)*sizeof(top_level_command));
		  }
		  
		  c.commands[c.size] = new;
		  c.size++;
		   
		  command_begin = it->next;
		  while (command_begin->m_token.type == NEWLINE_TOKEN)
		    command_begin = command_begin->next;
		}
		else if (!top_level && (prev_type == WORD_TOKEN || prev_type == RIGHT_PAREN_TOKEN))
		{
		  itr2 = it->next;
		  while(itr2->m_token.type == NEWLINE_TOKEN)
		    itr2 = itr2->next;
		  
		  if (itr2->m_token.type == WORD_TOKEN || itr2->m_token.type == LEFT_PAREN_TOKEN);
		    it->m_token.type = SEMICOLON_TOKEN;
		}
		
		line++;
		break;
	  }
	  case (COMMENT_TOKEN):
	    if (next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
	    break;
	}
	it = it->next;
  }
  
  // last token
  token_type last_type = it->m_token.type;
  if (last_type != WORD_TOKEN)
  {
	if (last_type != SEMICOLON_TOKEN && last_type != NEWLINE_TOKEN 
			  && last_type != RIGHT_PAREN_TOKEN)
	  output_read_error(line, it->m_token);
	else
	{ 
	  top_level_command new;
	  new.head = command_begin;
	  new.tail = it->previous;
	  
	  if (last_type == RIGHT_PAREN_TOKEN)
	  {
	    new.tail = it;
		paren_count--;
	  }
	
  	  if (c.size == max_commands)
      {
	    max_commands++;
	    c.commands = (top_level_command *) checked_realloc(c.commands, (max_commands)*sizeof(top_level_command));
      }
	
	  c.commands[c.size] = new;
	  c.size++;
	}
  }
  else
  {
    req_args = false;
    top_level_command new;
	new.head = command_begin;
	new.tail = it;
	
	if (c.size == max_commands)
    {
	  max_commands++;
	  c.commands = (top_level_command *) checked_realloc(c.commands, (max_commands)*sizeof(top_level_command));
    }
	
	c.commands[c.size] = new;
	c.size++;
  }
  
  if (paren_count != 0)
    error(1, 0, "Line %d: unmatched parentheses", line);
  
  if (req_args)
    error(1, 0, "Line %d: missing argument", line);
  
  return c;
}

void
remove_newline_tokens(top_level_command c)
{
  token_node *it = c.head;
  while(it != c.tail)
  {
    if (it->m_token.type != NEWLINE_TOKEN)
	{
	  it = it->next;
	  continue;
	}
	
	token_node *r = it;
	it->previous->next = it->next;
	it->next->previous = it->previous;
	it = it->next;
	free(r);
  }
}

void
output_read_error(int line, token node)
{
  char c[80];
  c[0] = '\0';
  
  switch (node.type)
  {
    case (WORD_TOKEN):
	{
	  memcpy(c, node.word, 80);
	  break;
	}
	case (PIPE_TOKEN):
	{
	  strcat(c, "|");
	  break;
	}
	case (OR_TOKEN):
	{
	  strcat(c, "||");
	  break;
	}
	case (AND_TOKEN):
	{
	  strcat(c, "&&");
	  break;
	}
	case (SEMICOLON_TOKEN):
	{
	  strcat(c, ";");
	  break;
	}
	case (LEFT_PAREN_TOKEN):
	{
	  strcat(c, "(");
	  break;
	}
	case (RIGHT_PAREN_TOKEN):
	{
	  strcat(c, ")");
	  break;
	}
	case (LESS_TOKEN):
	{
	  strcat(c, "<");
	  break;
	}
	case (GREATER_TOKEN):
	{
	  strcat(c, ">");
	  break;
	}
	case (NEWLINE_TOKEN):
	{
	  strcat(c, "new line");
	  break;
	}
	case (COMMENT_TOKEN):
	{
	  strcat(c, "#");
	  break;
	}
  }
  error(1, 0, "Line %d: syntax '%s'", line, c);
}

command_t
read_command_stream (command_stream_t s)
{
  tlc_wrapper_t c = *(s->it);
  (s->it)++;
  return c->command;
}
