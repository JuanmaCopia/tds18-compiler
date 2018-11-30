#include "structs.h"

int label_quantity = 0;

/*
	Creates and returns a new VarNode.
*/
VarNode * create_VarNode(char * id, int value, bool is_boolean) {
	VarNode * new_node = malloc(sizeof(VarNode));
	new_node -> id = id;
	new_node -> value = value;
	new_node -> is_boolean = is_boolean;
	new_node -> next = NULL;
	return new_node;
}

/*
	Returns a string that represents an offset of a local variable or of a temporal.
*/
char * create_string_offset(int offset) {
	char offset_string[64];
	sprintf(offset_string, "%d(%rbp)\0", offset);
	char * res = malloc(strlen(offset_string));
	sprintf(res, "%s", offset_string);
	return res;
}

/*
	Returns a string that represents a assembler reference to a global variable.
*/
char * create_global_string_offset(char * var_id) {
	char offset_string[64];
	sprintf(offset_string, "%s(%s)\0", var_id, "%rip");
	char * res = malloc(strlen(offset_string));
	sprintf(res, "%s", offset_string);
	return res;
}
/*
	Creates and returns a new VarNode created from a parameter.
*/
VarNode * create_varnode_from_param(Parameter * param) {
	VarNode * new_varnode = create_VarNode(param -> id, param -> value, param -> is_boolean);
	new_varnode -> is_defined = false;
	new_varnode -> offset = param -> offset;
	new_varnode -> string_offset = create_string_offset(param -> offset);
	printf("string offset de parametro generado: %s\n", new_varnode -> string_offset);
	new_varnode -> kind = _parameter;
	return new_varnode;
}
 
/*
	Creates a new varnode
*/
VarNode * create_var_node() {
	VarNode * new_node = malloc(sizeof(VarNode));
	new_node -> is_defined = false;
	new_node -> next = NULL;
	return new_node;
}

/*
	Creates and returns a new VarNode.
*/
VarNode * create_temporal_with_id(char * id) {
	VarNode * new_node = create_var_node();
	new_node -> id = id;
	new_node -> kind = _temporal;
	return new_node;
}

/*
	Creates a new instruction.
*/
InstructionNode * create_instructionNode(int operation, VarNode * result, VarNode * op1, VarNode * op2) {
	InstructionNode * new_node = malloc(sizeof(InstructionNode));
	new_node -> operation = operation;
	new_node -> op1 = op1;
	new_node -> op2 = op2;
	new_node -> result = result;
	new_node -> next= NULL;
	new_node -> back= NULL;
	return new_node;
}

/*
	Frees the memory of a VarNode and its related nodes.
*/
void free_varnode_memory(VarNode * v) {
	if (v != NULL) {
		VarNode * next = v -> next;
		free(v -> string_offset);
		free(v);
		v = NULL;
		free_varnode_memory(next);
	}
}

/*
	Frees the memory of a Parameter node and its related nodes.
*/
void free_parameter_memory(Parameter * p) {
	if (p != NULL) {
		Parameter * next = p -> next;
		free(p);
		p = NULL;
		free_parameter_memory(next);
	}
}

/*
	Frees the memory of an ASTNode and its related nodes.
*/
void free_astnode_memory(ASTNode * ast) {
	if (ast != NULL) {
		ASTNode * next_statement = ast -> next_statement;
		ASTNode * left_child = ast -> left_child;
		ASTNode * right_child = ast -> right_child;
		free(ast);
		ast = NULL;
		free_astnode_memory(left_child);
		free_astnode_memory(right_child);
		free_astnode_memory(next_statement);
	}
}

/*
	Frees the memory of a Function Node and its related nodes.
*/
void free_function_memory(FunctionNode * f) {
	if (f != NULL) {
		FunctionNode * next = f -> next;
		free_parameter_memory(f -> parameters);
		free_varnode_memory(f -> enviroment);
		free_astnode_memory(f -> body);
		free(f);
		f = NULL;
		free_function_memory(next);
	}
}

/*
	Frees the memory of a VarNode and its related nodes.
*/
void free_instruction_memory(InstructionNode * i) {
	if (i != NULL) {
		if (i -> operation == CALL)
		{
			free(i -> op1);
			free(i -> result);
		}
		if (i -> operation == CMP) 
		{
			free(i -> op1);
			free(i -> op2);
			free(i -> result);
		}
		if (i -> operation == LESSER_THAN ||
			i -> operation == GREATER_THAN) {
			free(i -> op1);
			free(i -> op2);	
		}
		if (i -> operation == JMP ||
			i -> operation == JE ||
			i -> operation == JNE) 
		{
			free(i -> result);	
		}
		InstructionNode * next = i -> next;
		free(i);
		i = NULL;
		free_instruction_memory(next);
	}
}

/*
	Returns the string representation of a temporal.
*/
char * get_temporal_string(VarNode * temp) {
	if (temp == NULL)
	return "";
	if (temp -> is_defined) {
	if (temp -> is_boolean) {
		if (temp -> value == 0)
		return "false";
		else
		return "true";
	}
	else {
		int i = temp -> value;
		char str[12];
		sprintf(str, "%d", i);
		char * ret = str;
		return ret;
	}
	}
	return temp -> id;
}

/*
	Given an isntruction node returns the string representation of the operation.
*/
char * get_operation_string(InstructionNode * i) {
	switch (i -> operation) {
	case PLUS:          return "PLUS ";
	case PROD:          return "PROD ";
	case MOD:           return "MOD  ";
	case DIV:           return "DIV  ";
	case MINUS:         return "MINUS";
	case EQUALS:        return "EQUAL";
	case OR:            return "OR   ";
	case AND:           return "AND  ";
	case CALL:          return "CALL ";
	case END_FUN:       return "ENDF ";
	case PUSH:          return "PUSH ";
	case PUSH1:         return "PUSH1";
	case PUSH2:         return "PUSH2";
	case PUSH3:         return "PUSH3";
	case PUSH4:         return "PUSH4";
	case PUSH5:         return "PUSH5";
	case PUSH6:         return "PUSH6";
	case POP:           return "POP  ";
	case GREATER_THAN:  return "GRETR";
	case LESSER_THAN:   return "LESSR";
	case ASSIGN:        return "ASSGN";
	case LABEL:         return "LABEL";
	case JMP:           return "JUMP ";
	case BEGIN_FUN:     return "BEGIN";
	case JE:            return "JE   ";
	case JNE:           return "JNE  ";
	case CMP:           return "CMP  ";
	case RETURN:        return "RETRN";
	case NEGAT:         return "NEG  ";
	case BREAK:         return "BREAK";
	default:            return "------ UNKNOWN OPERATION ------";
	}
}

/*
	Creates a new label.
*/
VarNode * create_label() {
	char label_name[128];
	sprintf(label_name, "label%d\0", label_quantity);
	char * res = malloc(strlen(label_name));
	sprintf(res, "%s", label_name);
	VarNode * new_node = create_var_node();
	new_node -> id = res;
	new_node -> kind = _label;
	label_quantity++;
	return new_node;
}

/*
	Creates a new label and sets its id.
*/
VarNode * create_label_with_id(char * id) {
	VarNode * new_node = create_var_node();
	new_node -> id = id;
	new_node -> kind = _label;
	return new_node;
}

/*
	Returns the corresponding node type depending of the operator.
*/
TypeNode get_node_type(int op) {
	if (op == 'i')
	return _if;
	else if (op == '=')
	return _assign;
	else if (op == 'r')
	return _return;
	else if (op == 'm')
	return _method_call;
	else if (op == 'l')
	return _literal;
	else if (op == 'b')
	return _if_body;
	else if (op == 'w')
	return _while;
	else if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
	return _arith_op;
	else if (op == '<' || op == '>' || op == 'e' || op == '&' || op == '|' || op == '!')
	return _boolean_op;
	else
	return _id;
}

/*
	Takes an int as parameter and returns the represented ReturnType.
*/
ReturnType get_return_type(int type_int_value) {
	switch (type_int_value) {
	case 0:
		return _boolean;
	case 1:
		return _integer;
	default:
		return _void;
	}
}

/*
	Returns the string representation of a ReturnType enum.
*/
char * get_return_type_string(ReturnType value) {
	switch (value)
	{
	case _boolean: return "bool";
	case _integer: return "integer";
	case _void: return "void";
	}
}

/*
	Returns the string representation of a ReturnType enum.
*/
char * get_varnode_kind_string(VarNode * var) {
	switch (var -> kind)
	{
	case _global: return "global";
	case _local: return "local";
	case _parameter: return "parameter";
	case _temporal: return "temporal";
	case _label: return "label";
	}
}

/*
	Takes an operator and returns true if the operation results in a boolean value, false cc.
*/
bool is_boolean_operation(int op) {
	if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
	return false;
	else if (op == '<' || op == '>' || op == 'e' || op == '&' || op == '|' || op == '!')
	return true;
	return false;
}

/*
	Returns the string representation of the enum TypeNode
*/
char * get_type_node_string(TypeNode tn) {
	switch (tn)
	{
	case _if: return "if";
	case _if_body: return "if body";
	case _while: return "while";
	case _arith_op: return "arith op";
	case _boolean_op: return "boolean op";
	case _assign: return "assign";
	case _method_call: return "method call";
	case _return: return "return";
	case _literal: return "literal";
	case _id: return "id";
	}
}

/*
	Returns the string representation of an ASTNode.
*/
char * get_string_representation(ASTNode * node) {
	char * aux;
	switch (node -> node_type) {
	case _if: return "if";
	case _if_body: return "if body";
	case _while: return "while";
	case _arith_op: return (char *) &(node -> data);
	case _boolean_op: return (char *) &(node -> data);
	case _assign: return "=";
	case _method_call:
		if (node -> function_data != NULL) {
		int fun_id_len = strlen(node -> function_data -> id);
		if (fun_id_len <= 10) {
			char str[12];
			sprintf(str, "%s%s", node -> function_data -> id, "()");
			char * ret = str;
			return ret;
		}
		else {
			char str[32];
			sprintf(str, "%s%s", node -> function_data -> id, "()");
			char * ret = str;
			return ret;
		}
		}
		else {
		return "method_call";
		}
		break;
	case _return: return "return";
	case _id:
		return node -> var_data -> id;
	case _literal:
		if (node -> is_boolean) {
		if (node -> data == 0)
			return "false";
		else
			return "true";
		}
		else {
		if (node -> var_data == NULL) {
			int i = node -> data;
			char str[8];
			sprintf(str, "%d", i);
			char * ret = str;
			return ret;
		}
		else {
			return node -> var_data -> id;
		}
		}
		break;
	}
}

/*
  Prints a single instruction on cosole.
*/
void print_instruction(InstructionNode * i) {
	char op1_string[12];
	char op2_string[12];
	char result_string[12];
	strcpy(op1_string, get_temporal_string(i -> op1));
	strcpy(op2_string, get_temporal_string(i -> op2));
	strcpy(result_string, get_temporal_string(i -> result));
	switch (i -> operation) {
		case BEGIN_FUN:
			printf("\n\n========================  START OF INSTRUCTIONS OF A NEW FUNCTION  ======================\n\n");
			printf("\t%s   %s", get_operation_string(i), result_string);
			//print_varnode(i -> result);
			printf("                          %s  %s   max_offset: %d\n", i -> result -> id, get_varnode_kind_string(i -> result), i -> result -> offset);
			printf("\n\n");
			break;
		case END_FUN:
      		printf("\t%s   %s\n", get_operation_string(i), result_string);
      		print_varnode(i -> result);
      		break;
		case LABEL:
      		printf("%s\n", result_string);
      		//print_varnode(i -> result);
      		break;
		case RETURN:
      		printf("\t%s   %s\n", get_operation_string(i), result_string);
      		print_varnode(i -> result);
      		break;
		case PUSH: case PUSH1: case PUSH2: case PUSH3: case PUSH4: case PUSH5: case PUSH6:
      		printf("\t%s   %s\n", get_operation_string(i), result_string);
      		print_varnode(i -> result);
      		break;
		case ASSIGN: case NEGAT: case CALL: 
      		printf("\t%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      		print_varnode(i -> result);
      		print_varnode(i -> op1);
      		break;
		case PLUS: case MINUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN: case CMP:
      		printf("\t%s   %s  %s  %s \n", get_operation_string(i), result_string, op1_string, op2_string);
      		print_varnode(i -> result);
      		print_varnode(i -> op1);
      		print_varnode(i -> op2);
      		break;
		case JMP: 
      		printf("\t%s   %s\n", get_operation_string(i), result_string);
      		//print_varnode(i -> result);
      		break;
		case JE:
      		printf("\t%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      		//print_varnode(i -> result);
      		print_varnode(i -> op1);
      		break;
		case POP: case BREAK:
      		printf("\t%s\n\n", get_operation_string(i));
      		break;
		default:
			printf("\tUNKNOWN INSTRUCTION\n");
			break;
  	}
}

/*
  Prints VarNode information.
*/
void print_varnode(VarNode * var) {
	switch (var -> kind) {
		case _local: case _parameter:
			printf("                                              %s  %s  offset: %d\n", var -> id, get_varnode_kind_string(var), var -> offset);
			break;
		case _temporal:
			printf("                                              %s  %s  offset: %d    value: %s\n", var -> id, get_varnode_kind_string(var), var -> offset, get_temporal_string(var));
			break;
		case _label:
			printf("                          %s  %s\n", var -> id, get_varnode_kind_string(var));
			break;
		case _global:
			printf("                                              %s  %s  offset: %s\n", var -> id, get_varnode_kind_string(var), var -> string_offset);
			break;
		default:
			printf(" ERROR, unknown varnode type \n");
			break;
	}
}