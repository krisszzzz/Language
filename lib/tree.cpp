#include "tree.h"
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

inline int max(int a, int b)
{
	return (a <= b) ? b : a;
}

int AllocMemoryForNodes(Tree_node *tree_node)
{
	if(tree_node->child_nodes == nullptr) { // Not allocated memory for child nodes
		tree_node->child_nodes 	   = (Tree_node*)calloc(MIN_VERTEX_DEGREE, sizeof(Tree_node));

		RET_IF(tree_node->child_nodes == nullptr, MEM_ALLOC_ERROR, "Memory allocation error\n");

		tree_node->node_degree     = 0;
		tree_node->max_node_degree = MIN_VERTEX_DEGREE;
	}
	return 0;
}

#define SIMPLE_NODE_TYPE_ADD(type_code, type_name) 	  		 \
case type_code: {								      		 \
	to_fill->data.type_##type_name = *(type_name*)node_data; \
	to_fill->type_id          	   = type_code;		     	 \
	break;										  			 \
}

int FillData(Tree_node *to_fill, const int node_type, const void *node_data)
{

	RET_IF(to_fill == nullptr, DATA_FILL_IN_ERROR, "can not work with nullptr object\n");

	switch(node_type)
	{
		case STRING: {												// string cannot be added by macros, because need to alloc memory
			int to_alloc              = strlen((char*)node_data);
			to_fill->data.type_string = (char*)calloc(to_alloc + 1, sizeof(char));
			to_fill->type_id          = STRING;
			memcpy(to_fill->data.type_string, node_data, to_alloc);
			break;
		}
		SIMPLE_NODE_TYPE_ADD(INT, 	 int);
		SIMPLE_NODE_TYPE_ADD(DOUBLE, double);
		SIMPLE_NODE_TYPE_ADD(CHAR,   char);
		default: {
			ErrorPrint("Unknown type name: %s", node_type);
			to_fill->type_id = -1;
			return DATA_FILL_IN_ERROR;
		}
	}
	to_fill->is_filled = 1;
	return 0;
}

#undef SIMPLE_NODE_TYPE_ADD

int CheckNodeType(int node_type)
{
	switch(node_type) {
		case INT:
		case STRING:
		case DOUBLE:
		case CHAR: {
				 return 0;
		}
		default: {
			return INCCORECT_NODE_TYPE;
		}
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Tree constructor

int TreeCtor(Tree *tree, const int num_of_branches, const int node_type, const void *node_data)
{
    RET_IF(tree      == nullptr, NULLPTR_TREE, 				"Cannot construct nullptr tree\n");
	RET_IF(num_of_branches <= 0, INCCORECT_NUM_OF_BRANCHES, "Numbers of branches should be positive, current number is %d\n",
															 num_of_branches);

	RET_IF(CheckNodeType(node_type) == INCCORECT_NODE_TYPE, INCCORECT_NODE_TYPE, "Inccorect node type, node type id is %d\n", node_type);

	tree->root_node.child_nodes = (Tree_node*)calloc(max(MIN_VERTEX_DEGREE, num_of_branches), sizeof(Tree_node));

	RET_IF(tree->root_node.child_nodes == nullptr, 	   			  MEM_ALLOC_ERROR,    "Inccorect memory allocation\n");
	RET_IF(FillData(&tree->root_node, node_type, node_data) != 0, DATA_FILL_IN_ERROR, "Inccorect node type\n");

	tree->root_node.parent = &tree->root_node;

	for(int node_count = 0; node_count < num_of_branches; ++node_count)
	{
		tree->root_node.child_nodes[node_count].parent = &tree->root_node;
	}

	tree->root_node.max_node_degree = max(MIN_VERTEX_DEGREE, num_of_branches);
	tree->root_node.node_degree		= 0;
	tree->num_of_branches 			= num_of_branches;

	return 0;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Adding new nodes

#define IS_FILLED_CHILD(node_count)	          \
tree_node->child_nodes[node_count].is_filled


int AddNode(Tree_node *tree_node, const int node_type, const void *data)
{
	RET_IF(tree_node          == nullptr, NULLPTR_NODE,      "Can not add node to nullptr vertex\n");
	//RET_IF(tree_node->parent  == nullptr, NULLPTR_PARENT,    "Can not add node, because current node have a nullptr root node\n");

	RET_IF(CheckNodeType(node_type) == INCCORECT_NODE_TYPE, INCCORECT_NODE_TYPE, "Inccorect node type, "
																				 "node type id is %d\n",
																				  node_type);
	RET_IF(AllocMemoryForNodes(tree_node) != 0, MEM_ALLOC_ERROR, "Memory allocation error\n");
	int node_count = 0;

	for(; node_count < tree_node->max_node_degree; ++node_count)  // Find empty element
	{
		if(!IS_FILLED_CHILD(node_count)) {
			break;
		}
	}
	if(node_count == tree_node->max_node_degree) {
		tree_node->child_nodes = (Tree_node*)realloc(tree_node->child_nodes, 2 * tree_node->max_node_degree);
	}

	RET_IF(FillData(&tree_node->child_nodes[node_count], node_type, data) != 0, DATA_FILL_IN_ERROR, "Inccorect node type\n");

	tree_node->child_nodes[node_count].type_id   = node_type;
	tree_node->child_nodes[node_count].is_filled = 1;
	tree_node->child_nodes[node_count].parent    = tree_node;
	tree_node->node_degree++;

	return 0;
}

int ConnectNodes(Tree_node *parent_node, const Tree_node *child_node)
{
	RET_IF(parent_node == nullptr, NULLPTR_NODE, "Nullptr parent node, connection of nodes failed\n");
	RET_IF(child_node  == nullptr, NULLPTR_NODE, "Nullptr child node,  connection of nodes failed\n");
	int node_degree = parent_node->node_degree;
	if(parent_node->child_nodes == nullptr) {
		RET_IF(AllocMemoryForNodes(parent_node) != 0, MEM_ALLOC_ERROR, "Memory allocation error\n");  // Rework for reallocation
	}
	parent_node->child_nodes[node_degree] 		    = *child_node;
	parent_node->child_nodes[node_degree].parent    = parent_node;
    parent_node->child_nodes[node_degree].is_filled = 1;
    //parent_node->is_filled = 1;

    if(child_node->is_filled && child_node->type_id == STRING) {
        FillData(&parent_node->child_nodes[node_degree], STRING, child_node->data.type_string);  // Copy string if it is needed
    }
	parent_node->node_degree++;
	return 0;
	
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Graphical dump part

#define NO_NODES_EXPECT_ROOT_NODE         \
!root_node->child_nodes[0].is_filled

int DefineAndPrintColor(Tree_node *node, FILE *dst)
{
	switch(node->type_id)
	{
		case INT: {
			fprintf(dst, "#00BFFF");
			break;
		}
		case CHAR: {
			fprintf(dst, "#FFFF00");
			break;
		}
		case STRING: {
			fprintf(dst, "#00FF00");
			break;
		}
		case DOUBLE: {
			fprintf(dst, "#800080");
			break;
		}
		default: {
			ErrorPrint("No such type id - %d", node->type_id);
		}
	}	
	return 0;
}

static int PrintNodeToDump(Tree_node* root_node, Tree_node *node, FILE* dst)
{
										// This node is root nood and no noodes exclude him
	if(node->child_nodes == nullptr || (node == root_node && NO_NODES_EXPECT_ROOT_NODE)) {
		fprintf(dst, "dot_%p;\n", node);
		fprintf(dst, "dot_%p[shape = record, style=\"filled\", fontname = Helvetica, fillcolor=\"", node);
		DefineAndPrintColor(node, dst);
		fprintf(dst, "\", label = \"value: | ");
		RET_IF(FprintNodeData(node, dst) == INCCORECT_NODE_TYPE, INCCORECT_NODE_TYPE, "Unknown name of type\n");
		fprintf(dst, "\" ]\n");
		return 0;
	}
	
	for(int node_number = 0; node_number < node->node_degree; ++node_number)
	{
		fprintf(dst, "dot_%p->", node);
		PrintNodeToDump(root_node, &node->child_nodes[node_number], dst);	

		fprintf(dst, "dot_%p[shape = record, fontname = Helvetica, style=\"filled\", fillcolor=\"", node);
		DefineAndPrintColor(node, dst);
		fprintf(dst, "\", label = \"value: | ");
		RET_IF(FprintNodeData(node, dst) == INCCORECT_NODE_TYPE, INCCORECT_NODE_TYPE, "Unknown name of type\n");
		fprintf(dst, "\" ]\n");	
	}
	return 0;	
}

int TreeGraphicalDump_(Tree_node* root_node, const char *tree_name)
{
	const int Buffer_Size    = 128;
	char buffer[Buffer_Size] = {};
	RET_IF(root_node == nullptr, NULLPTR_TREE, "Can not dump the nullptr tree\n");
	RET_IF(tree_name == nullptr, NULLPTR_TREE, "Can not dump the tree, because of nullptr tree name\n");

	FILE *to_gener_graphviz_code_for_debug = fopen(tree_name, "w");
	RET_IF(to_gener_graphviz_code_for_debug == nullptr, INCCORECT_FILE_OPEN, "Something went wrong while opening "
																			 "file to dump tree, tree name - %c\n",
 		 																	  tree_name);

	fprintf(to_gener_graphviz_code_for_debug, "digraph G {\n"
											  "rankdir = NB\n");

	PrintNodeToDump(root_node, root_node, to_gener_graphviz_code_for_debug);
	fprintf(to_gener_graphviz_code_for_debug, " }\n");
	
	sprintf(buffer, "dot -Tpng %s -o %s.png\n", tree_name, tree_name);
	printf(buffer);
	system(buffer);
	fclose(to_gener_graphviz_code_for_debug);
	return 0;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


#define ADD_TYPE_TO_PRINT(type_code, type_name, specifier)		\
case type_code: {												\
	fprintf(dst, specifier, node->data.type_##type_name);		\
	break;														\
}
// Print node data

int FprintNodeData(Tree_node *node, FILE *dst)
{
	RET_IF(node == nullptr, NULLPTR_NODE, "Cannot work with nullptr node\n");
	switch(node->type_id)
	{
		ADD_TYPE_TO_PRINT(INT,    int, 	  "%d");
		ADD_TYPE_TO_PRINT(STRING, string, "%s");
		ADD_TYPE_TO_PRINT(DOUBLE, double, "%lf");
		ADD_TYPE_TO_PRINT(CHAR,   char,   "%c");
		default: {
			ErrorPrint("Unknown name of type\n");
			return INCCORECT_NODE_TYPE;
		}
	}
	return 0;
	
}

int CpyTree(Tree_node* dst, Tree_node* src)
{
	for(int iter_count = 0; iter_count < src->node_degree; ++iter_count)
	{
		ConnectNodes(dst, &src->child_nodes[iter_count]);
		CpyTree(&dst->child_nodes[iter_count], &src->child_nodes[iter_count]);	
	}
	return 0;
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Destuctor part

static int DestroyNodesData(Tree_node* root_node, Tree_node *node)
{
	if(node == root_node) {
		free(node->data.type_string);
	}
	if(node->child_nodes == nullptr) {
		return 0;
	}
	
	for(int node_number = 0; node_number < node->node_degree; ++node_number)
	{
		if(node->child_nodes[node_number].type_id == STRING) {
			free(node->child_nodes[node_number].data.type_string);	
		}

		DestroyNodesData(root_node, &node->child_nodes[node_number]);	
	}
	return 0;
}


static int RecursivelyDestroyNodes(Tree_node* root_node, Tree_node *node)
 {
	int child_nodes_without_next_child_nodes = 0;

	for(int node_count = 0; node_count <= node->node_degree; ++node_count) 
	{
		if(node_count == node->node_degree) {
			free(node->child_nodes);
			node->child_nodes = nullptr;
			node->node_degree = 0;
			if(node->parent == root_node) {
				return 0;
			}
			break;
		}

		if(node->child_nodes[node_count].child_nodes != nullptr) {
			RecursivelyDestroyNodes(root_node, &node->child_nodes[node_count]);
		}
	}

	return 0;
}

int TreeDtor(Tree_node* root_node)
{
	RET_IF(root_node == nullptr, NULLPTR_TREE, "Can not destruct nullptr tree\n");
	DestroyNodesData(root_node, root_node); // Destroy all nodes data, if data is string type
	RecursivelyDestroyNodes(root_node, root_node); // Destroy all nodes (exclude root child nodes)
	free(root_node->child_nodes);
	
	return 0;
}
