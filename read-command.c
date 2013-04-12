// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <error.h>
#include <stdio.h>
#include <ctype.h>

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
                error(1,0,"dsd");
            }
            typeOfToken=AND_TOKEN;
            i++;
        }
        
        else if (buffer[i] == '#') //comment
        {
            
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
        else {error(1,0,"dds");}
        
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

        
        
        
        
    
    
    
    
    
    
    
    
    
    
    
    
    
    //=========Changes the tokens into commands============//
    
    return fake;
    
    
    
    
  //error (1, 0, "command reading not yet implemented");
  //return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
