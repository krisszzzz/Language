#include <string.h>
#include <stdlib.h>
#include "asm.h"
#include "../Proccessor_lib/murmurhash.h"
#undef binary_file


#define MAX_CMD_SIZE 	 32    /// Define the max command length
#define LABELS_MAX_COUNT 10
#define REG_NAME_LENGTH  2

const int Bin_change_cmd_code = 0x70;
const int Vmem_address_begin =  1000 - 7;

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// Labels

struct label
{
	int ip;
	int label;
};

static label* LabelsInit()
{
	label* labels = (label*)calloc(LABELS_MAX_COUNT, sizeof(label));
	for(int label_index = 0; label_index < LABELS_MAX_COUNT; ++label_index)
	{
		//Labels[iter_count].label =  -1;
		labels[label_index].ip    =  -1;
	}
	return labels;
}

static int AddLabel(label* labels, int label, int ip)
{
	static int label_index = 0;
	if(label_index >= LABELS_MAX_COUNT) {
		ErrorPrint("Error, labels overflow\n");
		return -1;
	} 
	labels[label_index].label = label;
	labels[label_index].ip	  = ip;
	++label_index;

	return 0;
}

static int GetLabelIndex(label* labels, int label)
{
	for(int label_index = 0; label_index < LABELS_MAX_COUNT; ++label_index)
	{
		if(labels[label_index].label == label) {
			return label_index;
		}
	}
	return 0;
}

/*
static int IsLabelsUnused(label* labels)
{
	for(int label_index = 0; label_index < LABELS_MAX_COUNT; ++label_index)
	{
		if(labels[label_index].ip != -1) {
			return 0;
		}
	}
			  /// Return 0 if labels unused, and return 1 if used
	return 1;
}
*/


/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// For graphics
const char All_pixel_types[]   = "%.:;~=#OB8";
const int  All_pixel_types_num = 10; 

static int DefinePixelTypeCode(const char pixel_type)
{
    for(int pixel_code = 0; pixel_code < All_pixel_types_num; pixel_code++)
    {
        if(pixel_type == All_pixel_types[pixel_code])
            return pixel_code;
    }
    return -1;
}

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// checkers


#define DEF_CMD(cmd_code, ...)                        \
case cmd_code: ;

static void CollisionChecker()
{
    const int check_code_collision = 0;

    switch(check_code_collision) {
    #include "../Proccessor_Lib/commands.h"
	default: 
	;
	/// This function "useless" for user, it's only to check, that code of commands don't collide 
    }
}
#define REG_CHECK(arg, reg, reg_code)                 \
if(strncmp(arg, #reg, 2) == 0)						  \
return reg_code;

static int RegisterChecker(const char* arg)
{
	REG_CHECK(arg, ax, 1);
	REG_CHECK(arg, bx, 2);
	REG_CHECK(arg, cx, 3);
	REG_CHECK(arg, dx, 4);

	return 0;
}

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// asm_file functions

int AsmFileCtor(asm_file* to_ctor, const char* source_name)
{
	asm_file null_object = {0};

	if(memcmp(&null_object, to_ctor, sizeof(asm_file)) != 0) {
		ErrorPrint("Inccorect construction: you give unitialized "
						   "or already initialized object\n");
		return ERROR_CONSTRUCTION;						   
	}

	if(to_ctor == nullptr) {
		ErrorPrint("Inccorect construction: you give nullptr argument\n");
		return ERROR_CONSTRUCTION;
	}
	if(source_name == nullptr) {
		ErrorPrint("You give for function inccorect pointer (name of file) << he is nullptr\n");
		return ERROR_CONSTRUCTION;
	}
	
	if((to_ctor->src_file = fopen(source_name, "rb")) == nullptr) {
		ErrorPrint("Something went wrong while opening the file");
		PrintToLog        (" << file name is - %s\n", source_name);
		return ERROR_CONSTRUCTION;
	}

	struct stat src_file_info = {};
	if(stat(source_name, &src_file_info) != 0) {
		ErrorPrint("Something went wrong, the program can't take info about your file");
		PrintToLog        (" << file name is - %s\n");
		return ERROR_CONSTRUCTION;
	}
	to_ctor->file_name 		      = source_name;
	to_ctor->info                 = src_file_info;
	to_ctor->strings              = nullptr;
	to_ctor->strings_num          = 0;
	to_ctor->is_initialized       = 1;
	char* file_ready_name 		  = strdup(source_name);
	file_ready_name	 			  = strcat(file_ready_name, ".bin");
	struct stat binary_file_info  = {};
	stat(file_ready_name, &binary_file_info);

	if(to_ctor->info.st_mtime < binary_file_info.st_mtime)	{
		PrintToLog("Already compilated file: file name --> %s\n", source_name);
		PrintToLog("source modification time: %ld, last Asm time: %ld\n", to_ctor->info.st_mtime,
					binary_file_info.st_mtime);
		return ALREADY_ASSEMBLED;
	}

	FILE* open_binary_file        = fopen(file_ready_name, "wb");
	to_ctor->binary_file		  = calloc(1, sizeof(binary_file)); 
	if(to_ctor->binary_file == nullptr) {
		ErrorPrint("Something went wrong, inccorect memory allocation\n");
	}
	if(open_binary_file == nullptr) {
		ErrorPrint("Something went wrong while opening your binary file\n");
	}

	(*(binary_file*)to_ctor->binary_file)  = { 
												open_binary_file,						/// bin_file
												binary_file_info,						/// info
												file_ready_name,						/// file_name
												ON_DEBUG(0)								/// hash
											};		
	ON_DEBUG(									
	(*(binary_file*)to_ctor->binary_file).hash = MurmurHash((char*)(to_ctor->binary_file), 
															sizeof(binary_file) - sizeof(hash_t));
	to_ctor->hash                 			   = MurmurHash((const char*)to_ctor, 
															sizeof(asm_file) - sizeof(hash_t));
	)
	return 0;
}


void MakeStringsFromAsmFile(asm_file* source)
{
	if(source == nullptr) {
		ErrorPrint("Nullptr asm_file\n");
		return;
	}

	if(ON_DEBUG(!IsAsmFileHashUnchanged(source) || ) source->is_initialized == 0) {
		return;
	}
	
	char*  buffer         = MakeBufferForFile(source->src_file);
	size_t num_of_strings = 0;

	ReadToBuffer(source->src_file, &buffer, &num_of_strings);
	string* strings = (string*)calloc(num_of_strings, sizeof(string));
	if(strings == nullptr) {
		ErrorPrint("Failed memory allocation\n");
	}

	MakeStrings(buffer, strings, num_of_strings);

	source->strings 	  = strings;
	source->strings_num   = num_of_strings;
	ON_DEBUG(
		source->hash      = MurmurHash((const char*)source, sizeof(asm_file) - sizeof(hash_t));
	)
}

void AsmFileDtor(asm_file* to_destruct)
{
	if(to_destruct == nullptr) {
		ErrorPrint("Nullptr asm_file\n");
		return;
	}
	ON_DEBUG(
		if(!IsAsmFileHashUnchanged(to_destruct)) {
			return;
		}
	)
	if(to_destruct->is_initialized != 1) {
		ErrorPrint("Unitialized asm_file\n");
		return;
	}
	if(to_destruct->binary_file == nullptr) {
		ErrorPrint("Binary file is nullptr\n");
		return;
	}

	fclose((*(binary_file*)to_destruct->binary_file).bin_file);
	fclose(to_destruct->src_file);
	free(to_destruct->binary_file);
	asm_file null_object = {0};
	*to_destruct 		 = null_object;
}

ON_DEBUG(
int IsAsmFileHashUnchanged(asm_file* to_check)
{
	if(to_check == nullptr) {
		ErrorPrint("Nullptr asm_file\n");
		return 0;
	}

	if(to_check->hash != MurmurHash((const char*)to_check, sizeof(asm_file) - sizeof(hash_t))) {
		ErrorPrint("The hash of asm_file is changed\ncurrent hash - %llu, should be - %llu", 
						   to_check->hash, MurmurHash((const char*)to_check, sizeof(asm_file) - sizeof(hash_t)));
		return 0;
	}
	if(to_check->binary_file == nullptr) {
		ErrorPrint("Something went wrong, your binary file is nullptr\n");
		return 0;
	}
	if((*(binary_file*)to_check->binary_file).hash != MurmurHash((char*)to_check->binary_file, 
																 sizeof(binary_file) - sizeof(hash_t))) {
		ErrorPrint("The binary data was corrupted\n");
		return 0;
	}

	return 1;
}
)


#define DEF_CMD(cmd_code, cmd_name, _with_double_arg, _with_mem_treatment, _with_label_treatment,		  \
				_with_reg_treatment,  ...)   															  \
if(strncmp(cmd, #cmd_name, cmd_length) == 0) {          												  \
    *with_double_arg      = _with_double_arg;                             								  \
	*with_mem_treatment   = _with_mem_treatment;													      \
	*with_label_treatment = _with_label_treatment;														  \
	*with_reg_treatment   = _with_reg_treatment;														  \
    return cmd_code;                                    												  \
}                                                       	

static char CompareCommands(const char* cmd,                const int cmd_length,   	    int* with_double_arg, 
							int*        with_mem_treatment, int*      with_label_treatment, int* with_reg_treatment)
{
    #include "asm_commands.h"

    return ERROR_CMD;
}																

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// These functions used to simplify the code in the AssembleCmd() function
static int DefaultCmdProccessing(asm_file* to_Assemble,      char* cmd, 	         	 int*    cmd_code, 
								 double*   arg,   	         int*  with_double_arg,  	 int*    num_of_assemblations,
								 int*      command_length,   int*  ip,               	 size_t* iter_count);

static void LabelProccessing	(asm_file* to_Assemble,      int* cmd_code,          	 label*  labels,
								 int*  	   label_code,       int* ip, 				 	 int* 	 num_of_assemblations);	 

static int  RegProccessing  	(asm_file* to_Assemble,   	 char* cmd,  	   		 	 int* 	 cmd_code, 
			   			 	 	 size_t*   iter_count,     	 int*  ip,       		 	 int* 	 reg_code, 
					 		 	 char* 	   reg_name,   	   	 int* num_of_assemblations);

static int MemProccessing   	(asm_file* to_Assemble,      char* cmd,                  int*    cmd_code,    
								 size_t*   iter_count,       int*  ip,                   int*    num_of_assemblations, 
								 int*  	   mem_address, 	 int*  command_length,       int* 	 second_arg_begin);

/// -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/// This is function needed to start Asm (assemblation)

int AssembleCmd(asm_file* to_Assemble)
{
	static int num_of_assemblations = 0;
	++num_of_assemblations;

    if(to_Assemble == nullptr) {
        ErrorPrint("Inccorect argument - is nullptr\n");
        return ASM_ERROR;
    }
    if(to_Assemble->is_initialized == 0) {
        ErrorPrint("Unitialized compilition_file object\n");
        return ASM_ERROR;
    }
    if(to_Assemble->strings_num == 0 || to_Assemble->strings == nullptr) {
        ErrorPrint("You source file is empty, or probably you forgot "
						   "to use MakeStringsForAsmFile() function\n");
        return ASM_ERROR;
    }

    ON_DEBUG(
        if(!IsAsmFileHashUnchanged(to_Assemble))
            return ASM_ERROR;
	)

	
	int 		  ip 				    = 0;
	char    	  cmd[MAX_CMD_SIZE + 1] = {0};
    int     	  cmd_code              = 0;
    int     	  command_length        = 0;
    double  	  arg           	    = 0; 
    int     	  mem_address           = 0;
    int     	  with_double_arg       = 0;
    int     	  with_mem_treatment    = 0;
	int     	  with_reg_treatment    = 0;
	int    	      with_label_treatment  = 0;
    int     	  second_arg_begin      = 0;
	int     	  reg_code 	            = 0;		// NORMAL REGISTER SIZE IS 2
	char*   	  reg_name     	        = (char*)calloc(REG_NAME_LENGTH + 1, sizeof(char));
	int     	  label_code	        = 0;
	static label* labels		        = nullptr;

	if(num_of_assemblations == 1) {
		labels = LabelsInit();
	}

	for(size_t iter_count = 0; iter_count < to_Assemble->strings_num; ++iter_count)
    {
		///Working with comments
		
		char* comment_section_begin = strchr(to_Assemble->strings[iter_count].line, ';');
		if(comment_section_begin != nullptr) {
			*comment_section_begin = '\0';
		}
		
        if(*to_Assemble->strings[iter_count].line == '\0') {
			continue;
		}
		

		/// Check for labels
		if(sscanf(to_Assemble->strings[iter_count].line, ":%d", &label_code) == 1) {
			if(num_of_assemblations == 1) {
				AddLabel(labels, label_code, ip);
			}
			continue;
		}
		/// Get cmd  
		sscanf(to_Assemble->strings[iter_count].line, "%s %n", cmd, &command_length);

		if(command_length >= MAX_CMD_SIZE) {
			ErrorPrint("The command too long, don't use the commands, that longer than %d\n",
							   command_length);
			return ASM_ERROR;
		}
		cmd_code = CompareCommands(cmd, command_length, &with_double_arg,      &with_mem_treatment, 
														&with_label_treatment, &with_reg_treatment);

		/// Check cmd 
		if(cmd_code == ERROR_CMD) {
			ErrorPrint("Inccorect command, please see the instruction, where all commands showed\n"
							   "Your input: %s, line number %llu\n", cmd, iter_count + 1);
            return ASM_ERROR;
		}
		/// Format, when you have to use RAM or Videomemory
		/// Working with RAM and Video memory
        if( with_mem_treatment && 
			sscanf(to_Assemble->strings[iter_count].line + command_length, "[%d] %n", &mem_address, &second_arg_begin) == 1
				   || sscanf(to_Assemble->strings[iter_count].line + command_length, "[%[a-dx]]") == 1 
				   || sscanf(to_Assemble->strings[iter_count].line + command_length, "[%[a-dx] %[+-] %d]")) { 		

			if(MemProccessing(to_Assemble, cmd, &cmd_code, &iter_count, &num_of_assemblations, &mem_address,
							  &ip, &command_length, &second_arg_begin) == ASM_ERROR) {
				return ASM_ERROR;
			}							   
			continue;
		/// Format when you have to use registers
		/// Working with registers		 
        } else if( with_reg_treatment &&
				   sscanf(to_Assemble->strings[iter_count].line + command_length, "%2s",  reg_name) == 1 &&
				  (sscanf(to_Assemble->strings[iter_count].line + command_length, "%lg",   &arg) 	== 0 &&
				   sscanf(to_Assemble->strings[iter_count].line + command_length, ":%lg",  &arg) 	== 0) ) {
			  
			if(RegProccessing (to_Assemble, cmd, 	   &cmd_code, &iter_count, &ip,
				    		   &reg_code,   reg_name, &num_of_assemblations) == ASM_ERROR) {
				return ASM_ERROR;				   	
			}

			continue;
  		  /// ja, jae, jb, jbe je, jne, jmp format
		  /// Work with labels				
		} else if(with_label_treatment && sscanf(to_Assemble->strings[iter_count].line + command_length, 
												 ":%d", &label_code) == 1) {  
																				
			LabelProccessing(to_Assemble, &cmd_code, labels, &label_code, &ip, &num_of_assemblations);
			continue;
		} 
		/// Default (without arguments) commands proccessing
		DefaultCmdProccessing(to_Assemble, cmd, &cmd_code, &arg, &with_double_arg,
							  &num_of_assemblations, &command_length, &ip,
							  &iter_count);
    }

	if(num_of_assemblations == 1) {
		AssembleCmd(to_Assemble);
	}
	
    return 0;
}

static int MemProccessing (asm_file* to_Assemble,     char* cmd,         int* cmd_code, size_t* iter_count,
						   int* num_of_assemblations, int*  mem_address, int* ip,       int*    command_length,
						   int* second_arg_begin) 
{	
	/// using binary operations, to label the new code, if push/pop used with RAM treatment
	/// Only for video (if you want use the videomemory you should pass the address above or equal to 1000 - 7):
	/// If you didn't pass argument, or you pass inccorect
	/// argument, the argument automatically will be '.'				

	*cmd_code |= Bin_change_cmd_code;           																						 
	if(*num_of_assemblations == 2) {																						 
    	fwrite(cmd_code,    sizeof(int),  1, (*(binary_file*)to_Assemble->binary_file).bin_file);						 
	}																													 
	*ip += 1;


	if(*mem_address < 0) {																								 
		ErrorPrint("Inccorect address\n" 
		                   "Line:%llu command:%s\n", *iter_count + 1, cmd);								 
		return ASM_ERROR;																								 
	}																													 
	if(*num_of_assemblations == 2) {																					     
    	fwrite(mem_address, sizeof(int),   1, (*(binary_file*)to_Assemble->binary_file).bin_file);						 
	}																													 
	*ip += 1;																									 
	if(*mem_address >= Vmem_address_begin) {																	 					 
																													 
		char pixel_type_temp  = 0;				/// Using this variable to avoid warning in GCC -Wformat, because if we use 
												/// int type variable we should use %d, but in this case we should use %c																			 
		if(sscanf(to_Assemble->strings[*iter_count].line + *command_length + *second_arg_begin, 							 
				  "%c", &pixel_type_temp) == 1) { 																			 
																													 
			if(DefinePixelTypeCode(pixel_type_temp) != -1) {																		 
			
				if(*num_of_assemblations == 2) {																				 
					int pixel_type = pixel_type_temp;
	    			fwrite(&pixel_type, sizeof(int), 1, (*(binary_file*)to_Assemble->binary_file).bin_file);                 
				}																											
				*ip += 1;																							
			} else {																									
				ErrorPrint("Inccorect video type object\nLine:%llu command:%s\n", *iter_count + 1, cmd);				 \
				return ASM_ERROR;																				         
			}																											 
																													 
		} else {																										 
    		int by_default_pixel_type = '.';															        		 
			if(*num_of_assemblations == 2) {																				 
        		fwrite(&by_default_pixel_type, sizeof(int), 1, 														     
					   (*(binary_file*)to_Assemble->binary_file).bin_file); 	 										 
			}																											 
			*ip += 1;
		} 																												 
	}																													 
	return 0;
}

static int  RegProccessing (asm_file* to_Assemble,    char* cmd,  	   int* cmd_code, 
			   			 	size_t*   iter_count,     int*  ip,        int*  reg_code, 
					 		char* 	  reg_name,   	  int* num_of_assemblations) {																				
	if((*reg_code = RegisterChecker(reg_name)) != 0) {														
		*cmd_code |= 0x60;           																		
		if(*num_of_assemblations == 2) {																		
			fwrite(cmd_code,    sizeof(int),  1, (*(binary_file*)to_Assemble->binary_file).bin_file);	
			fwrite(reg_code,    sizeof(int),  1, (*(binary_file*)to_Assemble->binary_file).bin_file);		
		}

	} else {																								
		ErrorPrint("Inccorect register name\nLine:%llu command:%s\n", *iter_count + 1, cmd);	
		return ASM_ERROR;	    
	}						
	*ip += 2;																				
	return 0;
}																				

static void LabelProccessing(asm_file* to_Assemble, int* cmd_code, label* labels,  
							 int* 	   label_code,  int* ip, 	   int*   num_of_assemblations)	 
{
	//Rework to check if is no labels
	int index = GetLabelIndex(labels, *label_code);																		
	if(*num_of_assemblations == 2) {																			
		fwrite(cmd_code, 		  sizeof(int), 1,  (*(binary_file*)to_Assemble->binary_file).bin_file);		
		fwrite(&labels[index].ip, sizeof(int), 1,  (*(binary_file*)to_Assemble->binary_file).bin_file);		
	}																										

	*ip += 2;
}																		

static int DefaultCmdProccessing(asm_file* to_Assemble, char* cmd, 	            int*    cmd_code, 
								 double* arg,   	    int*  with_double_arg,  int*    num_of_assemblations,
								 int* command_length,   int*  ip,               size_t* iter_count)												
{
	if(*num_of_assemblations == 2) {    
        	fwrite(cmd_code,   sizeof(int),  1, (*(binary_file*)to_Assemble->binary_file).bin_file);
		}
	*ip += 1;

    if(*with_double_arg) {
        if(sscanf(to_Assemble->strings[*iter_count].line + *command_length, "%lg", arg) <= 0) {

            ErrorPrint("Inccorect argument\n"
							   "Line number - %llu, command - %s\n", *iter_count + 1, cmd);
            return ASM_ERROR;

        } else {
			if(*num_of_assemblations == 2) {
                fwrite(arg, sizeof(double), 1, (*(binary_file*)to_Assemble->binary_file).bin_file);
			}
			*ip += 2;
        }
    }
	return 0;
}