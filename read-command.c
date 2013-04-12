// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include "alloc.h"
#include <error.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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
        else {error(1,0,"Unrecognizable character");}
        
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
    puts("SEMICOLON_TOKEN: 0 \nNEWLINE_TOKEN: 1 \nAND_TOKEN: 2 \nOR_TOKEN: 3 \nGREATER_TOKEN: 4 \nWORD_TOKEN: 5 \nLEFT_PAREN_TOKEN: 6 \nRIGHT_PAREN_TOKEN: 7 \nLESS_TOKEN: 8 \nCOMMENTS_TOKEN: 9 \nPIPE_TOKEN: 10 \nMISC_TOKEN: 11 \n");
    
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
	if (isSanitized_token_stream(head))
        puts("Looks good!");
    
   
        
        
        
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    //=========Changes the tokens into commands============//
    
    return fake;
    
    
    
    
  //error (1, 0, "command reading not yet implemented");
  //return 0;
}


bool isSanitized_token_stream (token_node* head)
{
  // stack substitution for now
  int paren_count = 0;
  
  // for error output
  int line = 1;
  
  bool req_args = false;

  token_type first = head->m_token.type;
  if (first != WORD_TOKEN && first != NEWLINE_TOKEN
       && first != AND_TOKEN && first != LEFT_PAREN_TOKEN)
	output_read_error(line, head->m_token);
	
  token_node* it = head;
  while (it->next != NULL)
  {
    token next_token = it->next->m_token;
    token_type next_type = next_token.type;
	printf("Current token type = %d\n", it->m_token.type);
    switch (it->m_token.type)
	{
	  case (WORD_TOKEN):
	  {
	    if (req_args)
		  req_args = false;
		  
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
		break;
	  }
	  case (LEFT_PAREN_TOKEN):
	  {
	    if (req_args)
		  req_args = false;
		  
	    paren_count++;
		if (next_type != WORD_TOKEN && next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
		break;
	  }
	  case (RIGHT_PAREN_TOKEN):
	  {
	    if (paren_count == 0 || next_type == WORD_TOKEN)
		  output_read_error(line, next_token);
	    paren_count--;
		break;
	  }
	  case (LESS_TOKEN):
	  {
	    // greater than followed by word followed by less than is invalid
	    if (it->previous != head &&
		     it->previous->previous->m_token.type == GREATER_TOKEN)
		  output_read_error(line, next_token);
	  }
	  case (GREATER_TOKEN):
	  {
	    if (next_type != WORD_TOKEN)
		  output_read_error(line, next_token);
		break;
	  }
	  case (NEWLINE_TOKEN):
	  {
	    if (next_type != LEFT_PAREN_TOKEN && next_type != RIGHT_PAREN_TOKEN
		      && next_type != WORD_TOKEN && next_type != COMMENT_TOKEN
			  && next_type != NEWLINE_TOKEN)
		  output_read_error(line, next_token);
		
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
    if (last_type == RIGHT_PAREN_TOKEN)
	{
	  if (paren_count == 0)
	    output_read_error(line, it->m_token);
	}
	else if (last_type != SEMICOLON_TOKEN && last_type != NEWLINE_TOKEN)
	  output_read_error(line, it->m_token);
  }
  else
  {
    req_args = false;
  }
  
  if (paren_count != 0)
    error(1, 0, "Line %d: mismatched parentheses", line);
  
  if (req_args)
    error(1, 0, "Line %d: missing argument", line);
  
  return true;
}

void
output_read_error(int line, token node)
{
  char *c = (char *) checked_malloc(80*sizeof(char));
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
