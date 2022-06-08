#ifndef TREE_INCLUDED

#define TREE_INCLUDED

#include <stdio.h>
#include <string.h>
#include "log.h"
#define BINARY_TREE
#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(code) code
#else
#define ON_DEBUG(...)  
#endif

#define MIN_VERTEX_DEGREE  256

typedef char* string;

#define RET_IF(condition, ret_val, ...)    \
if(condition) {							   \
	ErrorPrint(__VA_ARGS__);			   \
	return ret_val;						   \
}

#define ERROR_IF(condition, ret_val, ...)   \
if(condition) {							   \
	ErrorPrint(__VA_ARGS__);               \
	*error_code = ret_val;				   \
	return ret_val;						   \
}

enum TYPE_ID
{
	INT,
	DOUBLE,
	STRING,
	CHAR
};

const int OPERATOR = CHAR;

enum TREE_ERRORS
{
    NULLPTR_TREE = -(1000 - 7),
	NULLPTR_NODE,
	NULLPTR_NODE_TYPE,
	NULLPTR_PARENT,
    INCCORECT_NUM_OF_BRANCHES,
	MEM_ALLOC_ERROR,
	INCCORECT_NODE_COUNT,
	INCCORECT_FILE_OPEN,
	INCCORECT_NODE_TYPE,
	DATA_FILL_IN_ERROR,
	EMPTY_NODE,
	CONNECTION_ERROR
};

struct Tree_node
{
    int        node_degree;
	int        max_node_degree;
	int		   is_filled;
	int        type_id;
	union
	{
		char  *type_string;
		int    type_int;
		char   type_char;
		double type_double;
	} data;

	Tree_node *parent;
    Tree_node *child_nodes;
};

struct Tree
{
    Tree_node  root_node;
    int    	   num_of_branches;
};

int FillData(Tree_node* to_fill, const int node_type, const void *node_data);

int CpyTree(Tree_node* dst, Tree_node* src);

int TreeCtor(Tree *tree, const int num_of_branches, const int node_type, const void *node_data);

#define PrintNodeData(node)	\
FprintNodeData(node, stdout);

int FprintNodeData(Tree_node *node, FILE *dst);

int AddNode(Tree_node *tree_node, const int node_type, const void *data);

int ConnectNodes(Tree_node *parent_node, const Tree_node *child_node);


#define TreeGraphicalDump(tree)  \
TreeGraphicalDump_(tree, #tree);

int TreeGraphicalDump_(Tree_node *root_node, const char *tree_name);

int TreeDtor(Tree_node* root_node);

int AllocMemoryForNodes(Tree_node *tree_node);

#endif
