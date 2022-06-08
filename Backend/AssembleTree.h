
#include "NameTable_T.h"

enum ASSEMBLE_ERRORS
{
    NO_PREVIOUS_DECLARATION = -0x55521,
    UNEXPECTED_NODE,
    EXPECTED_LVALUE,
    EXPECTED_RVALUE,
    NOT_ASSIGNMENT,
    ERRORED_ASSIGNMENT,
    NULLPTR_DECLARATION_NODE,
    EXPECTED_DECLARATION_NODE,
    EXPECTED_VAR_DECLARATION,
    NULLPTR_DEFINE_NODE,
    EXPECTED_DEFINE_NODE,
    ERRORED_DEFINE,
    EXPECTED_FUNCTION_NODE,
    ERROR_FUNCTION_NODE,
    EXPECTED_STATEMENT_NODE,
    INCCORECT_FUNCTION_NAME,
    INCCORECT_PARAMETERS,
    NULLPTR_PARAMETER,
    NULLPTR_CHILD_NODE_PARAMETER,
    NULLPTR_CALL,
    ERRORED_CALL_NODE
};

const int IS_VAR              = 1;
const int NAME_TABLE_MIN_SIZE = 32;
const int MORE                = 1;
const int LESS                = 2;

int AssembleCallParameter(Tree_node* parameter, NameTable_T* name_table, FILE* dst);

int AssembleCondition(Tree_node* if_node, NameTable_T* name_table, FILE* dst);

int AssembleRetNode(Tree_node* ret_node, NameTable_T* name_table, FILE* dst);

int AssembleCall(Tree_node* call_node, NameTable_T* name_table, FILE* dst);

int AssembleVariableForOperation(Tree_node* var_node, NameTable_T* name_table, FILE* dst);

int AssembleOperations(Tree_node* operation_node, NameTable_T* name_table, FILE* dst);

int AssembleStatement(Tree_node* statement, NameTable_T* root_name_table, FILE* dst);

int AssembleFunction(Tree_node* define, NameTable_T* name_table, FILE* dst);

int AssembleDeclaration(Tree_node* declaration, NameTable_T* name_table, FILE* dst);

int AssembleAssignment(Tree_node* assignment, NameTable_T* name_table, FILE* dst);

int AssembleVariableForOperation(Tree_node* var_node, NameTable_T* name_table, FILE* dst);

int AssembleOperations(Tree_node* operation_node, NameTable_T* name_table, FILE* dst);

int AssembleStatement(Tree_node* statement, NameTable_T* root_name_table, FILE* dst);