%{
	
#include "intermediate_code.c"
#include "assembly_code.c"

VarNode * temporal_enviroment;                          // Holds the last closed enviroment
Parameter * temporal_parameter;                         // Holds the formal parameters of the current function
EnviromentNode * symbol_table = (EnviromentNode *) NULL; // Stack that contains all the open enviroment levels
FunctionNode * fun_list_head = (FunctionNode *) NULL;    // List of all the functions of the program

int amount_open_enviroments = 0;                        // Quantity of currently open enviroments
char * error_message;                                   // Stores an error message to disply
int current_local_offset = -8;                          // Holds the next offset for local variables to be used.
int current_parameter_offset = 16;                      // Holds the next offset for function parameters to be used.

void yyerror();
int yylex();
int get_line_number();
int get_column_number();
extern VarNode * create_VarNode(char * id, int value, bool is_boolean);

/*
	Increases the offset value for local variables.
*/
void increase_local_offset() {
	current_local_offset = current_local_offset - 8;
}

/*
	Increases the offset value for function parameters.
*/
void increase_parameter_offset() {
	current_parameter_offset = current_parameter_offset + 8;
}

/*
	Initializes the offsets values.
*/
void reset_offsets() {
	current_local_offset = -8;
	current_parameter_offset = 16;
}

/*
	Creates and adds a new variable to the current enviroment of the symbol table.
*/
void add_var_to_symbol_table(char * var_id, int value, bool is_boolean) {
	VarNode * new_var = create_VarNode(var_id, value, is_boolean);
	if (symbol_table -> variables != NULL)
		new_var -> next = symbol_table -> variables;
	symbol_table -> variables = new_var;
}

/*
	Returns a new ASTNode
*/
ASTNode * create_AST_node(ASTNode * left_child, char op, ASTNode * right_child) {
	ASTNode * new_node = (ASTNode *) malloc(sizeof(ASTNode));
	new_node -> data = op;
	new_node -> is_boolean = is_boolean_operation(op);
	new_node -> line_num = get_line_number();
	new_node -> col_num = get_column_number();
	new_node -> node_type = get_node_type(op);
	new_node -> var_data = NULL;
	new_node -> function_data = NULL;
	new_node -> left_child = left_child;
	new_node -> right_child = right_child;
	return new_node;
}

/*
	Returns a new leave created from a varNode
*/
ASTNode * create_AST_leave_from_VarNode(VarNode * var_data) {
	if (var_data == NULL) {
		printf("Cannot create leave from null var.\n");
		return NULL;
	}
	else {
		ASTNode * new_leave;
		if (var_data -> is_defined)
			new_leave = (ASTNode *) create_AST_node(NULL,'l',NULL);
		else
			new_leave = (ASTNode *) create_AST_node(NULL,'n',NULL);
		new_leave -> data = var_data -> value;
		new_leave -> node_type = _id;
		new_leave -> is_boolean = var_data -> is_boolean;
		new_leave -> var_data = var_data;
		return new_leave;
	}
}

/*
	Returns a new leave created from a value.
*/
ASTNode * create_AST_leave_from_value(int value, bool is_boolean) {
	ASTNode * new_leave = (ASTNode *) create_AST_node(NULL,'l',NULL);
	new_leave -> data = value;
	new_leave -> is_boolean = is_boolean;
	new_leave -> node_type = _literal;
	return new_leave;
}

/*
	Returns a new varNode with the ID taken as parameter.
*/
VarNode * create_var(char * var_id) {
	VarNode * new_var = create_VarNode(var_id, 0, false);
	if (amount_open_enviroments == 1) {
		new_var -> kind = _global;
		new_var -> string_offset = create_global_string_offset(new_var -> id);
		printf("string offset de global generado: %s\n", new_var -> string_offset);
	}
	else {
		new_var -> kind = _local;
		new_var -> offset = current_local_offset;
		new_var -> string_offset = create_string_offset(current_local_offset);
		printf("string offset de local generado: %s\n", new_var -> string_offset);
		increase_local_offset();
	}
	return new_var;
}

/*
	Appends a new parameter to a parameter's list.
*/
void add_new_parameter(Parameter * params_list_head, Parameter * to_add_param) {
	if (params_list_head == NULL)
		params_list_head = to_add_param;
	else {
		Parameter * parameterAuxNode = params_list_head;
		while (parameterAuxNode -> next != NULL)
			parameterAuxNode = parameterAuxNode -> next;
		parameterAuxNode -> next = to_add_param;
	}
}

/*
	Appends a varNode to a list of VarNodes.
*/
VarNode * concat_varnodes(VarNode * list1, VarNode * list2) {
	if (list1 != NULL) {
		VarNode * varAuxNode = list1;
		while (varAuxNode -> next != NULL) {
			varAuxNode = varAuxNode -> next;
		}
		varAuxNode -> next = list2;
		return list1;
	}
	return list2;
}

/*
	Closes the current enviroment, leaving the next enviroment on the top of the stack.
*/
void close_enviroment() {
	if (amount_open_enviroments != 1) {
		symbol_table = symbol_table -> next;
		amount_open_enviroments--;
	}
}

/*
	Sets a value to a varNode
*/
void set_value_to_varnode(VarNode * var_node, int value) {
	if (var_node == NULL)
		printf("Cant add value to a non-existent variable.\n");
	else
		var_node -> value = value;
}

/*
	Opens a new enviroment level. It is placed on the top of the stack.
*/
void open_enviroment() {
	EnviromentNode * new_level = (EnviromentNode *) malloc(sizeof(EnviromentNode));
	new_level -> variables = NULL;
	new_level -> next = symbol_table;
	symbol_table = new_level;
	amount_open_enviroments++;
}

int count_parameters(Parameter * list) {
	int amount = 0;
	Parameter * aux = list;
	while (aux != NULL) {
		amount++;
		aux = aux -> next;
	}
	return amount;
}


/*
	Creates a new function Node and adds it to the function's list.
*/
FunctionNode * add_function_to_funlist(int return_type, char * function_name, Parameter *parameters_list, ASTNode * body_head) {
	FunctionNode * new_function = (FunctionNode *) malloc(sizeof(FunctionNode));
	new_function -> id = function_name;
	new_function -> type = get_return_type(return_type);
	new_function -> parameters = parameters_list;
	new_function -> param_amount = count_parameters(parameters_list);
	new_function -> enviroment = temporal_enviroment;
	new_function -> next = fun_list_head;
	if (body_head == NULL)
		new_function -> is_extern = true;
	new_function -> body = body_head;
	new_function -> max_offset = current_local_offset + 8;
	fun_list_head = new_function;
	reset_offsets();
	return new_function;
}

/*
	Searches for a variable on a list of variables
*/
VarNode * find_variable(VarNode * head, char * var_name) {
	VarNode * varAuxNode = head;
	while (varAuxNode != NULL) {
		if (strcmp(varAuxNode -> id, var_name) == 0) {
			return varAuxNode;
		}
		varAuxNode = varAuxNode -> next;
	}
	return NULL;
}

/*
	Searches for a parameter on a list of parameters by id. if the parameter is found its returned, if not returns null
*/
Parameter * find_parameter(Parameter * param_list, char * param_name) {
	Parameter * aux = param_list;
	while (aux != NULL) {
		if (strcmp(aux -> id, param_name) == 0)
			return aux;
		aux = aux -> next;
	}
	return NULL;
}

/*
	Searches for a variable on all the enviroments.
*/
VarNode * find_variable_in_enviroments(char * var_name) {
	VarNode * result = NULL;
	Parameter * param_result = find_parameter(temporal_parameter, var_name);
	if (param_result != NULL)
		result = create_varnode_from_param(param_result);
	EnviromentNode * aux = symbol_table;
	while (result == NULL && aux != NULL) {
		result = find_variable(aux -> variables, var_name);
		aux = aux -> next;
	}
	return result;
}

/*
	Returns true if "expr" is an integer expression, false cc.
*/
bool is_integer_expression(ASTNode * expr) {
	return !expr -> is_boolean;
}

/*
	Returns true if "expr" is an boolean expression, false cc.
*/
bool is_boolean_expression(ASTNode * expr) {
	return expr -> is_boolean;
}

/*
	Returns true if both expressions are integer expressions, false cc.
*/
bool are_integer_expressions(ASTNode * expr1, ASTNode * expr2) {
	return is_integer_expression(expr1) && is_integer_expression(expr2);
}

/*
	Returns true if both expressions are boolean expressions, false cc.
*/
bool are_boolean_expressions(ASTNode * expr1, ASTNode * expr2) {
	return is_boolean_expression(expr1) && is_boolean_expression(expr2);
}

/*
	Returns true if both expressions had the same type, false cc.
*/
bool are_same_type_expressions(ASTNode * expr1, ASTNode * expr2) {
	return are_boolean_expressions(expr1,expr2) || are_integer_expressions(expr1,expr2);
}

/*
	Checks if two list of parameters are equals.
*/
bool are_parameters_equals(Parameter * formal_params, ASTNode * actual_params) {
	Parameter * formal_aux = formal_params;
	ASTNode * actual_aux = actual_params;
	while (formal_aux != NULL) {
		if (actual_aux == NULL || formal_aux -> is_boolean != actual_aux -> is_boolean) {
			error_message = "Error: Parameters in function call doesnt match";
			return false;
		}
		formal_aux = formal_aux -> next;
		actual_aux = actual_aux -> next_statement;
	}
	if (actual_aux != NULL) {
		error_message = "Error: Parameters in function call doesnt match";
		return false;
	}
	return true;
}

/*
	Checks if a function can be called. That is: it must exist and the parameters should be the same.
*/
bool is_callable(char * function_name, ASTNode * params) {
	FunctionNode * functionAuxNode = fun_list_head;
	while (functionAuxNode != NULL) {
		if (strcmp(functionAuxNode -> id, function_name) == 0)
			return are_parameters_equals(functionAuxNode -> parameters, params);
		functionAuxNode = functionAuxNode -> next;
	}
	error_message = "Error: Undefined function";
	return false;
}

/*
	Finds a function by id in the function list and its returned (if its not found returns null).
*/
FunctionNode * find_function(char * function_name) {
	FunctionNode * functionAuxNode = fun_list_head;
	while (functionAuxNode != NULL) {
		if (strcmp(functionAuxNode -> id, function_name) == 0)
			return functionAuxNode;
		functionAuxNode = functionAuxNode -> next;
	}
	return NULL;
}

/*
	Sets a type to all variables of the list.
*/
void set_types_to_var_list(int type, VarNode * var_list_head) {
	VarNode * aux = var_list_head;
	while (aux != NULL) {
		if (type == 0)
			aux -> is_boolean = true;
		else
			aux -> is_boolean = false;
		aux = aux -> next;
	}
}

/*
	Adds a variable list to the current enviroment.
*/
void add_varlist_to_enviroment(VarNode * var_list) {
	if (symbol_table != NULL)
		symbol_table -> variables = concat_varnodes(symbol_table -> variables, var_list);
}

/*
	Creates a new ASTNode.
*/
ASTNode * create_method_call_ASTnode(ASTNode * left_child, FunctionNode * function, ASTNode * right_child) {
	ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));
	result -> data = 'm';
	result -> is_boolean = function -> type == _boolean;
	result -> node_type = _method_call;
	result -> line_num = get_line_number();
	result -> col_num = get_column_number();
	result -> var_data = NULL;
	result -> function_data = function;
	result -> left_child = left_child;
	result -> right_child = right_child;
	return result;
}

/*
	Creates a new parameter Node from an expression that represents an actual parameter of a function call.
*/
Parameter * create_argument_parameter(ASTNode * expr_ASTNode) {
	Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));
	if (expr_ASTNode -> var_data != NULL) {
		new_param -> is_boolean = expr_ASTNode -> var_data -> is_boolean;
		new_param -> value = expr_ASTNode -> var_data -> value;
		new_param -> id = expr_ASTNode -> var_data -> id;
	}
	else {
		new_param -> is_boolean = expr_ASTNode -> is_boolean;
		new_param -> value = expr_ASTNode -> data;
		new_param -> id = NULL;
	}
	return new_param;
}

/*
	Creates a new parameter
*/
Parameter * create_parameter(char * id, bool is_boolean) {
	Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));
	new_param -> id = id;
	new_param -> is_boolean = is_boolean;
	new_param -> offset = current_parameter_offset;
	increase_parameter_offset();
	new_param -> next = NULL;
	return new_param;
}

/*
	Shows the symbol table on console.
*/
void print_symbol_table() {
	printf("\n");
	printf(" ============= SYMBOL TABLE ============\n");
	EnviromentNode * aux = symbol_table;
	int env = amount_open_enviroments;
	if (symbol_table == NULL)
		printf("The symbol table is empty \n");
	else {
		VarNode * varAuxNode;
		while (aux != NULL) {
			printf("Level %d:\n", env);
			varAuxNode = aux -> variables;
			while(varAuxNode != NULL) {
				if (varAuxNode -> is_boolean)
					printf("\tboolean ");
				else
					printf("\tinteger ");
				printf("%s ", varAuxNode -> id);
				printf("= %d", varAuxNode -> value);
				printf("\t%s\n", get_varnode_kind_string(varAuxNode));
				varAuxNode = varAuxNode -> next;
			}
			aux = aux -> next;
			env--;
		}
	}
	printf("===========================================\n");
}

/*
	Shows on console the formal parameters of a function.
*/
void print_formal_parameters(Parameter * head) {
	printf("(");
	if (head != NULL) {
		Parameter * aux = head;
		while (aux != NULL) {
			if (aux -> is_boolean)
				printf((aux -> next == NULL)?"bool %s":" bool %s, ", aux -> id);
			else
				printf((aux -> next == NULL)?"integer %s":" integer %s, ", aux -> id);
			aux = aux -> next;
		}
	}
	printf(")");
	printf("\n");
}

/*
	Appends an statement into a statements list.
*/
ASTNode * add_statement_to_list(ASTNode * statement_list, ASTNode * new_statement) {
	if (statement_list != NULL) {
		ASTNode * aux = statement_list;
		while (aux -> next_statement != NULL)
			aux = aux -> next_statement;
		aux -> next_statement = new_statement;
		return statement_list;
	}
	return new_statement;
}

/*
	Shows on console the AST of a function.
*/
void print_tree_formatted_by_level(ASTNode *root, int level) {
	if (root != NULL) {
		TypeNode node_type = root -> node_type;
		if (!(node_type == _if ||node_type == _while || node_type == _if_body))
			printf("   %d  ", root -> line_num);
		else
			printf("       ");
		for(int i = 0; i <= level; i++)
			printf("     ");
		printf(" |> '%s' \n", get_string_representation(root));
		print_tree_formatted_by_level(root -> left_child, level + 1);
		print_tree_formatted_by_level(root -> right_child, level + 1);
		print_tree_formatted_by_level(root -> next_statement, level);
	}
}

/*
	Shows on console the AST of a function.
*/
void print_whole_tree(ASTNode * root) {
	print_tree_formatted_by_level(root, 0);
}

/*
	Shows on console the complete information of the function, that is: identifier, parameters and AST.
*/
void print_function_node(FunctionNode * function) {
	printf("\n");
	printf("========== FUNCTION ========== \n");
	printf("\n");
	printf("%s %s",get_return_type_string(function -> type),function -> id);
	print_formal_parameters(function -> parameters);
	printf("Max offset: %d\n",function -> max_offset);
	printf("  Lines      TREE: \n");
	printf("\n");
	if (function -> body != NULL)
		 print_whole_tree(function -> body);
	else
		printf("Null body");
}

/*
	Shows on console all the functions of the program.
*/
void print_functions() {
	printf("\n");
	printf("\n");
	printf("================ FUNCTIONS OF THE PROGRAM  ===============\n");
	printf("\n");
	FunctionNode * aux = fun_list_head;
	while (aux != NULL) {
		print_function_node(aux);
		aux = aux -> next;
	}
	printf("\n");
	printf("\n");
	printf("=========== END FUNCTIONS OF THE PROGRAM  ==========\n");
	printf("\n");
	printf("\n");
}

/*
	Returns true if node is a return node, false cc.
*/
bool is_return_node(ASTNode * node) {
	return node -> node_type == _return;
}

/*
	Returns the type of an expression.
*/
ReturnType get_expression_type(ASTNode * expr) {
	if (expr -> is_boolean)
		return _boolean;
	else
		return _integer;
}

/*
	Returns true if a body of a function has a return statement, false cc.
*/
bool has_return(ASTNode * body) {
	ASTNode * root = body;
	if (root != NULL) {
		if (is_return_node(root) && (root -> right_child != NULL))
			return true;
		return has_return(root -> next_statement) || has_return(root -> right_child) || has_return(root -> left_child);
	}
	return false;
}

/*
	Returns true if a functions is returning the right type, false cc.
*/
bool check_return_types(ASTNode * body, ReturnType type) {
	ASTNode * root = body;
	bool no_errors_found = true;
	if (root != NULL) {
		// base case
		if (is_return_node(root)) {
			if (get_expression_type(root -> right_child) != type) {
				switch (type) {
					case _boolean:
						error_message = "Type Error: Boolean expression expected but Integer expression found";
						return false;
					case _integer:
						error_message = "Type Error: Integer expression expected but Boolean expression found";
						return false;
				}
			}
		}
		// inductive case
		no_errors_found = check_return_types(root -> next_statement, type) && check_return_types(root -> right_child, type) && check_return_types(root -> left_child, type);
	}
	return no_errors_found;
}

/*
	Does the semantic check of a return types of all defined functions.
*/
bool check_functions_return_types() {
	FunctionNode * aux = fun_list_head;
	bool no_errors_found = true;
	while (aux != NULL && no_errors_found) {
		if (!aux -> is_extern) {
			if (aux -> type == _void) {
				if (has_return(aux -> body)) {
					error_message = "Type Error: Cannot return an expression in a void function";
					return false;
				}
			}
			else if (!has_return(aux -> body)) {
				error_message = "Missing return statement";
				return false;
			}
			else
				no_errors_found = check_return_types(aux -> body, aux -> type);
		}
		aux = aux -> next;
	}
	return no_errors_found;
}

/*
	Appends a parameter to a list of parameters.
*/
void add_parameter_to_list(ASTNode * list, ASTNode * new_param) {
	if (list != NULL) {
		ASTNode * aux = list;
		while (aux -> next_statement != NULL)
			aux = aux -> next_statement;
		aux -> next_statement = new_param;
	}
}

/*
	Removes the ASTNodes from the Tree that are not reachable because of a return statement.
*/
void erase_statements_after_return(ASTNode * body) {
	ASTNode * root = body;
	if (root != NULL) {
		if (is_return_node(root)) {
			free_astnode_memory(root -> next_statement);
			root -> next_statement = NULL;
		}
		else {
			erase_statements_after_return(root -> right_child);
			erase_statements_after_return(root -> left_child);
			erase_statements_after_return(root -> next_statement);
		}
	}
}

/*
	Optimize each non extern function by removing the unreachable code caused by a return statements.
*/
void AST_optimization() {
	FunctionNode * aux = fun_list_head;
	while (aux != NULL) {
		if (!aux -> is_extern)
			erase_statements_after_return(aux -> body);
		aux = aux -> next;
	}
}

%}

%union {
	int i;
	char * s;
	ASTNode * node;
	VarNode * varnode;
	FunctionNode * functionnode;
	Parameter * parameternode;
};

%token<i> _PROGRAM_
%token<i> _BEGIN_
%token<i> _END_
%token<i> _VOID_
%token<i> _IF_
%token<i> _ELSE_
%token<i> _INT_
%token<i> _INTEGER_
%token<i> _BOOL_
%token<i> _RETURN_
%token<i> _MAIN_
%token<i> _PRINT_
%token<i> _THEN_
%token<i> _SEMICOLON_
%token<i> _COMMA_
%token<i> _L_PARENTHESIS_
%token<i> _R_PARENTHESIS_
%token<i> _PLUS_
%token<i> _MINUS_
%token<i> _MULTIPLY_
%token<i> _DIVIDE_
%token<i> _MOD_
%token<i> _GREATER_THAN_
%token<i> _LESSER_THAN_
%token<i> _EQUALS_
%token<i> _AND_
%token<i> _OR_
%token<i> _NOT_
%token<i> _ASSIGNMENT_
%token<i> _WHILE_
%token<i> _TRUE_
%token<i> _FALSE_
%token<s> _ID_
%token<i> _EXTERN_

%start prog

%nonassoc _ASSIGNMENT_
%left _AND_ _OR_
%nonassoc _EQUALS_ _GREATER_THAN_ _LESSER_THAN_
%left _PLUS_ _MINUS_
%left _MULTIPLY_ _DIVIDE_ _MOD_
%right NEG

%type<varnode> vars_block
%type<varnode> id_list
%type<parameternode> params_def
%type<node> params_call
%type<node> method_decl
%type<node> main_decl
%type<node> method_call
%type<node> code_block
%type<node> code_block_body
%type<node> statements_block
%type<node> statement
%type<node> conditional_statement
%type<node> expr
%type<node> literal
%type<node> integer_literal
%type<node> bool_literal
%type<i> type

%locations

%%

prog: _PROGRAM_ scope_open prog_body scope_close
		{
			if (!check_functions_return_types()) {
				yyerror(error_message);
				return -1;
			}
			print_functions();
			AST_optimization();
			printf("arbol optimizado \n");
			print_functions();
			printf("por generar codigo intermedio \n");
			generate_functions_intermediate_code(fun_list_head);
			printf("codigo intermedio generado \n");
			print_instructions();
			create_assembly_file(head, symbol_table -> variables);
			free_function_memory(fun_list_head);
			free_instruction_memory(head);
		}
;

scope_open: _BEGIN_
		{
			open_enviroment();
		}
;

scope_close: _END_
		{
			temporal_enviroment = symbol_table -> variables;
			print_symbol_table();
			close_enviroment();
		}
;

prog_body: vars_block methods_block main_decl
	| vars_block main_decl
	| methods_block main_decl
	| main_decl
;

methods_block: method_decl
	| methods_block method_decl
;

method_decl: type _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, $2, $4, $6);
		}
	| type _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, $2, NULL, $5);
		}
	| _VOID_ _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(2, $2, $4, $6);
		}
	| _VOID_ _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(2, $2, NULL, $5);
		}
	| type _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ _EXTERN_ _SEMICOLON_
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, $2, $4, NULL);
		}
	| type _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ _EXTERN_ _SEMICOLON_
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, $2, NULL, NULL);
		}
	| _VOID_ _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ _EXTERN_ _SEMICOLON_
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(-1, $2, $4, NULL);
		}
	| _VOID_ _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ _EXTERN_ _SEMICOLON_
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(-1, $2, NULL, NULL);
		}
;

main_decl: type _MAIN_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, "main", $4, $6);
		}
	| type _MAIN_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist($1, "main", NULL, $5);
		}
	| _VOID_ _MAIN_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(-1, "main", $4, $6);
		}
	| _VOID_ _MAIN_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block
		{
			temporal_parameter = NULL;
			FunctionNode * new_function = add_function_to_funlist(-1, "main", NULL, $5);
		}
;

code_block: scope_open code_block_body scope_close
		{
			$$ = $2;
		}
;

code_block_body: vars_block statements_block
		{
			$$ = $2;
		}
	| statements_block
		{
			$$ = $1;
		}
	|
		{
			$$ = NULL;
		}
;

statements_block: statement
		{
			$$ = $1;
		}
	| statements_block statement
		{
			$$ = add_statement_to_list($$, $2);
		}
;

statement:  _ID_ _ASSIGNMENT_ expr _SEMICOLON_
		{
			VarNode *id_varnode = find_variable_in_enviroments($1);
			if (id_varnode == NULL) {
				yyerror("Definition Error: Cannot give value to an inexistent variable");
				return -1;
			}
			ASTNode * node_from_id = create_AST_leave_from_VarNode(id_varnode);
			if (are_same_type_expressions(node_from_id, $3)) {
				$$ = create_AST_node(node_from_id, '=', $3);
			}
			else {
				if(is_boolean_expression($3)) {
					yyerror("Type Error: Cannot assign a Bool value on Integer variable");
					return -1;
				}
				if(is_integer_expression($3)) {
					yyerror("Type Error: Cannot assign an Integer value on Bool variable");
					return -1;
				}
			}
		}
	| method_call _SEMICOLON_
		{
			$$ = $1;
		}
	| conditional_statement
		{
			$$ = $1;
		}
	| _WHILE_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ code_block
		{
			if (is_boolean_expression($3))
				$$ = create_AST_node($3, 'w', $5);
			else {
				yyerror("Type error: Integer expression found in While condition. It must be a Boolean expression");
				return -1;
			}
		}
	| _RETURN_ expr _SEMICOLON_
		{
			$$ = create_AST_node(NULL, 'r', $2);
		}
	| _RETURN_ _SEMICOLON_
		{
			$$ = create_AST_node(NULL, 'r', NULL);
		}
	| _SEMICOLON_
		{
			$$ = NULL;
		}
	| code_block
		{
			$$ = $1;
		}
;

conditional_statement: _IF_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ _THEN_ code_block
		{
			ASTNode * if_body;
			if (is_boolean_expression($3)) {
				if_body = create_AST_node($6, 'b', NULL);
				$$ = create_AST_node($3, 'i', if_body);
			}
			else {
				yyerror("Type error: Integer expression found in If condition. It must be a Boolean expression");
				return -1;
			}
		}
	| _IF_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ _THEN_ code_block _ELSE_ code_block
		{
			ASTNode * if_body;
			if (is_boolean_expression($3)) {
				if_body = create_AST_node($6, 'b', $8);
				$$ = create_AST_node($3, 'i', if_body);
			}
			else {
				yyerror("Type error: Integer expression found in If condition. It must be a Boolean expression");
				return -1;
			}
		}
;

params_call: expr
		{
			$$ = $1;
		}
	| params_call _COMMA_ expr
		{
			add_parameter_to_list($1, $3);
			$$ = $1;
		}
;

params_def: type _ID_
		{
			temporal_parameter = create_parameter($2, $1 == 0);
			$$ = temporal_parameter;
		}
	| params_def _COMMA_ type _ID_
		{
			Parameter * aux = $1;
			while (aux -> next != NULL)
				aux = aux -> next;
			aux -> next = create_parameter($4, $3 == 0);
			temporal_parameter = $1;
			$$ = $1;
		}
;

type: _INTEGER_
		{
			$$ = 1;
		}
	| _BOOL_
		{
			$$ = 0;
		}
;

expr: _ID_
		{
			char * var_name = $1;
			VarNode * var_data = find_variable_in_enviroments(var_name);
			if (var_data != NULL)
				$$ = create_AST_leave_from_VarNode(var_data);
			else {
				$$ = NULL;
				yyerror("Error: Undefined variable");
				return -1;
			}
		}
	| literal
		{
			$$ = $1;
		}
	| method_call
		{
			$$ = $1;
		}
	| expr _PLUS_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '+', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _MINUS_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '-', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _MULTIPLY_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '*', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _DIVIDE_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '/', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _MOD_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '%', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _LESSER_THAN_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '<', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _GREATER_THAN_ expr
		{
			if (are_integer_expressions($1,$3))
				$$ = create_AST_node($1, '>', $3);
			else {
				yyerror("Type error: integer expressions expected but boolean expression found");
				return -1;
			}
		}
	| expr _EQUALS_ expr
		{
			if (are_same_type_expressions($1,$3))
				$$ = create_AST_node($1, 'e', $3);
			else {
				yyerror("Type error: Different types cant be compared");
				return -1;
			}
		}
	| expr _AND_ expr
		{
			if (are_boolean_expressions($1,$3))
				$$ = create_AST_node($1, '&', $3);
			else {
				yyerror("Type error: boolean expressions expected but integer expression found");
				return -1;
			}
		}
	| expr _OR_ expr
		{
			if (are_boolean_expressions($1,$3))
				$$ = create_AST_node($1, '|', $3);
			else {
				yyerror("Type error: boolean expressions expected but integer expression found");
				return -1;
			}
		}
	| _MINUS_ expr %prec NEG
		{
			if(is_integer_expression($2))
				$$ = create_AST_node(NULL, '-', $2);
			else{
				yyerror("Type error: integer expression expected but boolean expression found");
				return -1;
			}
		}
	| _NOT_ expr %prec NEG
		{
			if(is_boolean_expression($2))
				$$ = create_AST_node(NULL, '!', $2);
			else{
				yyerror("Type error: cannot applicants boolean operator to a non boolean expression");
				return -1;
			}
		}
	| _L_PARENTHESIS_ expr _R_PARENTHESIS_
		{
			$$ = $2;
		}
;

method_call: _ID_ _L_PARENTHESIS_ params_call _R_PARENTHESIS_
		{
			if (!is_callable($1, $3)) {
				yyerror(error_message);
				return -1;
			}
			$$ = create_method_call_ASTnode(NULL, find_function($1), $3);
		}
	| _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_
		{
			if (!is_callable($1, NULL)) {
				yyerror(error_message);
				return -1;
			}
			$$ = create_method_call_ASTnode(NULL, find_function($1), NULL);
		}
;

literal: integer_literal
		{
			$$ = $1;
		}
	| bool_literal
		{
			$$ = $1;
		}
;

bool_literal: _TRUE_
		{
			$$ = create_AST_leave_from_value(1, true);
		}
	| _FALSE_
		{
			$$ = create_AST_leave_from_value(0, true);
		}
;

integer_literal: _INT_
		{
			$$ = create_AST_leave_from_value($1, false);
		}
;

vars_block: type id_list _SEMICOLON_
		{
			set_types_to_var_list($1, $2);
			$$ = $2;
			add_varlist_to_enviroment($$);
		}
	| vars_block type id_list _SEMICOLON_
		{
			set_types_to_var_list($2, $3);
			$$ = $3;
			add_varlist_to_enviroment($$);
		}
;

id_list: _ID_
		{
			$$ = create_var($1);
		}
	| id_list _COMMA_ _ID_
		{
			$$ = concat_varnodes($$, create_var($3));
		}
;

%%
