#include "../lib/tree.h"
#include "../lib/textlib.h"
#include "../lib/log.h"
#include "../lib/murmurhash.h"
#include <math.h>

// Remove

#define KEY_STRNCMP(key_word)                                           \
strncasecmp((*expression), key_word, WordLength(*expression)) == 0                                                          

///


#define INCREASE_SIZE_WITH_CHECK(increment_size)                                                                             \
(*tokens_size) = increment_size + (*tokens_size);                                                                            \
if((*tokens_size) == *tokens_capacity) {                                                                                     \
    RET_IF((tokens = (Tree_node*)realloc(tokens, *tokens_capacity * 2)) == nullptr,                                          \
            MEM_ALLOC_ERROR, "Memory allocation error\n");                                                                   \
    *tokens_capacity *= 2;                                                                                                   \
}   


struct hash_type {
    const char*  string;
    const hash_t hash;
};

const int  INITIALIZER_LEXER_ERROR = -0xdead;
const int  EXPECTED_OPERATOR       = -0xdeda;
const int  STATEMENT_PROCCESSING   = -0xaddd;

// Lexer part
double     TransferStringToDouble(char **expression);

int        GetNumber             (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetDivMul             (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetPlusMinus          (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetBracket            (char** expression, Tree_node* tokens, int* tokens_size, int* itokens_capacity, hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetConstant           (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetCmpOperator        (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetLogicalOp          (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity,  hash_type* operation_table, 
                                  int    hash_table_size, int   binary_operation_part);

int        GetBinaryOperator           (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetPow                (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        InitializerLexer      (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, int terminate_symbol = 0);

int        GetFunction           (char**     expression, Tree_node* tokens,          int* tokens_size,        int* tokens_capacity,
                                  hash_type* operation_table, int        hash_table_size, int  binary_operation_part);

const char* DefineLogicalOp(char* logical_operation, int word_length);

void       SyntaxError           ();

int        Require               (char **expression, char to_cmp);
// Parser part

