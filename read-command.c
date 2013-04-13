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
        //    printf("looped once: %c \n", value);
        //    printf("bufferSize: %d \n", (int) bufferSize);
        (*bufferEndIndex)++;
        if (bufferSize == *((size_t*) bufferEndIndex))
        {
            bufferSize*=2;
            buffer =  (char*)checked_realloc(buffer, bufferSize);
            //printf("called checked_realloc: %d\n", (int) bufferSize);
        }
        //printf ("%c \n", value);
    }
    return buffer;
}

command_stream_t make_command_stream (int (*get_next_byte) (void *),void *get_next_byte_argument)
{
    //READ FROM FILE INTO CHARACTER BUFFER
    int bufferEndIndex=0;
    char* buffer = ReadFileIntoCharacterBuffer(get_next_byte, get_next_byte_argument,&bufferEndIndex); 
    //printf("%i \n", bufferEndIndex); //characters from 0th index to (bufferEndIndex-1)
    //puts(buffer);
    
    //TOKENIZE THE CHARACTER BUFFER AND CONSTRUCT TOKEN_NODE
    
    command_stream_t fake;
    fake.size = 1;
    //ensure file wasn't empty
    if(bufferEndIndex == 0)
    {
        return fake; //REPLACE W/ PROPER CODE
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
                error(1,0,"incorrect AND command");
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
                     error(1,0,"improper comment declaration");
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
		  error(1,0,"Unrecognizable character");
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
    /*
    puts("SEMICOLON_TOKEN: 0 \nNEWLINE_TOKEN: 1 \nAND_TOKEN: 2 \nOR_TOKEN: 3 \nGREATER_TOKEN: 4 \nWORD_TOKEN: 5 \nLEFT_PAREN_TOKEN: 6 \nRIGHT_PAREN_TOKEN: 7 \nLESS_TOKEN: 8 \nCOMMENTS_TOKEN: 9 \nPIPE_TOKEN: 10 \nMISC_TOKEN: 11 \n");*/
    /*
    //puts(tstream->m_token.words);
    token_node* itr = head;
    while (itr != NULL)
    {
        //printf("%d \n", tstream->m_token.type);
        //the above line works to display just the tokens, the below doesn't work because there's a segfault on accessing the token words
        if (itr->m_token.type == WORD_TOKEN)
        {
            printf("%d: ", itr->m_token.type);
            
            puts(itr->m_token.word); //doesn't work if string is very last thing
            //printf("Line number:%i \n ", tstream->m_token.lineNumber);
            //puts("\n");
        }
        else 
            printf("Token %d \n", itr->m_token.type);
        
        
        itr = itr->next;
    }
	
	*/
	//==============Sanitize token stream===========//
	free_list[TOKEN_STREAM] = head;
	top_level_command_t c = isSanitized_token_stream(head);
    puts("Looks good!");
	puts("\n //============== DEBUG TOP LEVEL COMMANDS =================//\n");
    /*typedef enum {
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
    } token_type;*/
    
    for (i = 0; i < c.size; i++)
	{
	  top_level_command t = c.commands[i];
	  remove_newline_tokens(t);
	  token_node *itr = t.head;
	  while(itr != t.tail)
	  {
	    printf("%d ", itr->m_token.type);
		itr = itr->next;
	  }
	  printf("%d\n", itr->m_token.type);
	}
    
    top_level_command t = c.commands[0];

    
    
    //int it = sizeof(command);
    //printf("size: %i", it);
    command_t command = CreateCommand(t.head, t.tail);
    // testing github
    
    //if (command->u.word[3])
      //  puts("NO!");
    //puts(command->u.word[2]);
    
    
    //if (command->u.word[2])
      //  puts("NO!");
    
    //puts((command->u.word)[0]);
    //puts((command->u.word)[1]);
    //puts((command->u.word)[2]);
    //printf("pointer address:%p \n",(command->u.word)[2]);
    //printf("pointer address:%p \n",(command->u.word)[5]);
    //printf("pointer address:%p",(&(command->u.word)[1]));
    //printf("string:%s",((command->u.word)[3]));
    print_command(command);
    //printf("COMMAND TYPE:%i \n", command->type); 
    /*char **w = command->u.word;
    //printf ("%s \n", *w);
	while (*++w)
        printf (" %s \n", *w);
    
    //puts(*(command->u.word));

        
        */
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    //=========Changes the tokens into commands============//
    
    return fake;
    
    
    
    
  //error (1, 0, "command reading not yet implemented");
  //return 0;
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
    int numNewlines=0;
    int numWordNodes=0;
    int numWordsBeforeRedirection = 0;
    int AND_index_placeholder = 0;
    int OR_index_placeholder = 0;
    
    
    int LESSTHAN_index_placeholder=0;
    int GREATERTHAN_index_placeholder=0;
    
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
                LESSTHAN_index_placeholder=totalNodes;
            }
        
        
            if (itr->m_token.type== GREATER_TOKEN)
            {
                ptr_to_GREATERTHAN_Token = itr;
                GREATERTHAN_index_placeholder=totalNodes;
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
        int it = (sizeof(char*))*numWordNodes;
        printf("size: %i \n", it);
        
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
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&&(ptr_to_LESSTHAN_Token) && (!ptr_to_GREATERTHAN_Token))
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
            command->output = NULL;
        
            command->input = ptr_to_LESSTHAN_Token->next->m_token.word;
            return command;
        }
    }
    
    //basic redirection w/ only >
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&&(!ptr_to_LESSTHAN_Token) && (ptr_to_GREATERTHAN_Token))
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
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&&(ptr_to_LESSTHAN_Token) && (ptr_to_GREATERTHAN_Token))
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
    
    
    
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (!ptr_to_PIPE_Token)&& (!ptr_to_LESSTHAN_Token) && (!ptr_to_GREATERTHAN_Token) && (ptr_to_SEMICOLON_Token))
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
    
    if ((!ptr_to_AND_Token) && (!ptr_to_OR_Token) && (ptr_to_PIPE_Token))
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
    
    if ((ptr_to_AND_Token) && (AND_index_placeholder>OR_index_placeholder))
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
    if ((ptr_to_OR_Token) && (OR_index_placeholder>AND_index_placeholder))
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
       && first != AND_TOKEN && first != LEFT_PAREN_TOKEN)
	output_read_error(line, head->m_token);
	
  token_node *it = head;
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
		
		token_type prev_type = it->previous->m_token.type;
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
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
