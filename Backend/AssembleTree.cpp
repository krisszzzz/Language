#include "AssembleTree.h"
#include "../Frontend/language_processing.h"
            // Yes it's not right, but funcking debug


#define CURRENT_STATENT(number)     \
statement->child_nodes[number]

#define OP_CURRENT(number)          \
operation_node->child_nodes[number]

#define ASSIGNMENT_CURRENT(number)  \
assignment->child_nodes[number]

#define DECLARE_CURRENT(number)     \
declaration->child_nodes[number]

#define PARAMETER_CURRENT(number)   \
parameter->child_nodes[number]

int isLogicalOpNode(Tree_node* op)
{
    if(op->type_id == STRING) {
        if(strcmp(op->data.type_string, ">") == 0) {
            return MORE;
        }
        if(strcmp(op->data.type_string, "<") == 0) {
            return LESS;
        }
    }
    return -1;
}

int IsOperatorNode(Tree_node* op)
{
    if(op->type_id == OPERATOR) {
        if(op->data.type_char == '+' || op->data.type_char == '*' || op->data.type_char == '-' 
        || op->data.type_char == '/' || op->data.type_char == '^') {
            return 1;
        }
    }
    return 0;
}

const char* DefineOperator(Tree_node* op)
{
    fprintf(stderr, "CURRENT OP _");
    FprintNodeData(op, stderr);
    fprintf(stderr, "_\n");
    RET_IF(!IsOperator(op), nullptr,  "Expected operation\n");
    switch(op->data.type_char) 
    {
        case '+': return "add";
        case '-': return "sub";
        case '*': return "mul";
        case '/': return "div";
        default: {
            return nullptr;
        }
    }
}

int AssembleOperations(Tree_node* operation_node, NameTable_T* name_table, FILE* dst)
{
    RET_IF(operation_node    == nullptr,  NULLPTR_NODE,       "Cannot assemble nullptr node\n");
    RET_IF(name_table        ==  nullptr, NULLPTR_NAME_TABLE, "Cannot work without name table\n");

    AssembleVariableForOperation(operation_node, name_table, dst);

    const char* operation = DefineOperator(operation_node);

    RET_IF(operation == nullptr, UNEXPECTED_NODE, "Unexpected node, expected operator node\n");
    fprintf(stderr, "OPERATION = %s\n", operation);

    getchar();

    if(operation_node->child_nodes != nullptr) {
        for(int iter_count = 0; iter_count < operation_node->node_degree; ++iter_count) 
        {
            if(OP_CURRENT(iter_count).type_id == STRING) {

                if(*(OP_CURRENT(iter_count).data.type_string) == '#') {

                    AssembleVariableForOperation(&OP_CURRENT(iter_count), name_table, dst);

                } else if(strcmp(OP_CURRENT(iter_count).data.type_string, "call") == 0) {

                    AssembleCall(&OP_CURRENT(iter_count), name_table, dst);
                }

            } else if(IsOperator(&OP_CURRENT(iter_count))) {

                AssembleOperations(&OP_CURRENT(iter_count), name_table, dst);

            } else if(OP_CURRENT(iter_count).type_id == DOUBLE) {

                fprintf(dst, "push %lf \n", OP_CURRENT(iter_count).data.type_double);                            

            } else {
                FprintNodeData(&OP_CURRENT(iter_count), stderr);
                ErrorPrint("Unexpected child nodes\n");
            }
        }

        for(int iter_count = 0; iter_count < operation_node->node_degree - 1; ++iter_count) 
        {
            fprintf(dst, "%s\n", operation);
        }
    }
    return 0;
}

int AssembleVariableForOperation(Tree_node* var_node, NameTable_T* name_table, FILE* dst)
{
    if(var_node->type_id == STRING) {

        if(*var_node->data.type_string == '#') {

            int buffer_offset = 1;
            int var_identifier = FindVarName(name_table, WORD_HASH(var_node->data.type_string));

            RET_IF(var_identifier == NOT_FOUNDED_VAR, NO_PREVIOUS_DECLARATION, "varible %s is not declared in this scope\n",
                                                                                var_node->data.type_string
                                                                                + buffer_offset);           
                        
            fprintf(dst, ";Current node type_id = %d, node data - ",  var_node->type_id);
            FprintNodeData(var_node, dst);             
            fprintf(dst, "\n");

            fprintf(dst, "push [bx + %d] \n", var_identifier);
            return IS_VAR;            
        }
    }
    return 0;
}


int AssembleAssignment(Tree_node* assignment, NameTable_T* name_table, FILE* dst)
{
    RET_IF(assignment                    == nullptr, NULLPTR_NODE,       "Cannot assemble nullptr node\n");
    RET_IF(name_table                    == nullptr, NULLPTR_NAME_TABLE, "Cannot work without name table\n");
    RET_IF(assignment->type_id           != STRING,  NOT_ASSIGNMENT,     "Expected assignment\n");
    RET_IF(*assignment->data.type_string != '=',     NOT_ASSIGNMENT,     "Expected assignment node\n");
    RET_IF(assignment->child_nodes       == nullptr, ERRORED_ASSIGNMENT, "Expected assignment with correct child nodes\n");

    int right_value = assignment->node_degree - 1;

    if(ASSIGNMENT_CURRENT(right_value).type_id == DOUBLE) { // Should be double in this case
        fprintf(dst, "push %lf \n", ASSIGNMENT_CURRENT(right_value).data.type_double);
    } else {
        RET_IF(AssembleOperations(&ASSIGNMENT_CURRENT(right_value), name_table, dst) != 0, EXPECTED_RVALUE, "Expected right value\n");
    }

    fprintf(dst, "pop ax \n");   

    if(assignment->child_nodes != nullptr) {
        for(int iter_count = 0; iter_count < right_value; iter_count++) 
        {
            if(ASSIGNMENT_CURRENT(iter_count).type_id !=  STRING) {
                ErrorPrint("Expected a l-value in multi assignment expression\n");
                return EXPECTED_LVALUE;

            } else if(*ASSIGNMENT_CURRENT(iter_count).data.type_string != '#') {
                ErrorPrint("Expected a l-value in multi assignment expression\n");
                return EXPECTED_LVALUE;
            }

            int buffer_offset = 1;
            int var_identifier = FindVarName(name_table, WORD_HASH(ASSIGNMENT_CURRENT(iter_count).data.type_string));

            RET_IF(var_identifier == NOT_FOUNDED_VAR, NO_PREVIOUS_DECLARATION, "varible %s is not declared in this scope\n",
                                                                                ASSIGNMENT_CURRENT(iter_count).data.type_string + buffer_offset);           
            
            fprintf(dst, ";Current node type_id = %d, iter_count = %d, node data - ", ASSIGNMENT_CURRENT(iter_count).type_id, iter_count);
            FprintNodeData(&ASSIGNMENT_CURRENT(iter_count), dst);      
            fprintf(dst, "\n");

            fprintf(dst, "push ax \n");
            fprintf(dst, "pop [bx + %d] \n", var_identifier);
        }
    }

    return 0;
}

int AssembleDeclaration(Tree_node* declaration, NameTable_T* name_table, FILE* dst)
{
    RET_IF(declaration           == nullptr,                     NULLPTR_DECLARATION_NODE,  "Cannot work with nullptr declaration node\n");    
    RET_IF(declaration->type_id  != STRING,                      EXPECTED_DECLARATION_NODE, "Expected variable declaration, but declaration node is inccorect\n");
    RET_IF(strcmp(declaration->data.type_string, "declaration")
           && strcmp(declaration->data.type_string, "parameter"), EXPECTED_DECLARATION_NODE, "Expected declaration node\n");

    for(int iter_count = 0; iter_count < declaration->node_degree; ++iter_count)
    {
        if(DECLARE_CURRENT(iter_count).type_id == STRING) {
            if(*DECLARE_CURRENT(iter_count).data.type_string == '=') {
            
                Tree_node* assignment   = &DECLARE_CURRENT(iter_count);
                int exclude_right_value = 1;

                for(int iter_count = 0; iter_count < assignment->node_degree - exclude_right_value; ++iter_count) 
                {
                    if(ASSIGNMENT_CURRENT(iter_count).type_id == STRING) {
                        if(*ASSIGNMENT_CURRENT(iter_count).data.type_string == '#') {
                            AddName(name_table, WORD_HASH(ASSIGNMENT_CURRENT(iter_count).data.type_string));                               
                        } else {
                            ErrorPrint("Multi declaration cannot contain any r-value except last r-value\n");
                            return EXPECTED_LVALUE;
                        }
                    } else {
                           ErrorPrint("Multi declaration cannot contain any r-value except last r-value\n");
                           return EXPECTED_LVALUE;
                    }
                }
                AssembleAssignment(&DECLARE_CURRENT(iter_count), name_table, dst);   

            } else if(*DECLARE_CURRENT(iter_count).data.type_string == '#') {
                AddName(name_table, WORD_HASH(DECLARE_CURRENT(iter_count).data.type_string));                                               
            } else {
                ErrorPrint("Expected variable/parameter declaration, anything else is defined like a errror\n");
                return EXPECTED_VAR_DECLARATION;
            }
            
        } else {
            ErrorPrint("Expected variable/parameter declaration, anything else is defined like a errror\n");
            return EXPECTED_VAR_DECLARATION;
         }
    }
    return 0;
}

int AssembleFunction(Tree_node* define, NameTable_T* name_table, FILE* dst)
{
    RET_IF(define          == nullptr,                                           NULLPTR_DEFINE_NODE,       "Cannot work with nullptr function node\n");
    RET_IF(define->type_id != STRING,                                            EXPECTED_DEFINE_NODE,      "Expected function node\n");
    RET_IF(strcmp(define->data.type_string, "define"),                           EXPECTED_DEFINE_NODE,      "Expected function node\n");    

    RET_IF(define->child_nodes == nullptr,                                       ERRORED_DEFINE,            "Expected child nodes for define node\n"); // TODO warning may fall

    RET_IF(strcmp(define->child_nodes[0].data.type_string, "function"),          EXPECTED_FUNCTION_NODE,    "Expected function node like a first child node of \"define\" node\n");
    RET_IF(strcmp(define->child_nodes[1].data.type_string, "declaration"),       EXPECTED_DECLARATION_NODE, "Expected declaration node like a second child node of"
                                                                                                           " \"define\" node\n");
    RET_IF(strcmp(define->child_nodes[2].data.type_string, "statement"),         EXPECTED_STATEMENT_NODE,   "Expected statement node like a third child node of "
                                                                                                            "\"define\" node\n");
    Tree_node* function       = &define->child_nodes[0];
    Tree_node* param          = &function->child_nodes[1];
    Tree_node* function_name  = &function->child_nodes[0];

    RET_IF(function->child_nodes == nullptr,                                     ERROR_FUNCTION_NODE,       "Expected corect function node with existing child nodes\n");
                                // name node
    RET_IF(function->type_id != STRING,                                          INCCORECT_FUNCTION_NAME,   "Expected node with function name, but is inccorect\n");

    RET_IF(AssembleDeclaration(param, name_table, dst) != 0,                     INCCORECT_PARAMETERS,      "Inccorect parameter node\n");    

    RET_IF(AssembleDeclaration(&define->child_nodes[1], name_table, dst)   != 0, EXPECTED_VAR_DECLARATION, "Variable or parameter declaration error\n");
    
    fprintf(dst, ";Current node type_id = %d, node data - ", function_name->type_id);
    FprintNodeData(function_name, dst);      
    fprintf(dst, "\n");

    fprintf(dst, ":%llu\n", WORD_HASH(function_name->data.type_string));    
    AssembleStatement(&define->child_nodes[2], name_table, dst);
    return 0;
    
}

int AssembleStatement(Tree_node* statement, NameTable_T* root_name_table, FILE* dst)
{
    for(int iter_count = 0; iter_count < statement->node_degree; ++iter_count)
    {
        if(statement->child_nodes[iter_count].type_id == 0) {
            continue;
        }

        if(CURRENT_STATENT(iter_count).type_id == STRING) {
            if(strcmp(CURRENT_STATENT(iter_count).data.type_string, "define") == 0) {

                NameTable_T name_table = {};
                NameTableCtor(&name_table, NAME_TABLE_MIN_SIZE);

                AssembleFunction(&CURRENT_STATENT(iter_count), &name_table, dst);                   

            } else if(*CURRENT_STATENT(iter_count).data.type_string == '=') {

                AssembleAssignment(&CURRENT_STATENT(iter_count), root_name_table, dst);

            } else if(strcmp(CURRENT_STATENT(iter_count).data.type_string, "if") == 0) {

                AssembleCondition(&CURRENT_STATENT(iter_count), root_name_table, dst);

            } else if(strcmp(CURRENT_STATENT(iter_count).data.type_string, "return") == 0) {

                AssembleRetNode(&CURRENT_STATENT(iter_count), root_name_table, dst);

            } else if(strcmp(CURRENT_STATENT(iter_count).data.type_string, "call") == 0) {

                AssembleCall(&CURRENT_STATENT(iter_count), root_name_table, dst);                 

            } else if(strcmp(CURRENT_STATENT(iter_count).data.type_string, "if") == 0) {

                AssembleCondition(&CURRENT_STATENT(iter_count), root_name_table, dst);
            }
            else {
                 ErrorPrint("Unknown node type, node type id - %d\n", CURRENT_STATENT(iter_count).type_id);
                 FprintNodeData(&CURRENT_STATENT(iter_count), stderr);
                 return UNEXPECTED_NODE;
            }
        } else {
                ErrorPrint("Unknown node type\n");
                return UNEXPECTED_NODE;
        }
    } 
    return 0;       
}

int AssembleRetNode(Tree_node* ret_node, NameTable_T* name_table, FILE* dst)
{
    if(ret_node->child_nodes[0].type_id == STRING) {

        if(*ret_node->child_nodes[0].data.type_string == '#') {
            AssembleVariableForOperation(&ret_node->child_nodes[0], name_table, dst);
        } else {
            RET_IF(AssembleOperations(&ret_node->child_nodes[0], name_table, dst) != 0, EXPECTED_RVALUE, "Expected rvalue in return case\n");
        }

    } else if(ret_node->child_nodes[0].type_id == DOUBLE) {
        fprintf(dst, "push %lf \n", ret_node->child_nodes[0].data.type_double);   
    } else if (IsOperatorNode(&ret_node->child_nodes[0])) {
        printf("\nOK\n");
        AssembleOperations(&ret_node->child_nodes[0], name_table, dst);
    }

    fprintf(dst, "push bx \n");                                //WARNING
    fprintf(dst, "push %d \n", name_table->var_size);
    fprintf(dst, "sub \n");
    fprintf(dst, "pop bx \n");
    fprintf(dst, "ret \n");

    return 0;
}

int AssembleCallParameter(Tree_node* parameter, NameTable_T* name_table, FILE* dst)
{
    RET_IF(parameter              ==  nullptr, NULLPTR_PARAMETER,             "Cannot work with nullptr parameter node\n");
    RET_IF(name_table             == nullptr,  NULLPTR_NAME_TABLE,            "Cannot work with nullptr name table\n");
    RET_IF(parameter->child_nodes == nullptr,  NULLPTR_CHILD_NODE_PARAMETER,  "Cannot work with parameter node that "
                                                                              "don't have child nodes.\n" );                                                                             

    for(int iter_count = 0; iter_count < parameter->node_degree; ++iter_count)
    {
        int offset = name_table->var_size + iter_count;
        if(parameter->child_nodes[iter_count].type_id == DOUBLE) {

            fprintf(dst, "push %d \n", parameter->child_nodes[iter_count].data.type_double);            
            fprintf(dst, "pop [bx + %d] \n", offset);

        } else if(parameter->child_nodes[iter_count].type_id == STRING) {

            if(*parameter->child_nodes[iter_count].data.type_string == '#') {
                AssembleVariableForOperation(&parameter->child_nodes[iter_count], name_table, dst);
                fprintf(dst, "pop [bx + %d] \n", offset);
            }
        } else if (IsOperatorNode(&parameter->child_nodes[iter_count])) {

            AssembleOperations(&parameter->child_nodes[iter_count], name_table, dst);
            fprintf(dst, "pop [bx + %d] \n", offset);

        } else {
            ErrorPrint("Expected parameter for call function\n");
            return EXPECTED_PARAMETER;
        }
    }

    fprintf(dst, "push bx \n");
    fprintf(dst, "push %d \n", name_table->var_size);
    fprintf(dst, "add \n");
    fprintf(dst, "pop bx \n");

    return 0;
}

int AssembleCall(Tree_node* call_node, NameTable_T* name_table, FILE* dst)
{
    RET_IF(call_node  == nullptr,                          NULLPTR_CALL,       "Expected call node but he is nullptr\n");
    RET_IF(name_table == nullptr,                          NULLPTR_NAME_TABLE, "Expected correct name table\n");

    RET_IF(call_node->child_nodes == nullptr,              ERRORED_CALL_NODE,  "Expected correct call node, but it is"
                                                                               " don't have a valid child nodes\n");

    RET_IF(call_node->child_nodes[0].type_id != STRING,    ERRORED_CALL_NODE,   "Errored call node, expected that first parameter will be "
                                                                                "function\n" );
    Tree_node* function = &call_node->child_nodes[0];

    RET_IF(strcmp(function->data.type_string, "function"),  ERRORED_CALL_NODE,    "Errored call node, expected that first parameter will be "
                                                                                  "function\n" );

    Tree_node* param  = &function->child_nodes[1];

    AssembleCallParameter(param, name_table, dst);

    fprintf(dst, "call :%llu\n", WORD_HASH(function->child_nodes[0].data.type_string));
    return 0;       
}

int AssembleCondition(Tree_node* if_node, NameTable_T* name_table, FILE* dst)
{
    RET_IF(if_node == nullptr, NULLPTR_NODE, "Cannot work with nullptr node\n");  

    Tree_node* condition  = &if_node->child_nodes[0];
    Tree_node* if_body    = &if_node->child_nodes[1];

    for(int iter_count = 0; iter_count < condition->node_degree; iter_count++)
    {
        if(condition->child_nodes[iter_count].type_id == DOUBLE) {
            fprintf(dst, "push %lf\n", condition->child_nodes[iter_count].data.type_double);
        } else if(condition->child_nodes[iter_count].type_id == STRING) {
            AssembleVariableForOperation(&condition->child_nodes[iter_count], name_table, dst);
        }
    }    
    
    static int label_count = 0;
    label_count++;
    fprintf(dst, "jb :%llu\n", label_count);

    
    AssembleRetNode(if_body, name_table, dst); 
    fprintf(dst, ": %llu\n", label_count);

    return 0;        
}


