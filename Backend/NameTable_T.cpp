#include "NameTable_T.h"
#include "../lib/log.h"

int NameTableCtor(NameTable_T* name_table, int size)
{
    RET_IF(size       <= 0,       INCCORECT_SIZE,        "Inccorect size, he is should be positive\n");
    RET_IF(name_table == nullptr, NULLPTR_NAME_TABLE,    "Cannot constuct nullptr object\n");

    name_table->var_names = (NameTableNode_T*)calloc(size, sizeof(NameTableNode_T));

    RET_IF(name_table->var_names == nullptr, MEMORY_ALLOCATION_ERROR, "Memory allocation error\n");

    name_table->var_size = 0;
    name_table->var_capacity = size;

    return 0;
}

int NameTableDtor(NameTable_T* name_table)
{
    RET_IF(name_table == nullptr, NULLPTR_NAME_TABLE,    "Cannot destruct nullptr object\n");

    free(name_table->var_names);
    return 0;
}

int AddName(NameTable_T* name_table, hash_t identifier_hash)
{
    if(name_table->var_size >= name_table->var_capacity) {
        name_table->var_names = (NameTableNode_T*)realloc(name_table->var_names, 2*name_table->var_capacity * sizeof(NameTableNode_T));

        RET_IF(name_table->var_names == nullptr, MEMORY_ALLOCATION_ERROR, "Cannot realloc current name_table\n");
    }
    if(FindVarName(name_table, identifier_hash) != NOT_FOUNDED_VAR) {
        ErrorPrint("Name collision\n");
        return NAME_COLLISION;
    }    
    name_table->var_names[name_table->var_size].hash        = identifier_hash;
    name_table->var_names[name_table->var_size].identifier  = name_table->var_size;

    name_table->var_size++;
    return 0;
}

int FindVarName(NameTable_T* name_table, hash_t identifier_hash)
{
    for(int iter_count = 0; iter_count < name_table->var_size; ++iter_count)
    {
        if(name_table->var_names[iter_count].hash  == identifier_hash) {
            return name_table->var_names[iter_count].identifier;
        }
    }
    return NOT_FOUNDED_VAR;
}

