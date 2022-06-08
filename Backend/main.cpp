#include "../Frontend/language_processing.h"
#include "AssembleTree.h"

int main()
{
    Tree_node tokens[512] = {};

    FILE* source      = fopen("source.txt", "r");
    char* buffer      = nullptr;
    int   buffer_size = 0;

    buffer = MakeBufferForFile(source, &buffer_size);
    fread(buffer, sizeof(char), buffer_size, source);
    buffer[buffer_size] = '\0';

    int token_size     = 0;
    int token_capacity = 512;
    int error_code     = 0;

    Tree_node  to_test = {};
    Tree_node* ptr = &to_test;
    FillData(ptr, STRING, "statement");

    GetFunction(&buffer, tokens, ptr, &token_size, &token_capacity, &error_code);

    TreeGraphicalDump_(ptr, "Please.txt");

    NameTable_T name_table = {};
    NameTableCtor(&name_table, NAME_TABLE_MIN_SIZE);

    FILE* test = fopen("Cringe.asm", "w");

    AssembleStatement(ptr, &name_table, test);
    
    fclose(test);
    TreeDtor(ptr);
    NameTableDtor(&name_table);
            
}


