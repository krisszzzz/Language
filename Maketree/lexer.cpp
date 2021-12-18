#include "Eval.h"
#include "lexer.h"
#include <ctype.h>

inline Min(int first, int second)
{
    return (first <= second) ? first : second;
}

int IsEqual(double first, double second)
{
    return (abs(first - second) < ACCURACY) ? 1 : 0;
}

#define KEY_STRNCMP(key_word)                                               \
strncasecmp((*expression), key_word, WordLength(*expression));

// think about array standart

int IsBlockChanged(char** expression)
{
    char* local_expression = *expression;
    IGNORE_NEWLINES_AND_SPACES();
    for(int iter_count = 0; iter_count < BLOCKS_AMOUNT; ++iter_count) {
        if(strncasecmp(local_expression, BLOCKS[iter_count], WordLength(*expression))) {
            return 1;
        }
    }
    return 0;
}

// Rework ReadEq

Tree_node* VarDeclarationProccessing(char** expression)
{

    if(KEY_STRNCMP("ingredients")) {
        *expression += strlen("ingredients");
        IGNORE_NEWLINES_AND_SPACES();
        while(!IsBlockChanged(expression)) {
            CalculateResult(expression);               
        }
        //if(KEY_STRNCMP("tst.") || )
    }
    return nullptr;
}


#define IGNORE_SPACES()                                  \
while(**expression == ' ' || **expression == '\t')       \
    ++*expression;

#define IGNORE_NEWLINES_AND_SPACES()                                             \
while(**expression == '\n' || **expression == ' ' || **expression == '\t')       \
    ++*expression;      

int WordLength(char* expression)
{
    int word_length = 0;
    while(*expression != ' ' || *expression != '\0') {
        ++word_length;
    }
    return word_length;
}

Tree_node* GetFunction(char** expression)
{
    for(int iter_count; iter_count < SIZEOF_FUNCTION_DECLARATION_KEYWORD; ++iter_count)
}

Tree_node* GetKeyWord(char** expression, Tree_node* tokens)
{
    static int call_count = 0;
    call_count++;

    Tree_node main_statement = {};
    FillData(&main_statement, STRING, "statement");
    tokens[0] = main_statement;

    IGNORE_SPACES();

    if(call_count == 1) {
        IGNORE_NEWLINES_AND_SPACES();

        const int kCheckOnlyFirstKeyword = 1; // "recipe" keyword check
        RET_IF(CheckKeywords(FUNCTION_DECLARATION_KEYWORD, *expression, 
                             WordLength(*expression),       kCheckOnlyFirstKeyword)) == -1,
                             EXPECTED_FUNCTION, "Expected a function specified like a recipe in this language\n"
                                                "Please follow the github and read instructions if you have any qustions\n");
    }

    if() {
                
//    }

}

