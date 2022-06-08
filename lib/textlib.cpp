#include "textlib.h"

char* alloc_buffer(FILE* source, int* buffer_size)
{
    if(source == nullptr) {
        printf("Error: nullptr FILE pointer in alloc_buffer\n");
        return nullptr;
    }

    fseek(source, 0, SEEK_END);
	int file_size = ftell(source);
	fseek(source, 0, SEEK_SET);

	char* buffer = (char*)calloc(file_size + 1, sizeof(char));

    if(!buffer_size) {
        *buffer_size = file_size;
    }

	return buffer;
}

int InputString(char *buffer, int buffer_size) 
{
	char character = 0;
	int iter_count = 0;
	
	while((character = getchar()) != '\n') 
	{
		buffer[iter_count] = character;
		iter_count++;
		if(iter_count >= buffer_size - 1) {
			printf("Warning: buffer overflow\n");
			return EOF;
		}
	} 
	buffer[iter_count] = '\0';
	return 0;
}

int WordLength(const char* expression)
{
    int word_length = 0;
    while(!isspace(*expression) && *expression != '\0' && *expression != '(' && *expression != ')') {
        expression++;
        ++word_length;
    }
    return word_length;
}
