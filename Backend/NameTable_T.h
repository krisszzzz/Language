#include "../lib/murmurhash.h"
#include "../lib/tree.h"

#define MAX_LAYER 128

struct NameTableNode_T
{
    hash_t hash;
    int    identifier;
};

struct NameTable_T 
{
    int               var_size;
    int               function_size;
    int               var_capacity;
    NameTableNode_T*  var_names;
};


enum NameTableErrors
{
    INCCORECT_SIZE = -0x5555,
    MEMORY_ALLOCATION_ERROR,
    NOT_FOUNDED_VAR,
    NOT_FOUNDED_FUNCTION,
    NULLPTR_NAME_TABLE,
    NAME_COLLISION,
    NULLPTR_STACK
};


int NameTableCtor(NameTable_T* name_table, int size);

int NameTableDtor(NameTable_T* name_table);

int FindVarName(NameTable_T* name_table, hash_t identifier_hash);

int AddName(NameTable_T* name_table, hash_t identifier_hash);

