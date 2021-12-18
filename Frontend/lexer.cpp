#include "ReadEq.h"
#include "lexer.h"
#include <ctype.h>


char* MakeBufferForFile(FILE* source, int* buffer_size)
{
    fseek(source, 0, SEEK_END);
	int file_size = ftell(source);
	fseek(source, 0, SEEK_SET);

	char* buffer = (char*)calloc(file_size + 1, sizeof(char));
    *buffer_size = file_size;
	return buffer;
}

inline int Min(int first, int second)
{
    return (first <= second) ? first : second;
}

int IsEqual(double first, double second)
{
    return (abs(first - second) < ACCURACY) ? 1 : 0;
}

#define KEY_STRNCMP(key_word)                                               \
strncasecmp((*expression), key_word, WordLength(*expression)) == 0

// think about array standart

int IsBlockChanged(char** expression)
{
    IGNORE_NEWLINES_AND_SPACES();
    fprintf(stderr, "BLOCK_AMOUNT: %d", BLOCKS_AMOUNT);
    for(int iter_count = 0; iter_count < BLOCKS_AMOUNT; ++iter_count) {
        
            fprintf(stderr, "\nEXPR: %s\n", *expression);
        if(KEY_STRNCMP(BLOCKS[iter_count])) {
            return 1;
        }
    }
    fprintf(stderr, "SUPEROK");
    return 0;
}

#define KEY_WORD_REQUIRE(keyword, keyword_length)               \
if(strncasecmp(*expression, keyword, keyword_length) == 0) {    \
    *expression += keyword_length;                              \
}

// int SyntaxRequirementForVarDeclaration(char** expression)
// {
//     KEY_WORD_REQUIRE("ml", 2);
//     KEY_WORD_REQUIRE("")
// }

// Rework ReadEq

Tree_node* VarDeclarationLexer(char** expression, Tree_node* tokens, int* tokens_size, int tokens_capacity)
{
    const char assign      = '=';
    const int  prev_size   = *tokens_size;
    int        curr_size   = *tokens_size;
    Tree_node* statement   = &tokens[tokens_size++];
    int        expr_offset = 0;

    IGNORE_NEWLINES_AND_SPACES();
    fprintf(stderr, "%s", *expression);
    if(KEY_STRNCMP("ingredients")) {
        *expression += strlen("ingredients");
        IGNORE_NEWLINES_AND_SPACES();

        FillData(statement, STRING, "statement");
        char var_name[MAX_VAR_LENGTH] = {};

        while(!IsBlockChanged(expression)) {

            expr_offset            = 0;
            Tree_node* assignment  = &tokens[curr_size++];
            Tree_node* initializer = &tokens[curr_size++];

            fprintf(stderr, "token_size = %d", curr_size);

            FillData(assignment,  OPERATOR, &assign);            
            FillData(initializer, STRING,   "initializer");

            ConnectNodes(statement, assignment);


            Tree_node* expr_node   = nullptr;
            RET_IF((expr_node = Expr_Parser(expression,  &tokens[curr_size], &expr_offset, tokens_capacity)) == nullptr, 
                                            EXPECTED_EXPRESSION,      "Expected a expression for variable initialization\n"
                                                                      "Beginning of errored fragment: %s\n", *expression);   //Rework
            TreeGraphicalDump_(expr_node, "expr_node.txt");                                                                                         
            curr_size += expr_offset;

            IGNORE_SPACES();

            fprintf(stderr, "EXPR: %s\n", *expression);
            KEY_WORD_REQUIRE("ml",   WordLength(*expression));
            KEY_WORD_REQUIRE("g",    WordLength(*expression));
            KEY_WORD_REQUIRE("tst.", WordLength(*expression));
            KEY_WORD_REQUIRE("p.",   WordLength(*expression));         // Rework for arrays


            RET_IF(**expression != ' ' && **expression != '\t', EXPECTED_KEYWORD,  "Error: expected such keyword like ml, g, tst., p.\n"
                                                                                   "Beginning of errored fragment: %s", *expression);
            IGNORE_SPACES();

            KEY_WORD_REQUIRE("of", WordLength(*expression));

            RET_IF(**expression != ' ' && **expression != '\t', EXPECTED_KEYWORD, "Error: expected of\n"
                                                                                  "Beginning of errored fragment: %s", *expression);
            IGNORE_SPACES();
        
            RET_IF(sscanf(*expression, "%s", var_name) != 1, EXPECTED_VAR_NAME,  "Error: expected name of variable\n"
                                                                                 "Beginning of errored fragment: %s", *expression);
            *expression += strlen(var_name);

            FillData(tokens[curr_size], STRING, var_name);            
            ConnectNodes(assignment, &tokens[curr_size++]);
            ConnectNodes(assignment, initializer);
        }

    }

    *tokens_size = curr_size;

    return statement;
}

Tree_node* VarDeclarationParser(Tree_node* tokens, int start_indx, int end_indx)
{   
    RET_IF(tokens == nullptr,     nullptr, "Nullptr tokens - something went wrong\n");
    RET_IF(start_indx > end_indx, nullptr, "Inccorect index, the start_indx should be less than end_indx\n");
    if(tokens[start_indx].type_id == STRING) {
        RET_IF(strcasecmp(tokens[start_indx].data.type_string, "statement") != 0, nullptr, "The node should be statement\n");
    } else {
        ErrorPrint("The token inccorect it should be a string type\n");
        return nullptr;
    }
    
    const int kMinTokenAmount = 4;

    Tree_node* statement = &tokens[start_indx++];

    while(start_indx <= end_indx - kToken_To_Build) {
        Tree_node* assignment  = &tokens[start_indx++];
        Tree_node* initializer = &tokens[start_indx++];
        Tree_node* var         = &tokens[start_indx++];

        ConnectNodes(statement, assignment);
        ConnectNodes(assignment, var);
        ConnectNodes(assignment, initializer);
    }
    return statement;
}


int WordLength(char* expression)
{
    int word_length = 0;
    while(!isspace(*expression) && *expression != '\0') {
    fprintf(stderr, "RET_VAL = %d, expression = %c\n",word_length, *expression);
        expression++;
        ++word_length;
    }
    return word_length;
}

// Tree_node* GetFunction(char** expression, Tree_node* tokens)
// {
//     for(int iter_count; iter_count < SIZEOF_FUNCTION_DECLARATION_KEYWORD; ++iter_count)
// }

// Tree_node* GetKeyWord(char** expression, Tree_node* tokens, int tokens_size, int token_capacity)
// {
//     static int call_count = 0;
//     call_count++;

//     Tree_node main_statement = {};
//     FillData(&main_statement, STRING, "statement");
//     tokens[0] = main_statement;

//     IGNORE_SPACES();

//     if(call_count == 1) {
//         IGNORE_NEWLINES_AND_SPACES();

//         const int kCheckOnlyFirstKeyword = 1; // "recipe" keyword check
//         RET_IF(CheckKeywords(FUNCTION_DECLARATION_KEYWORD, *expression, 
//                              WordLength(*expression),       kCheckOnlyFirstKeyword)) == -1,
//                              EXPECTED_FUNCTION, "Expected a function specified like a recipe in this language\n"
//                                                 "Please follow the github and read instructions if you have any qustions\n");
//     }

// //     if() {
                
// // //    }

// }

int main()
{
    Tree_node tokens[512] = {};

    FILE* source      = fopen("source.txt", "r");
    char* buffer      = nullptr;
    int   buffer_size = 0;

    buffer = MakeBufferForFile(source, &buffer_size);
    fread(buffer, sizeof(char), buffer_size, source);
    buffer[buffer_size] = '\0';

    int token_size = 0;
    Tree_node* ptr = VarDeclarationLexer(&buffer, tokens, &token_size, 512);
    //fprintf(stderr, "OK, token_size = %d");

    for(int iter_count = 0; iter_count < 20; ++iter_count)
    {
        fprintf(stderr, "Token - %d: ", iter_count + 1);
        FprintNodeData(&tokens[iter_count], stderr);
        fprintf(stderr, "\n");
    }
    //Tree_node* ptr = VarDeclarationParser(tokens, 0, token_size);
    //TreeGraphicalDump_(ptr, "test.txt");

            

}
