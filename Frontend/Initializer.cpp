#include "initializer.h"
#include "differentiator.h"
#include <math.h>

const int MAX_VAR_LENGTH = 128;   ///TODO CHANGE to EXTERN

int Search_Binary (hash_type array[], int left, int right, hash_t key)
{
    int midd = 0;
    while (true)
    {
        midd = (left + right) / 2;
 
       if (key < array[midd].hash)       
          right = midd - 1;      
       else if (key > array[midd].hash) 
          left = midd + 1;    
       else                   
           return midd;       
 
       if (left > right)
            return -1;
    }
}

inline int Max(const void* first, const void* second)
{
    hash_type num_first  = *(hash_type*)first;
    hash_type num_second = *(hash_type*)second;
    if(num_first.hash > num_second.hash) {
        return 1;
    } else if(num_first.hash == num_second.hash) {
        return 0;
    } else {
        return -1;
    }

}

double TransferStringToDouble(char **expression)  //Rework
{
    double value = 0;
    while('0' <= **expression && **expression <= '9') 
    {
        value = value * 10 + (**expression - '0');
        ++*expression;
    }
    double frac_part = 0;
    if(**expression == ',' || **expression == '.') {
        ++*expression;
        for(int iter_count = 1; '0' <= **expression && **expression <= '9'; ++iter_count) 
        {
            frac_part += (**expression - '0') / pow(10, iter_count);   
            ++*expression;
        }
    }
    return value + frac_part;

}

int GetVar(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();
    char buffer[MAX_VAR_LENGTH] = {};
    int  var_length             = 0; 

    if(**expression == '\0') {
        return 0;
    }
    
    RET_IF(sscanf(*expression, "%[A-z] %n", buffer, &var_length) != 1, INITIALIZER_LEXER_ERROR, "syntax error, cannot read variable or number\n"
                                                                                                "Beggining of error segment: %s", *expression);

    Tree_node variable = {};
    if(Search_Binary(hash_table, 0, sizeof(hash_table) / sizeof(hash_table[0]), MurmurHash(buffer, strlen(buffer))) == - 1) {     

        *expression += var_length;                                                                                            
        FillData(&variable, STRING, buffer);
        tokens[*tokens_size] = variable;

        INCREASE_SIZE_WITH_CHECK(1);
    }
    IGNORE_SPACES();
    return 0;    
}

int GetNumber(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();
    const char expr_begin = **expression;

    double value          = TransferStringToDouble(expression);
    if(**expression == expr_begin) {        

        double constant = GetConstant(expression, tokens, tokens_size, tokens_capacity, hash_table);    
        IGNORE_SPACES();

        if(constant == 0) {
            RET_IF(GetVar(expression, tokens, tokens_size, tokens_capacity, hash_table) != 0, INITIALIZER_LEXER_ERROR,
                  "Error in GetVar() function\n");
        } else {
            FillData(&tokens[*tokens_size], DOUBLE, &constant);
            INCREASE_SIZE_WITH_CHECK(1);
        }

        return 0;
    }
    IGNORE_SPACES();

    FillData(&tokens[*tokens_size], DOUBLE, &value);

    INCREASE_SIZE_WITH_CHECK(1);
    return 0;
}

int GetDivMul(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();
    GetFunction(expression, tokens, tokens_size, tokens_capacity, hash_table);

    while(**expression == '*' || ** expression == '/')
    {
        char operation = **expression;
        ++*expression;

        FillData(&tokens[*tokens_size], OPERATOR, &operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();

        GetFunction(expression, tokens, tokens_size, tokens_capacity, hash_table);
    } 
    return 0;
}

int GetPlusMinus(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();

    GetDivMul(expression, tokens, tokens_size, tokens_capacity, hash_table);
    

    while(**expression == '+' || **expression == '-')
    {
        char operation = **expression;
        ++*expression;
 
        FillData(&tokens[*tokens_size], OPERATOR, &operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();        
        GetDivMul(expression, tokens, tokens_size, tokens_capacity, hash_table);
    }
    return 0;
}

int GetFunction(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();

    GetBracket(expression, tokens, tokens_size, tokens_capacity, hash_table);
    if(**expression != '\0') {
        int find_indx = 0;
        while((find_indx = Search_Binary(hash_table, 0, sizeof(hash_table) / sizeof(hash_table[0]), MurmurHash(*expression, WordLength(*expression)))) != -1)
        {
            const char operation = '!';

            FillData(&tokens[*tokens_size], OPERATOR, &operation);

            INCREASE_SIZE_WITH_CHECK(1);

            IGNORE_SPACES();

            *expression += strlen(hash_table[find_indx].string);

            IGNORE_SPACES();
            GetBracket(expression, tokens, tokens_size, tokens_capacity, hash_table);

            if(**expression == '\0') {
                break;
            }
        }
    }

    return 0;
}


// int GetMathFunction(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
// {
//     IGNORE_SPACES();
//     GetBracket(expression, tokens, tokens_size, tokens_capacity, hash_table);
//     if(**expression != '\0') {
//         int is_keyword = 0;
//         printf("EXPR: %s\n", *expression);
//         while((is_keyword = Search_Binary(hash_table, 0, sizeof(hash_table) / sizeof(hash_table[0]), MurmurHash(*expression, WordLength(*expression)))) != -1) {
            
//                 printf("OK");
//                 FillData(&tokens[*tokens_size], STRING, hash_table[is_keyword].string);
//                 INCREASE_SIZE_WITH_CHECK(1);
//                 IGNORE_SPACES();
//                 *expression += strlen(hash_table[is_keyword].string);
//                 printf("\n%s\n", *expression);
//                 IGNORE_SPACES();
//                 GetBracket(expression, tokens, tokens_size, tokens_capacity, hash_table);

//                 if(**expression == '\0') {
//                     return 0;
//                 }
            
        
//         }
//     }
//     return 0;
//}

// int GetLogicalOp(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity)
// {

// }

// int GetCmpOperator(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity)
// {
//     IGNORE_SPACES();
//     GetPlusMinus(expression, tokens, tokens_size, tokens_capacity);
    
//     if(**expression != 0) {
//         if(KEY_STRNCMP("heat") || KEY_STRNCMP("chill")) {
//                 char operation = 0;
//             if(KEY_STRNCMP("chill")) {
//                 operation = '>';
//                 *expression += strlen("chill");
//             } else {
//                 operation= '<';
//                 *expression += strlen("heat");

//             }

//             FillData(&tokens[*tokens_size], OPERATOR, &operation);        
//             INCREASE_SIZE_WITH_CHECK(1);                        
//             GetPlusMinus(expression, tokens, tokens_size, tokens_capacity);
//         } else {
//             ErrorPrint("Expected operator such > or < \n");
//             return EXPECTED_OPERATOR;
//         }
//     }
//     return 0;
// }

int GetBracket(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();
    if(**expression == '(') {

        char bracket = **expression;
        ++*expression;

        FillData(&tokens[*tokens_size], CHAR, &bracket);
        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();

        GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, hash_table);

        RET_IF(!Require(expression, ')'), INITIALIZER_LEXER_ERROR, "Expected ) brackets\n");

        bracket = **expression;

        FillData(&tokens[*tokens_size], CHAR, &bracket);
        INCREASE_SIZE_WITH_CHECK(1);

        ++*expression;

        IGNORE_SPACES();

        return 0;
    }

    IGNORE_SPACES();
    return GetNumber(expression, tokens, tokens_size, tokens_capacity, hash_table);
}

int GetConstant(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, hash_type* hash_table)
{
    IGNORE_SPACES();
    if(strncmp(*expression, "pi", 2) == 0) {   
        (*expression) += 2;
        const double pi = 3.1416;
        FillData(&tokens[*tokens_size], DOUBLE, &pi);
        INCREASE_SIZE_WITH_CHECK(1);
        return 1;
    }
    if(strncmp(*expression, "e", 1) == 0) {
        const double e = 2.171828;
        FillData(&tokens[*tokens_size], DOUBLE, &e);
        INCREASE_SIZE_WITH_CHECK(1);
        return 1;
    }
    return 0;
}

int IsInitializerEnd(char** expression)
{
    if(**expression == '~') {
        return 1;
    }
    return 0;
}

#define DEF_KEY_WORD(keyword)                          \
{#keyword, MurmurHash(#keyword, strlen(#keyword))}

int InitializerLexer(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, int terminate_symbol)
{
 static hash_type hash_table[] {
                            DEF_KEY_WORD(or),
                            DEF_KEY_WORD(and),
                            DEF_KEY_WORD(pi),
                            DEF_KEY_WORD(e),
                            DEF_KEY_WORD(not),
                            DEF_KEY_WORD(is),
                            DEF_KEY_WORD(log),
                            DEF_KEY_WORD(ln),
                            DEF_KEY_WORD(exp),
                            DEF_KEY_WORD(sin),
                            DEF_KEY_WORD(cos),
                            DEF_KEY_WORD(arcsin),
                            DEF_KEY_WORD(arccos)
                        };

    qsort(hash_table, sizeof(hash_table) / sizeof(hash_table[0]), sizeof(hash_type),  Max);

    IGNORE_SPACES();
    GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, hash_table);

    IGNORE_SPACES();
    if(terminate_symbol == 0) {       
        RET_IF(!IsInitializerEnd(expression), INITIALIZER_LEXER_ERROR, "Expected terminated whitespace\n");        
        return 0;
    }

    RET_IF(**expression != terminate_symbol, INITIALIZER_LEXER_ERROR, "Expected terminated symbol - %c", terminate_symbol);
    
    return 0;
}


int Require(char **expression, char to_cmp)
{
    return (**expression == to_cmp) ? 1 : 0;
}


/// Parser part


// Tree_node* TakeCmpOperatorNode(Tree_node* tokens, int* tokens_size)
// {
//     Tree_node* to_return = TakePlusMinusNode(tokens, tokens_size);
//             TreeGraphicalDump_(to_return, "to_return.txt");
//     if(tokens[*tokens_size].is_filled) {
//         if(tokens[*tokens_size].type_id == CHAR && (tokens[*tokens_size].data.type_char == '>'
//                                                   || tokens[*tokens_size].data.type_char == '<')) {
//             int current = *tokens_size;
//             ConnectNodes(&tokens[current], to_return);

//             (*tokens_size) += 1;

//             ConnectNodes(&tokens[current], TakePlusMinusNode(tokens, tokens_size));
//             to_return = &tokens[current];
//         }
//     }    
//     return to_return;
// }

Tree_node* TakeVarOrNumberNode(Tree_node* tokens, int* tokens_size)
{
    if(tokens[*tokens_size].type_id == DOUBLE || tokens[*tokens_size].type_id == STRING) {
        int temp       = *tokens_size;
        (*tokens_size) += 1;
        return &tokens[temp];
    }
    return &tokens[*tokens_size];
    // return nullptr;
}

Tree_node* TakeNegationNode(Tree_node* tokens, int* tokens_size)
{
    if(tokens[*tokens_size].type_id == CHAR) {
        if(tokens[*tokens_size].data.type_char == '!') {
            Tree_node* curr_op = &tokens[*tokens_size];
            (*tokens_size) += 1;
            Tree_node* condition = TakeBracketNode(tokens, tokens_size);             
            ConnectNodes(curr_op, condition);
            return curr_op;
        }
    }
    return TakeBracketNode(tokens, tokens_size);
}

Tree_node* TakeBracketNode(Tree_node* tokens, int* tokens_size)
{
    if(tokens[*tokens_size].is_filled && tokens[*tokens_size].type_id == CHAR) {
        if(tokens[*tokens_size].data.type_char == '(') {
            (*tokens_size) += 1;
            Tree_node* expr = TakePlusMinusNode(tokens, tokens_size);            
            (*tokens_size) += 1;
            return expr;
        }
    }

    return TakeVarOrNumberNode(tokens, tokens_size);
    
}
                                    /// token size is begining of block
Tree_node* TakePlusMinusNode(Tree_node* tokens, int* tokens_size)
{
    int is_changed_node    = 0;
    Tree_node* first_expr  = TakeDivMulNode(tokens, tokens_size);
    Tree_node* curr_op     = &tokens[*tokens_size];

    if(IsOperator(&tokens[*tokens_size])) {
        while(tokens[*tokens_size].data.type_char == '+' || tokens[*tokens_size].data.type_char == '-')
        {
            ConnectNodes(curr_op, first_expr);

            if(curr_op->data.type_char != tokens[*tokens_size].data.type_char) {
                ConnectNodes(&tokens[*tokens_size], curr_op);
                is_changed_node = 1;
            }
            
            if(is_changed_node) {
                curr_op = &tokens[*tokens_size];                
                is_changed_node = 0;
            }

            (*tokens_size)++;
            first_expr = TakeDivMulNode(tokens, tokens_size);
        }
    }

    if(curr_op->is_filled == 0) {
        return first_expr;
    }

    if(curr_op->data.type_char != ')') {
        ConnectNodes(curr_op, first_expr);
    }

    return (curr_op->data.type_char != ')') ? curr_op : first_expr;

}

int IsOperator(Tree_node* op)
{
    if(op->type_id == OPERATOR) {
        if(op->data.type_char == '+' || op->data.type_char == '*' || op->data.type_char == '-' || op->data.type_char == '/') {
            return 1;
        }
    }
    return 0;
}

Tree_node* TakeDivMulNode(Tree_node* tokens, int* tokens_size)
{
    int is_changed_node     = 0;
    int is_mul_or_div       = 0;
    Tree_node* first_number = TakeNegationNode(tokens, tokens_size);
    TreeGraphicalDump_(first_number, "text.txt");
    Tree_node* curr_op      = &tokens[*tokens_size];

    if(IsOperator(&tokens[*tokens_size])) {
        while(tokens[*tokens_size].data.type_char == '/' || tokens[*tokens_size].data.type_char == '*')
        {
            is_mul_or_div = 1;
            ConnectNodes(curr_op, first_number);

            if((curr_op->data.type_char != tokens[*tokens_size].data.type_char) && IsOperator(&tokens[*tokens_size])) {
                ConnectNodes(&tokens[*tokens_size], curr_op);
                is_changed_node = 1;
            }
            
            if(is_changed_node) {
                curr_op = &tokens[*tokens_size];                
                is_changed_node = 0;
            }

            (*tokens_size)++;

            first_number = TakeNegationNode(tokens, tokens_size);
        }
    }

    if(is_mul_or_div == 1) { 
        ConnectNodes(curr_op, first_number);
    }
    return (is_mul_or_div) ? curr_op : first_number;
}

Tree_node* InitializerParser(Tree_node* tokens, int* tokens_size)
{
    RET_IF(tokens      == nullptr, nullptr,  "Something went wrong, tokens is nullptr\n");
    RET_IF(tokens_size == nullptr, nullptr,  "Cannot work with token size because it nullptr\n");
    
    return TakePlusMinusNode(tokens, tokens_size);
}

int main()
{
    char* expression = (char*)calloc(512, sizeof(char));
    Tree_node tokens[512] = {};

    InputString(expression, 512);
    
    int tokens_size = 0;
    int tokens_capacity = 512;
    InitializerLexer(&expression, tokens, &tokens_size, &tokens_capacity);


    for(int iter_count = 0; iter_count < 30; ++iter_count)
    {
        fprintf(stderr, "token[%d] = ", iter_count);
        FprintNodeData(&tokens[iter_count], stderr);
        fprintf(stderr, "\n");
    }

    tokens_size     = 0;
    Tree_node* ptr = InitializerParser(tokens, &tokens_size);


    TreeGraphicalDump_(ptr, "test.txt");

}


