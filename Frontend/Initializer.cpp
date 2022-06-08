// TODO NULL is bad because is can be readed like a variable

#include "Initializer.h"
//#include "differentiator.h"
#include <math.h>
#include "../lib/log.h"

#define INIT_HASH_TABLE                                                 \
static hash_type operation_table[] {                                    \
                            DEF_KEY_WORD(or),                           \
                            DEF_KEY_WORD(and),                          \
                            DEF_KEY_WORD(heat),                         \
                            DEF_KEY_WORD(chill),                        \
                            DEF_KEY_WORD(is),                           \
                            DEF_KEY_WORD(not),                          \
                            DEF_KEY_WORD(log),                          \
                            DEF_KEY_WORD(ln),                           \
                            DEF_KEY_WORD(diff),                         \
                            DEF_KEY_WORD(exp),                          \
                            DEF_KEY_WORD(sin),                          \
                            DEF_KEY_WORD(cos),                          \
                            DEF_KEY_WORD(arcsin),                       \
                            DEF_KEY_WORD(arccos)                        \
                        };                                              \

//const int hash_table_size = sizeof(operation_table) / sizeof(operation_table[0]); 
const int hash_table_size = 14;

const int MAX_VAR_LENGTH = 128;   ///TODO CHANGE to EXTERN

/// Additional Functions

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

int SimpleHashTableSearch(hash_type array[], size_t size, hash_t key)
{
    for(size_t iter_count = 0; iter_count < size; ++iter_count)
    {
        if(array[iter_count].hash == key) {
            return iter_count;
        }
    }
    return -1;
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
    double value     = 0;
    double frac_part = 0;
    int    is_number = 0;
    while('0' <= **expression && **expression <= '9') 
    {
        value = value * 10 + (**expression - '0');
        ++*expression;
        is_number = 1;
    }
    if(is_number && (**expression == ',' || **expression == '.')) {
        ++*expression;
        for(int iter_count = 1; '0' <= **expression && **expression <= '9'; ++iter_count) 
        {
            frac_part += (**expression - '0') / pow(10, iter_count);   
            ++*expression;
        }
    }
    return value + frac_part;

}

int IsInitializerEnd(char** expression)
{
    if(**expression == '~') {
        return 1;
    }
    return 0;
}

int IsOperator(Tree_node* op)
{
    if(op->type_id == OPERATOR) {
        if(op->data.type_char == '+' || op->data.type_char == '*' || op->data.type_char == '-' 
        || op->data.type_char == '/' || op->data.type_char == '^') {
            return 1;
        }
    }
    return 0;
}

int IsAssignment(Tree_node* token)
{
    if(token->type_id == STRING) {
        if(*token->data.type_string == '=') {
            return 1;
        }
    }
    return 0;
}

int IsComparisonOp(Tree_node* token)
{
    if(token->type_id == STRING) {
        if(*token->data.type_string == '>' || *token->data.type_string == '<') {
            return 1;
        }
    }
    return 0;
}

int isLogicalOp(Tree_node* token) 
{
    if(token->type_id == STRING) {
        if(strcmp(token->data.type_string, "||") == 0 || strcmp(token->data.type_string, "&&") == 0) {
            return 1;            
        }
    }
    return 0;
}


/// Lexer Part

#define CURRENT_TOKEN       \
tokens[*tokens_size]


int GetVar(char** expression,       Tree_node* tokens,                int* tokens_size, int* tokens_capacity, hash_type* operation_table,
           int    hash_table_size,  int        binary_operation_part)
{
    static int name_table_size = 0;

    IGNORE_SPACES();

    char buffer[MAX_VAR_LENGTH] = {};
    int  var_length             = 0; 
    int  buffer_offset          = 1;

    if(**expression == '\0' || **expression == '*' || **expression == '^' || **expression == '/' || **expression == '+' ||
       **expression == '-'  || **expression == '~' || **expression == ',' || **expression == '.' || **expression == ')') { // TODO REMOVE ~ and change parameters
        return 0;
    }

    RET_IF(sscanf(*expression, "%[A-Za-z_] %n", buffer + buffer_offset, &var_length) != 1, INITIALIZER_LEXER_ERROR, "syntax error, cannot read variable or number\n"
                                                                                                                    "Beggining of error segment: %s", *expression);

    Tree_node variable = {};

    if(strcmp(buffer + buffer_offset, "o_O") == 0) { // Terminated symbols
        return 0;
    } else if(strcmp(buffer + buffer_offset, "of") == 0) {
        return 0;
    } else if(strcmp(buffer + buffer_offset, "g") == 0) {
        return 0;
    } else if (strcmp(buffer + buffer_offset, "ml") == 0) {
        return 0;
    } else if (strcmp(buffer + buffer_offset, "serves") == 0) {
        return 0;
    }
    
    if(SimpleHashTableSearch(operation_table, hash_table_size, WORD_HASH(buffer)) == NOT_FOUNDED) {     

        *expression += var_length;    

        buffer[0] = '#';                                                                                        
        printf("BUFFER: %s", buffer);

        FillData(&variable, STRING, buffer);
        CURRENT_TOKEN = variable;

        INCREASE_SIZE_WITH_CHECK(1);
    }

    IGNORE_SPACES();
    return 0;    
}

int GetNumber(char** expression,      Tree_node* tokens,          int* tokens_size, int* tokens_capacity, hash_type* operation_table,
              int    hash_table_size, int        binary_operation_part)
{
    IGNORE_SPACES();
    const char expr_begin = **expression;
    double value          = TransferStringToDouble(expression);

    if(**expression == '\0' ) { return 0;}

    if(**expression == expr_begin) {        

        IGNORE_SPACES();

        RET_IF(GetVar(expression,      tokens, tokens_size, tokens_capacity, operation_table,
                      hash_table_size, binary_operation_part) != 0, INITIALIZER_LEXER_ERROR,
               "Error in GetVar() function\n");
        return 0;
    }

    IGNORE_SPACES();

    FillData(&CURRENT_TOKEN, DOUBLE, &value);

    INCREASE_SIZE_WITH_CHECK(1);
    return 0;
}

int GetDivMul(char** expression,      Tree_node* tokens,                int* tokens_size, int* tokens_capacity, hash_type* operation_table,
              int    hash_table_size, int        binary_operation_part)
{
    IGNORE_SPACES();
    GetPow(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);

    while(**expression == '*' || ** expression == '/')
    {
        char operation = **expression;
        ++*expression;

        FillData(&CURRENT_TOKEN, OPERATOR, &operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();

        GetPow(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
    } 
    return 0;
}

int GetPlusMinus(char** expression,      Tree_node* tokens,                int*   tokens_size, int* tokens_capacity, hash_type* operation_table,
                int     hash_table_size, int        binary_operation_part)
{
    IGNORE_SPACES();
    GetDivMul(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);

    while(**expression == '+' || **expression == '-')
    {
        char operation = **expression;
        ++*expression;
        FillData(&CURRENT_TOKEN, OPERATOR, &operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();        
        GetDivMul(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
    }
    return 0;
}

int GetBinaryOperator(char** expression,      Tree_node* tokens,           int* tokens_size, int* tokens_capacity, hash_type* operation_table,
                      int    hash_table_size, int        binary_operation_part)
{
    int is_function = 0;
    IGNORE_SPACES();
    
    if(**expression != '\0' && **expression != '+' && **expression != '-' && 
       **expression != '*' &&  **expression != '/' && **expression != '^') {
        int find_indx = 0;

        while((find_indx = SimpleHashTableSearch(operation_table + binary_operation_part, hash_table_size - binary_operation_part, 
                                          WORD_HASH(*expression))) != NOT_FOUNDED)
        {
            if(strncmp(operation_table[find_indx + binary_operation_part].string, "not", strlen("not")) == 0) {
                const char* operation = "!";
                FillData(&CURRENT_TOKEN, STRING, operation);   
            } else {
                FillData(&CURRENT_TOKEN, STRING, operation_table[find_indx + binary_operation_part].string);
            }

            IGNORE_SPACES();

            if(is_function == 0) {
                INCREASE_SIZE_WITH_CHECK(1);
                *expression += WordLength(operation_table[find_indx + binary_operation_part].string);
            }

            GetBinaryOperator(expression, tokens, tokens_size, tokens_capacity, 
                              operation_table, hash_table_size, binary_operation_part);

            if(**expression == '\0') {
                break;
            }
        }
    }

    return GetFunction(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
}

int GetPow(char**     expression,      Tree_node* tokens,          int* tokens_size,      int* tokens_capacity, 
           hash_type* operation_table, int        hash_table_size, int  binary_operation_part)
{
    IGNORE_SPACES();
    GetLogicalOp(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);

    while(**expression == '^')
    {
        char operation = **expression;
        ++*expression;

        FillData(&CURRENT_TOKEN, OPERATOR, &operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();

        GetLogicalOp(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
    } 
    return 0;
}

int GetLogicalOp(char**     expression, Tree_node* tokens,          int* tokens_size,                int*    tokens_capacity, 
                 hash_type* operation_table, int   hash_table_size, int  binary_operation_part)
{
    IGNORE_SPACES();

    GetBinaryOperator(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);

    int find_indx = -1;

    while((find_indx = SimpleHashTableSearch(operation_table, binary_operation_part, WORD_HASH((*expression)))) != -1)
    {
        const char* operation = DefineLogicalOp(*expression, WordLength(*expression));

        
        *expression += WordLength(*expression);
    
        FillData(&CURRENT_TOKEN, STRING, operation);

        INCREASE_SIZE_WITH_CHECK(1);
        IGNORE_SPACES();

        GetBinaryOperator(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
    } 
    return 0;
}

const char* DefineLogicalOp(char* logical_operation, int word_length)
{
    if(strncmp("or", logical_operation, word_length) == 0) {
        return "||";
    } else if(strncmp("and", logical_operation, word_length) == 0) {
        return "&&";
    } else if(strncmp("chill", logical_operation, word_length) == 0) {
        return ">";
    } else if(strncmp("heat", logical_operation, word_length) == 0) {
        return "<";
    } else if (strncmp("is", logical_operation, word_length) == 0) {
        return "=";
    }
    return nullptr;
}

int GetBracket(char** expression,      Tree_node* tokens,                int* tokens_size, int* tokens_capacity, hash_type* operation_table,
               int    hash_table_size, int        binary_operation_part)
{
    IGNORE_SPACES();
    if(**expression == '(') {

        char bracket = **expression;
        ++*expression;

        FillData(&CURRENT_TOKEN, CHAR, &bracket);
        INCREASE_SIZE_WITH_CHECK(1);

        IGNORE_SPACES();

        GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);


        RET_IF(!Require(expression, ')'), INITIALIZER_LEXER_ERROR, "Expected ) brackets\n");

        bracket = **expression;

        FillData(&CURRENT_TOKEN, CHAR, &bracket);
        INCREASE_SIZE_WITH_CHECK(1);

        ++*expression;

        IGNORE_SPACES();

        return 0;
    }

    IGNORE_SPACES();
    return GetNumber(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
}

#define DELETE_WITH_TOKEN             \
Tree_node null_node = {};             \
tokens[*tokens_size - 1] = null_node; \
(*tokens_size) -= 1;            


int GetFunction(char**     expression, Tree_node* tokens,          int* tokens_size,           int*    tokens_capacity,
                hash_type* operation_table, int   hash_table_size, int  binary_operation_part)
{
    IGNORE_SPACES();

    printf("expression in GET_FUNCTION: _%s_", *expression);

    if (strncmp(*expression, "cook", strlen("cook")) == 0) {
        IGNORE_SPACES();
        *expression += strlen("cook");

        FillData(&CURRENT_TOKEN, STRING, "call");
        INCREASE_SIZE_WITH_CHECK(1);

        GetVar(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);        

        FillData(&CURRENT_TOKEN, STRING, "parameter");
        INCREASE_SIZE_WITH_CHECK(1);

        GetVar(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);        

        RET_IF(strcmp(tokens[*tokens_size - 1].data.type_string, "#with") != 0, EXPECTED_KEYWORD, "Expected <with>\n");

        DELETE_WITH_TOKEN;
        

        IGNORE_SPACES();

            GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);        

            while(**expression == ',' || **expression == '.') {  // Parameters proccessing

                char between_parameter_sign = '~';

                if(**expression == ',') {
                    between_parameter_sign = '$';
                    FillData(&CURRENT_TOKEN, CHAR, &between_parameter_sign);
                } else if(**expression == '.') {
                    FillData(&CURRENT_TOKEN, CHAR, &between_parameter_sign);
                }

                ++*expression;
                INCREASE_SIZE_WITH_CHECK(1);

                GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
            }
    } 
    
    return GetBracket(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);
}


#define DEF_KEY_WORD(keyword)                          \
{#keyword, MurmurHash(#keyword, strlen(#keyword))}

int InitializerLexer(char** expression, Tree_node* tokens, int* tokens_size, int* tokens_capacity, int terminate_symbol)
{
    INIT_HASH_TABLE;
    const int binary_operation_part = 5;  // This variable show the end index in hash table for operation such as && (and) || (or)

    GetPlusMinus(expression, tokens, tokens_size, tokens_capacity, operation_table, hash_table_size, binary_operation_part);

    IGNORE_SPACES();

    printf("INITIALIZER LEXER EXPRESSION: %s\n", *expression);

    if(terminate_symbol == DECLARATION_PROCCESSING) {
        if(strncmp(*expression, "ml", WordLength(*expression)) == 0     || strncmp(*expression, "g", WordLength(*expression))   == 0 ||  // Reserved Words
           strncmp(*expression, "of", WordLength(*expression) == 0)     || strncmp(*expression, "o_O", WordLength(*expression)) == 0 || 
           strncmp(*expression, "serves", WordLength(*expression)) == 0) {
            printf("RETURNED");
            return 0;
        } else {
            ErrorPrint("Expected terminated symbol such as ml, g, of, o_O\n");
            return INITIALIZER_LEXER_ERROR;
        }
    }

    if(terminate_symbol == 0) {       
        RET_IF(!IsInitializerEnd(expression), INITIALIZER_LEXER_ERROR, "Expected terminated ~\n");        
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

Tree_node* TakeVarOrNumberNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    int is_var = 0;
    if(tokens[*tokens_size].type_id == STRING) {
        if(SimpleHashTableSearch(operation_table, hash_table_size, 
           MurmurHash(CURRENT_TOKEN.data.type_string, strlen(CURRENT_TOKEN.data.type_string))) == NOT_FOUNDED) {   
            is_var = 1;
        }
    }

    if(tokens[*tokens_size].type_id == DOUBLE || is_var) {
        int temp = *tokens_size;

        (*tokens_size) += 1;

        return &tokens[temp];
    }
    return &CURRENT_TOKEN;
}

Tree_node* TakeFunctionNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    Tree_node* curr_op = &CURRENT_TOKEN;
    if(curr_op->type_id == STRING) {
        if(strcmp(curr_op->data.type_string, "call") == 0) {

                (*tokens_size) += 1;

                Tree_node function = {};
                FillData(&function, STRING, "function");

                ConnectNodes(curr_op, &function);

                Tree_node* function_node_ptr = &curr_op->child_nodes[0];  // Rework

                Tree_node* function_name = TakeVarOrNumberNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
                ConnectNodes(function_node_ptr, function_name);

                Tree_node* parameter = &tokens[*tokens_size];
                ConnectNodes(function_node_ptr, parameter);
                (*tokens_size) += 1;
                
                parameter = &function_node_ptr->child_nodes[function_node_ptr->node_degree - 1];

                Tree_node* parameter_to_connect = TakeAssignmentNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
                
                while(true) 
                {
                    if(tokens[*tokens_size].data.type_char == '$') {
                        (*tokens_size) += 1;
                    } else if(tokens[*tokens_size].data.type_char == '~') {
                        (*tokens_size) += 1;
                        ConnectNodes(parameter, parameter_to_connect);
                        break;
                    } else {
                        ErrorPrint("Expected parameter separators such $ or ~\n");
                        return nullptr;
                    }

                ConnectNodes(parameter, parameter_to_connect);

                parameter_to_connect = TakeAssignmentNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);

            }
                
            return curr_op;
        }
    }
    return TakeBracketNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
}

Tree_node* TakeUnaryOperatorNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    int is_function = 0;
    if(tokens[*tokens_size].type_id == STRING) {
        if(SimpleHashTableSearch(operation_table + binary_operation_part, hash_table_size - binary_operation_part, 
                         MurmurHash(CURRENT_TOKEN.data.type_string, strlen(CURRENT_TOKEN.data.type_string))) != NOT_FOUNDED) {
            is_function = 1;   
        } else if (*CURRENT_TOKEN.data.type_string == '!') {
            is_function = 1;
        }
    }

    if(is_function) {
        Tree_node* curr_op = &tokens[*tokens_size];
        (*tokens_size) += 1;

        Tree_node* initializer = TakeUnaryOperatorNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);             
        ConnectNodes(curr_op, initializer);

        return curr_op;
    }

    return TakeFunctionNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
}

Tree_node* TakeBracketNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    if(tokens[*tokens_size].is_filled && tokens[*tokens_size].type_id == CHAR) {
        if(tokens[*tokens_size].data.type_char == '(') {
            (*tokens_size) += 1;

            Tree_node* expr = TakeAssignmentNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);            

            (*tokens_size) += 1;

            return expr;
        }
    }

    return TakeVarOrNumberNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
    
}
                                    /// token size is begining of block
Tree_node* TakeAssignmentNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    Tree_node* first_expr  = TakeLogicalOperationNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
    Tree_node* curr_op     = &CURRENT_TOKEN;

       if(IsAssignment(&CURRENT_TOKEN)) {
        while(IsAssignment(&CURRENT_TOKEN))
        {
            ConnectNodes(curr_op, first_expr);
            (*tokens_size) += 1;

            first_expr = TakeLogicalOperationNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
        }
    }

    if(curr_op->is_filled == 0) {
        return first_expr;
    }

    if(curr_op->data.type_char != ')' && curr_op->data.type_char != '~' && curr_op->data.type_char != '$') {
        ConnectNodes(curr_op, first_expr);
        return curr_op;
    }

    return first_expr;

}

Tree_node* TakeLogicalOperationNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    Tree_node* first_expr = TakeComparisonOperationNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
    Tree_node* curr_op    = &CURRENT_TOKEN;
    int        is_logical = 0;
    int        is_changed = 0;

    if(isLogicalOp(&CURRENT_TOKEN)) {
        while(isLogicalOp(&tokens[*tokens_size]))
        {
            is_logical = 1;

            ConnectNodes(curr_op, first_expr);

            if(*curr_op->data.type_string != *CURRENT_TOKEN.data.type_string) {
               ConnectNodes(&CURRENT_TOKEN, curr_op);
                 is_changed = 1;
            }

            if(is_changed) {
                curr_op = &CURRENT_TOKEN;
                is_changed = 0;
            }

            (*tokens_size)++;

            first_expr = TakeComparisonOperationNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
        }
    }
    if(is_logical) {
        ConnectNodes(curr_op, first_expr);
    }

    return (is_logical) ? curr_op : first_expr;

}

Tree_node* TakeComparisonOperationNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    Tree_node* first_expr    = TakePlusMinusNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);

    Tree_node* curr_op       = &CURRENT_TOKEN;
    int        is_comparison = 0;

    if(IsComparisonOp(&CURRENT_TOKEN)) {
        while(IsComparisonOp(&CURRENT_TOKEN))
        {
            is_comparison = 1;

            ConnectNodes(curr_op, first_expr);
            (*tokens_size)++;

            first_expr = TakePlusMinusNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
        }
    }

    if(is_comparison) {
        ConnectNodes(curr_op, first_expr);
    }

    return (is_comparison) ? curr_op : first_expr;
}

Tree_node* TakePlusMinusNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    int is_changed_node    = 0;
    int is_minus_plus      = 0;
    Tree_node* first_expr  = TakeDivMulNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
    Tree_node* curr_op     = &CURRENT_TOKEN;

    if(IsOperator(&CURRENT_TOKEN)) {
        while(CURRENT_TOKEN.data.type_char == '+' || CURRENT_TOKEN.data.type_char == '-')
        {
            ConnectNodes(curr_op, first_expr);
            is_minus_plus = 1;

            if(curr_op->data.type_char != CURRENT_TOKEN.data.type_char) {
                ConnectNodes(&CURRENT_TOKEN, curr_op);
                is_changed_node = 1;
            }
            
            if(is_changed_node) {
                curr_op = &CURRENT_TOKEN; 
                is_changed_node = 0;
            }

            (*tokens_size)++;
            first_expr = TakeDivMulNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
        }
    }

    if(is_minus_plus) {
        ConnectNodes(curr_op, first_expr);
    }

    return (is_minus_plus) ? curr_op : first_expr;

}

Tree_node* TakePowNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    int is_changed_node  = 0;
    int is_pow           = 0;
    
    Tree_node* first_number = TakeUnaryOperatorNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);

    Tree_node* curr_op = &CURRENT_TOKEN;

    if(IsOperator(&CURRENT_TOKEN)) {
        while(CURRENT_TOKEN.data.type_char == '^')
        {
            is_pow = 1;
            ConnectNodes(curr_op, first_number);

            (*tokens_size)++;

            first_number = TakeUnaryOperatorNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
        }
    }

    if(is_pow == 1) { 
         ConnectNodes(curr_op, first_number);
    }

    return (is_pow) ? curr_op : first_number;

}

Tree_node* TakeDivMulNode(Tree_node* tokens, int* tokens_size, hash_type* operation_table, int hash_table_size, int binary_operation_part)
{
    int is_changed_node     = 0;
    int is_mul_or_div       = 0;

    Tree_node* first_number = TakePowNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
    Tree_node* curr_op      = &CURRENT_TOKEN;

    if(IsOperator(&CURRENT_TOKEN)) {
        while(CURRENT_TOKEN.data.type_char == '/' || CURRENT_TOKEN.data.type_char == '*')
        {
            is_mul_or_div = 1;
            ConnectNodes(curr_op, first_number);

            if((curr_op->data.type_char != CURRENT_TOKEN.data.type_char)) {
                ConnectNodes(&CURRENT_TOKEN, curr_op);
                is_changed_node = 1;
            }
            
            if(is_changed_node) {
                curr_op = &CURRENT_TOKEN;                
                is_changed_node = 0;
            }

            (*tokens_size)++;

            first_number = TakePowNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
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

    INIT_HASH_TABLE;

    const int binary_operation_part = 5;
    
    return TakeAssignmentNode(tokens, tokens_size, operation_table, hash_table_size, binary_operation_part);
}

