#include "language_processing.h"
#include "../lib/textlib.h"
#include <ctype.h>

const char* BLOCKS[] = {
                       "ingredients:",
                       "to buy list:",
                       "recipe",
                       "cook time",
                       "serves",
                       "time",
                       "method:", 
                       "salt and papper:"
                       };

const int   BLOCKS_AMOUNT = sizeof(BLOCKS) / sizeof(BLOCKS[0]);  // Not in header because of fucking collect2 errror



inline int Min(int first, int second)
{
    return (first <= second) ? first : second;
}

int IsEqual(double first, double second)
{
    return (abs(first - second) < ACCURACY) ? 1 : 0;
}

// think about array standart

int IsBlockChanged(char** expression)
{
    IGNORE_NEWLINES_AND_SPACES();

    if(**expression == '\0') {
        return 1;
    }
    for(int iter_count = 0; iter_count < BLOCKS_AMOUNT; ++iter_count) {
        printf("TO_COMPARE: %s, CURRENT_LINE: %s\n", *expression, BLOCKS[iter_count]);
        if(KEY_STRNCMP(BLOCKS[iter_count])) {
            return 1;
        }
    }
    return 0;
}

#define KEY_WORD_REQUIRE(keyword, keyword_length)                   \
if(strncasecmp(*expression, keyword, keyword_length) == 0) {        \
    *expression += keyword_length;                                  \
}


int GetVariableDeclarations(char**     expression,  Tree_node* tokens,          Tree_node* root_node, 
                            int*       tokens_size, int*       tokens_capacity, int*       error_code,
                            Tree_node* declaration)
{        
    STANDART_ERROR_PROCCESSING;

    if(KEY_STRNCMP("ingredients:")) {
        *expression += strlen("ingredients:");
        do  
        {
            Tree_node assignment = {};
            FillData(&assignment, STRING, "=");    

            IGNORE_NEWLINES_AND_SPACES();

            int curr_size = *tokens_size;
            InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING);        
            Tree_node* right_value = InitializerParser(tokens, &curr_size);

            fprintf(stderr, "EXPRESSION: %s", *expression);
            getchar();

            KEY_WORD_REQUIRE("ml", strlen("ml"));
            KEY_WORD_REQUIRE("g", strlen("g"));

            IGNORE_NEWLINES_AND_SPACES();

            KEY_WORD_REQUIRE("of", strlen("of"));

            RET_IF(**expression != ' ' && **expression != '\t', EXPECTED_KEYWORD, "Error: expected of\n"
                                                                                  "Beginning of errored fragment: %s", *expression);
        
            curr_size = *tokens_size;
            InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING); 

            Tree_node* variable = InitializerParser(tokens, &curr_size);

            RET_IF(variable == nullptr, LEXER_PARSING_ERROR, "Syntax error while parsing\n");
            IGNORE_NEWLINES_AND_SPACES();

            KEY_WORD_REQUIRE("o_O", strlen("o_O"));

            ConnectNodes(&assignment, variable);
            ConnectNodes(&assignment, right_value);
            ConnectNodes(declaration, &assignment);
        } while (!IsBlockChanged(expression));
    } 

    return 0;
}


int GetMethod(char**     expression,    Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code,
              Tree_node* root_statement)
{
    STANDART_ERROR_PROCCESSING;
    if(KEY_STRNCMP("method:")) {        

        *expression += strlen("method:");
        printf("LINE BEFORE METHOD: %s", *expression);
        IGNORE_NEWLINES_AND_SPACES();
        do 
        {
            if(KEY_STRNCMP("salt and papper:")) {
                Tree_node if_node = {};
                FillData(&if_node, STRING, "if");
                
                GetCondition(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &if_node);   
                ConnectNodes(root_statement, &if_node);
            }

            if(strncmp(*expression, "serves", WordLength("serves")) == 0) {
                GetReturn(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, root_statement);
            }

            int curr_size = *tokens_size;
            IGNORE_NEWLINES_AND_SPACES();
            InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING);

            Tree_node* statement = InitializerParser(tokens, &curr_size);
            ConnectNodes(root_statement, statement);
            IGNORE_NEWLINES_AND_SPACES();

            *expression += strlen("o_O");            
        } while (!IsBlockChanged(expression));

    } else {
        *error_code = EXPECTED_METHOD;
        return EXPECTED_METHOD;
    }
    return 0;
}

int GetCondition(char**     expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code, 
                 Tree_node* if_node)
{
    STANDART_ERROR_PROCCESSING;

    if(KEY_STRNCMP("salt and papper:")) {

        *expression += strlen("salt and papper:");
        int curr_size = *tokens_size;
        IGNORE_NEWLINES_AND_SPACES();

        InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING);
        Tree_node* condition = InitializerParser(tokens, &curr_size);               

        *expression += strlen("o_O");
        IGNORE_NEWLINES_AND_SPACES();

        ConnectNodes(if_node, condition);
        printf("\n\nexpression in get_condition: %s\n\n", *expression);

        if(KEY_STRNCMP("add:")) {
            *expression += strlen("add:");
            IGNORE_NEWLINES_AND_SPACES();
            
            if(strncmp(*expression, "serves", WordLength("serves")) == 0) {        

                Tree_node _return_ = {};
                FillData(&_return_, STRING, "return");

                GetReturn(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &_return_);
                ConnectNodes(if_node, &_return_);
                return 0;
            }
            
            curr_size = *tokens_size;
        
            InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING); // Rework
            Tree_node* if_body = InitializerParser(tokens, &curr_size); 

            ConnectNodes(if_node, if_body);                    
            *expression += strlen("o_O");

        } else {
            ErrorPrint("Expected if body with keyword add\n");
            return EXPECTED_IF_BODY;
        }

    } else {
        ErrorPrint("Expected if condition with salt and papper keyword\n");
        return EXPECTED_IF;
    }

    return 0;       
}

int GetParameter(char**     expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code, 
                 Tree_node* parameter)
{
    STANDART_ERROR_PROCCESSING;
    if(KEY_STRNCMP("to buy list:")) {

        *expression += strlen("to buy list:");

        do {
            int curr_size = *tokens_size;
            IGNORE_NEWLINES_AND_SPACES();
            InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING);
            Tree_node* param = InitializerParser(tokens, &curr_size);

            *expression += strlen("o_O");            

            ConnectNodes(parameter, param);
        } while(!IsBlockChanged(expression));
    } else {
        *error_code = EXPECTED_PARAMETER;
        return EXPECTED_PARAMETER;
    }
    return 0;
}

int GetFunction(char** expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code)
{
    if(KEY_STRNCMP("recipe")) {

        IGNORE_NEWLINES_AND_SPACES();

        Tree_node define = {};
        FillData(&define, STRING, "define");

        char function_name[MAX_FUNCTION_NAME_SIZE] = {};
        int  function_name_length                  = 0;
        int  function_offset                       = 1;

        *expression += strlen("recipe");
        IGNORE_NEWLINES_AND_SPACES();

        RET_IF(sscanf(*expression, "\"%[A-Z_a-z]\"%n", function_name + function_offset, &function_name_length) != 1, EXPECTED_FUNCTION, "Expected function name\n");
        function_name[0] = '#';        

        Tree_node function = {};
        FillData(&function, STRING, "function");

        Tree_node funct_name = {};
        FillData(&funct_name, STRING, function_name);
        
        ConnectNodes(&function, &funct_name);

        const int brackets_count = 2;
        *expression += function_name_length + brackets_count;

        Tree_node param = {};
        FillData(&param, STRING, "parameter");

        IGNORE_NEWLINES_AND_SPACES();
        GetParameter(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &param);
        RET_IF(*error_code == EXPECTED_PARAMETER, EXPECTED_PARAMETER, "After function declaration you should write parametr list "
                                                                      "starting with to buy list: not forget :\n");

        ConnectNodes(&function, &param);        

        Tree_node declaration = {};
        FillData(&declaration, STRING, "declaration");

        GetVariableDeclarations(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &declaration);

        Tree_node statement = {};
        FillData(&statement, STRING, "statement");

        GetMethod(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &statement);

        RET_IF(*error_code == EXPECTED_METHOD, EXPECTED_METHOD, "Expected method: \n");

        Tree_node _return_ = {};
        FillData(&_return_, STRING, "return");
        GetReturn(expression, tokens, root_node, tokens_size, tokens_capacity, error_code, &_return_);

        ConnectNodes(&statement, &_return_);
        ConnectNodes(&define,    &function);
        ConnectNodes(&define,    &declaration);
        ConnectNodes(&define,    &statement);
        ConnectNodes(root_node,  &define);
    }

    TreeGraphicalDump_(root_node, "root_node.txt");
    return 0;
}

int GetReturn(char**     expression, Tree_node* tokens, Tree_node* root_node, int* tokens_size, int* tokens_capacity, int* error_code,
              Tree_node* _return_)
{
    IGNORE_NEWLINES_AND_SPACES();
    printf("EXRPESSION BEFORE RETURN: %s", *expression);

    RET_IF(!KEY_STRNCMP("serves"), EXPECTED_RETURN, "expected return with keyword serves\n");

    *expression += strlen("serves");

    IGNORE_NEWLINES_AND_SPACES();

    int curr_size = *tokens_size;

    InitializerLexer(expression, tokens, tokens_size, tokens_capacity, DECLARATION_PROCCESSING);

    Tree_node* ret_val = InitializerParser(tokens, &curr_size);

    ConnectNodes(_return_, ret_val);
    return 0;
}

