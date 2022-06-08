#if !defined LEXER_INCLUDED
#define LEXER_INCLUDED

#include "../lib/tree.h"

#include "Initializer.h"

#define KEY_STRNCMP(key_word)                                           \
strncasecmp((*expression), key_word, strlen(key_word)) == 0                                                          

#define STANDART_ERROR_PROCCESSING                                                                                           \
ERROR_IF  (error_code == nullptr, NULLPTR_ERROR_CODE,    "Cannot save error code, because the error code is nullptr\n"       \
                                                        "The program is terminated\n");                                      \
ERROR_IF(*error_code != 0,       VAR_DECLARATION_ERROR,  "Error in variable declarations\n");                                \
ERROR_IF(tokens      == nullptr, NULLPTR_TOKENS,         "Nullptr tokens, something went wrong\n");                          \
ERROR_IF(tokens_size == nullptr, NULLPTR_TOKEN_SIZE,     "Cannot save token size, because it's nullptr\n"                    \
                                                         "The program is terminated\n");


enum SYNTAX_ERRORS
{
    EXPECTED_FUNCTION = -0xdaaaa,
    EXPECTED_EXPRESSION,
    EXPECTED_PARAMETER,
    EXPECTED_IF,
    EXPECTED_IF_BODY,
    EXPECTED_METHOD,
    EXPECTED_RETURN,
    EXPECTED_DECLARATION,
    EXPECTED_VAR_NAME,
    VAR_DECLARATION_ERROR,
    EXPECTED_LINE_ENDING
};

enum LEXER_ERRORS
{
    INCCORECT_BUILT_NODE = -0x666666,
    LEXER_READING_ERROR,
    NULLPTR_TOKENS,
    LEXER_PARSING_ERROR,
    EXPECTED_INITIALIZER,
};

enum GLOBAL_ERRORS
{
  NULLPTR_ERROR_CODE = -0x1000-7,
  NULLPTR_TOKEN_SIZE
};


const int   MAX_VAR_LENGTH         = 128;
const int   ACCURACY               = 1e-6;
const int   MAX_FUNCTION_NAME_SIZE = 128;
        


char*      MakeBufferForFile(FILE* source, int* buffer_size);


int GetVariableDeclarations(char**     expression,  Tree_node* tokens,          Tree_node* root_node, 
                            int*       tokens_size, int*       tokens_capacity, int*       error_code,
                            Tree_node* declaration);
                            
int GetBlock(char** expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code);

int GetFunction(char** expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code);

int GetMethod(char**     expression,    Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code,
              Tree_node* root_statement);

int GetCondition(char**     expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code, 
                 Tree_node* if_node);

int GetReturn(char**     expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code,
          Tree_node* _return_);

inline int Min(int first, int second);
int        IsEqual(double first, double second);
int        IsBlockChanged(char** expression);


#endif


