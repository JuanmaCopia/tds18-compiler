#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define PLUS '+'
#define MINUS '-'
#define PROD '*'
#define DIV '/'
#define MOD '%'

#define ASSIGN '='
#define EQUALS 'e'

#define GREATER_THAN '>'
#define LESSER_THAN '<'

#define AND '&'
#define OR '|'

#define LABEL 'l'
#define CALL 'c'
#define BEGIN_FUN 'b' + 'f'
#define END_FUN 'e' + 'f'
#define PUSH 'p' + 'u'
#define POP 'p' + 'o'

#define PSEUDO 'p'

#define JMP 'j'
#define JE  'j' + 'e'
#define JNE 'j' + 'n' + 'e'
#define CMP 'c' + 'm'
#define RETURN 'r'
#define NEGAT 'n' + 'g'
#define BREAK 'B'

#define PUSH1 'P'
#define PUSH2 'P' + 1
#define PUSH3 'P' + 2
#define PUSH4 'P' + 3
#define PUSH5 'P' + 4
#define PUSH6 'P' + 5

// return types of functions
typedef enum return_types {
	_boolean,
	_integer,
	_void
} ReturnType;

// possible types of ASTNodes
typedef enum type_of_node {
	_if,
	_if_body,
	_while,
	_arith_op,
	_boolean_op,
	_assign,
	_return,
	_method_call,
	_id,
	_literal
} TypeNode;

// return types of functions
typedef enum varnode_kind {
	_global,
	_local,
	_parameter,
	_temporal,
	_label,
} VarNodeKind;

// Struct that holds variables data
typedef struct var_struct {
	char *id;
	int value;
	bool is_boolean;
	bool is_defined;
	VarNodeKind kind;
	int offset;
	char * string_offset;
	struct var_struct *next;
} VarNode;

// Struct that holds parameter information
typedef struct parameter_struct {
	char *id;
	int value; // is this really needed?
	bool is_boolean;
	int offset;
	struct parameter_struct *next;
} Parameter;

struct functions_struct;
struct ast_node_struct;

// Function Node: Represents a function of the program
typedef struct functions_struct {
	char *id;
	ReturnType type;
	Parameter *parameters;
	int param_amount;
	VarNode *enviroment;
	bool is_extern;
	int max_offset;
	struct ast_node_struct *body;
	struct functions_struct *next;
} FunctionNode;

// Node of the AST
typedef struct ast_node_struct {
	int data;
	bool is_boolean;
	TypeNode node_type;
	int line_num;
	int col_num;
	VarNode *var_data;
	struct functions_struct *function_data;
	struct ast_node_struct *next_statement;
	struct ast_node_struct *left_child;
	struct ast_node_struct *right_child;
} ASTNode;

// Envitoment list, it holds all the head pointers of the diferent enviroment levels.
typedef struct enviroment_stack {
	VarNode *variables;
	struct enviroment_stack *next;
} EnviromentNode;

// Node of the intermidiate code list;
typedef struct instruction_node {
	int operation;
	VarNode * op1;
	VarNode * op2;
	VarNode * result;
	struct instruction_node * next;
	struct instruction_node * back;
} InstructionNode;


int get_operation(ASTNode * node);
VarNode * create_VarNode(char * id, int value, bool is_boolean);
VarNode * create_var_node();
VarNode * create_temporal_with_id(char * id);
InstructionNode * create_instructionNode(int operation, VarNode * result, VarNode * op1, VarNode * op2);
InstructionNode * create_instruction_from_ASTNode(ASTNode * root);
ASTNode * get_next_statement(ASTNode * statement);
char * get_temporal_string(VarNode * temp);
char * get_operation_string(InstructionNode * i);
VarNode * get_var(ASTNode * node);
InstructionNode * create_instruction(int operation);
VarNode * create_label();
VarNode * create_label_with_id(char * id);
TypeNode get_node_type(int op);
ReturnType get_return_type(int type_int_value);
char * get_return_type_string(ReturnType value);
char * get_type_node_string(TypeNode tn);
char * get_string_representation(ASTNode * node);
bool is_boolean_operation(int op);
VarNode * create_varnode_from_param(Parameter * param);
char * get_varnode_kind_string(VarNode * var);
char * create_string_offset(int offset);
char * create_global_string_offset(char * var_id);
void free_varnode_memory(VarNode * v);
void free_parameter_memory(Parameter * p);
void free_astnode_memory(ASTNode * ast);
void free_function_memory(FunctionNode * f);
void free_instruction_memory(InstructionNode * i);
void print_instruction(InstructionNode * i);
void print_varnode(VarNode * var);