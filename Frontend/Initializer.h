#ifndef INITIALIZER_INCLUDED
#define INITIALIZER_INCLUDED

#include "../lib/tree.h"
#include "../lib/murmurhash.h"
#include "../lib/textlib.h"
#include <math.h>

// Remove
///

#define WORD_HASH(word)             \
MurmurHash(word, WordLength(word))

#define INCREASE_SIZE_WITH_CHECK(increment_size)                                                                             \
(*tokens_size) = increment_size + (*tokens_size);                                                                            \
if((*tokens_size) == *tokens_capacity) {                                                                                     \
    RET_IF((tokens = (Tree_node*)realloc(tokens, *tokens_capacity * 2)) == nullptr,                                          \
            MEM_ALLOC_ERROR, "Memory allocation error\n");                                                                   \
    *tokens_capacity *= 2;                                                                                                   \
}   


struct hash_type 
{                               // Contaion operator hash
    const char*  string;
    const hash_t hash;
};



enum INITIALIZER_ERRORS 
{
    INITIALIZER_LEXER_ERROR = -0xdead,
    EXPECTED_OPERATOR       = -0xdeda,
    EXPECTED_KEYWORD        = -0xdaaa
};

const int TATEMENT_PROCCESSING    = -0xaddd;
const int NOT_FOUNDED             = - 1;
const int DECLARATION_PROCCESSING = -1000-7;

// Lexer part
double     TransferStringToDouble(char **expression);

int        GetNumber             (char** expression, Tree_node* tokens, int* tokens_size, int*    tokens_capacity,  hash_type* operation_table,
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

int        GetBinaryOperator     (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        GetPow                (char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* operation_table,
                                  int    hash_table_size, int   binary_operation_part);

int        InitializerLexer      (char** expression,  Tree_node* tokens, int* tokens_size, int* tokens_capacity, int terminate_symbol = 0);

int        GetFunction           (char**     expression,      Tree_node* tokens,          int* tokens_size,        int* tokens_capacity,
                                  hash_type* operation_table, int        hash_table_size, int  binary_operation_part);

const char* DefineLogicalOp(char* logical_operation, int word_length);

void       SyntaxError           ();

int        Require               (char **expression, char to_cmp);
// Parser part

Tree_node* InitializerParser           (Tree_node* tokens, int* tokens_size);

Tree_node* TakeBracketNode             (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeConditionNode           (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakePlusMinusNode           (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeVarorNumberNode         (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeDivMulNode              (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeComparisonOperationNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakePowNode                (Tree_node* tokens, int* token_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeAssignmentNode         (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeFunctionNode           (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeLogicalOperationNode   (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

Tree_node* TakeUnaryOperatorNode      (Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part);

int IsOperator(Tree_node* op);

int IsAssignment(Tree_node* token);

int IsComparisonOp(Tree_node* token);

int isLogicalOp(Tree_node* token); 

#endif