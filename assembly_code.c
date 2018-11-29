#include "assembly_code.h"

int label_amount = 0;        // number of labels created
int final_label_amount = 0;  // number of end labels created.

/*
	Open/Creates the assembly code file.
*/
void initialize() {
	assembly_file = fopen("assembly.s" ,"w");
	if (assembly_file == NULL)
		assembly_file = fopen("assembly.s", "wb");
}

/*
	Close the assembly code file.
*/
void finalize() {
	fclose(assembly_file);
}

/*
	Generates all the assembler instructions.
*/
void create_assembly_file(InstructionNode * ins, VarNode * global_variables) {
	if (assembly_file == NULL)
		initialize();
	generate_assembly_global_variables(global_variables);
	InstructionNode * aux = ins;
	while (aux != NULL) {
		generate_assembly_code(aux);
		aux = aux -> next;
	}
	finalize();
}

/*
	Generates the assembly instructions that creates the global variables.
*/
void generate_assembly_global_variables(VarNode * variables) {
	fprintf(assembly_file, "\t%s\n", ".text");
	VarNode * aux = variables;
	while (aux != NULL) {
		fprintf(assembly_file, "\t.comm\t%s,%d\n", aux -> id, 8);
		aux = aux -> next;
	}
}

/*
	Given an intermediate code instructions, generates the corresponding assembly instructions.
*/
void generate_assembly_code(InstructionNode * ins) {
	if (ins != NULL)
		switch (ins -> operation) {
			case PLUS:
				generate_assembly_plus(ins);
				break;
			case MINUS:
				generate_assembly_minus(ins);
				break;
			case PROD:
				generate_assembly_prod(ins);
				break;
			case DIV:
				generate_assembly_div(ins);
				break;
			case EQUALS:
				generate_assembly_equals(ins);
				break;
			case AND:
				generate_assembly_and(ins);
				break;
			case OR:
				generate_assembly_or(ins);
				break;
			case GREATER_THAN:
				generate_assembly_greater(ins);
				break;
			case LESSER_THAN:
				generate_assembly_lesser(ins);
				break;
			case MOD:
				generate_assembly_mod(ins);
				break;
			case BEGIN_FUN:
				generate_assembly_begin_fun(ins);
				break;
			case END_FUN:
				generate_assembly_end_fun(ins);
				break;
			case ASSIGN:
				generate_assembly_assign(ins); 
				break;
			case LABEL:
				create_assembly_label(ins -> result -> id);
				break;
			case JMP:
				create_instruction_1op(JUMP, ins -> result -> id);
				break;
			case JE:
				create_instruction_1op(JUMPE, ins -> result -> id);
				break;
			case JNE:
				create_instruction_1op(JUMPNE, ins -> result -> id);
				break;
			case CMP:
				generate_assembly_compare(ins);
				break;
			case PUSH:
				generate_assembly_push(ins);
				break;
			case POP:
				create_instruction_1op(POP_, RCX);
				break;
			case CALL:
				create_instruction_1op(CALL_, ins -> op1 -> id);
				create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
				break;
			case RETURN:
				generate_assembly_return(ins);
				break;
			case PUSH1:
				generate_assembly_extern_param_passage(ins, RDI);
				break;
			case PUSH2:
				generate_assembly_extern_param_passage(ins, RSI);
				break;
			case PUSH3:
				generate_assembly_extern_param_passage(ins, RDX);
				break;
			case PUSH4:
				generate_assembly_extern_param_passage(ins, RCX);
				break;
			case PUSH5:
				generate_assembly_extern_param_passage(ins, R8);
				break;
			case PUSH6:
				generate_assembly_extern_param_passage(ins, R9);
				break;
			case NEGAT:
				generate_assembly_not(ins);
				break;
			case BREAK:
				create_instruction_1op(JUMP, get_current_end_asmlabel());
				break;
			}
} 

/*
	
*/
void create_optimized_instruction(char * instruction, VarNode * op, char * reg) {
	if (op != NULL) {
		if (op -> is_defined)
			create_instruction_2op(instruction, create_constant_string(op -> value), reg);
		else
			create_instruction_2op(instruction, op -> string_offset, reg);
	} 
}

/*
	
*/
void create_optimized_instruction_1op(char * instruction, VarNode * op) {
	if (op != NULL) {
		if (op -> is_defined)
			create_instruction_1op(instruction, create_constant_string(op -> value));
		else
			create_instruction_1op(instruction, op -> string_offset);
	} 
}

/*
	
*/
void set_value_on_register(VarNode * op, char * reg) {
	if (op != NULL) {
		if (op -> is_defined)
			create_instruction_2op(MOVQ, create_constant_string(op -> value), reg);
		else
			create_instruction_2op(MOVQ, op -> string_offset, reg);
	}
}


/*
	Generates the corresponding assembly instructions that compares 2 operands.
*/
void generate_assembly_compare(InstructionNode * ins) {
	set_value_on_register(ins -> op1, RAX);
	create_optimized_instruction(COMP, ins -> op2, RAX);
}

/*
	Generates the corresponding assembly instructions that executes an operation (MINUS, PLUS or PROD) with 2 operands.
*/
void generate_assembly_minus(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value - ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(SUB, ins -> op2, RAX);
		create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes an operation (MINUS, PLUS or PROD) with 2 operands.
*/
void generate_assembly_plus(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value + ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(ADD, ins -> op2, RAX);
		create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes an operation (MINUS, PLUS or PROD) with 2 operands.
*/
void generate_assembly_prod(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value * ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(IMUL, ins -> op2, RAX);
		create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes a logical operation (GREATER or LESSER) with 2 operands.
*/
void generate_assembly_greater(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const(ins -> op1 -> value > ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(COMP, ins -> op2, RAX);
		create_instruction_2op(MOVQ, "$0", RDX);
		create_instruction_1op(SETG, DL);
		create_instruction_2op(MOVQ, RDX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes a logical operation (GREATER or LESSER) with 2 operands.
*/
void generate_assembly_lesser(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const(ins -> op1 -> value < ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(COMP, ins -> op2, RAX);
		create_instruction_2op(MOVQ, "$0", RDX);
		create_instruction_1op(SETL, DL);
		create_instruction_2op(MOVQ, RDX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes a logical operation (GREATER or LESSER) with 2 operands.
*/
void generate_assembly_equals(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const(ins -> op1 -> value == ins -> op2 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(COMP, ins -> op2, RAX);
		create_instruction_2op(MOVQ, "$0", RDX);
		create_instruction_1op(SETE, DL);
		create_instruction_2op(MOVQ, RDX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes a DIV or a MOD operation.
*/
void generate_assembly_div(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value / ins -> op2 -> value), ins -> result -> string_offset);
	else {
		if (ins -> op2 -> is_defined)
			create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
		set_value_on_register(ins -> op1, RAX);
		create_instruction_1op(CQTO, "");
		create_instruction_1op(IDIVQ, ins -> op2 -> string_offset);
		create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes a DIV or a MOD operation.
*/
void generate_assembly_mod(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value % ins -> op2 -> value), ins -> result -> string_offset);
	else {
		if (ins -> op2 -> is_defined)
			create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
		set_value_on_register(ins -> op1, RAX);
		create_instruction_1op(CQTO, "");
		create_instruction_1op(IDIVQ, ins -> op2 -> string_offset);
		create_instruction_2op(MOVQ, RDX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes an AND operation.
*/
void generate_assembly_and(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const((bool) ins -> op1 -> value && (bool) ins -> op2 -> value), ins -> result -> string_offset);
	else {
		char * label_false = create_asmlabel();
		char * label_true = create_asmlabel();
		char * label_end = create_asmlabel();
		set_value_on_register(ins -> op1, RDX);
		create_instruction_2op(COMP, "$0", RDX);
		create_instruction_1op(JUMPE, label_false);
		set_value_on_register(ins -> op2, RDX);
		create_instruction_2op(COMP, "$0", RDX);
		create_instruction_1op(JUMPE, label_false);
		create_instruction_1op(JUMP, label_true);
		create_assembly_label(label_false);
		create_instruction_2op(MOVQ, "$0", ins -> result -> string_offset);
		create_instruction_1op(JUMP, label_end);
		create_assembly_label(label_true);
		create_instruction_2op(MOVQ, "$1", ins -> result -> string_offset);
		create_instruction_1op(JUMP, label_end);
		create_assembly_label(label_end);
	}
}

/*
	Generates the corresponding assembly instructions that executes a NOT operation.
*/
void generate_assembly_not(InstructionNode * ins) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const(!(bool)ins -> op1 -> value), ins -> result -> string_offset);
	else {
		set_value_on_register(ins -> op1, RAX);
		create_instruction_2op(XORQ, "$1", RAX);
		create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
	}
}

/*
	Generates the corresponding assembly instructions that executes an OR operation.
*/
void generate_assembly_or(InstructionNode * ins) {
	if (ins -> op1 -> is_defined && ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_bool_string_const((bool) ins -> op1 -> value || (bool) ins -> op2 -> value), ins -> result -> string_offset);
	else {
		char * label_false = create_asmlabel();
		char * label_true = create_asmlabel();
		char * label_end = create_asmlabel();
		set_value_on_register(ins -> op1, RAX);
		create_optimized_instruction(ADD, ins -> op2, RAX);
		create_instruction_2op(COMP, "$0", RAX);
		create_instruction_1op(JUMPE, label_false);
		create_instruction_1op(JUMP, label_true);
		create_assembly_label(label_false);
		create_instruction_2op(MOVQ, "$0", ins -> result -> string_offset);
		create_instruction_1op(JUMP, label_end);
		create_assembly_label(label_true);
		create_instruction_2op(MOVQ, "$1", ins -> result -> string_offset);
		create_assembly_label(label_end);
	}
}

/*
	Generates the corresponding assembly instructions that executes an assignment.
*/
void generate_assembly_assign(InstructionNode * ins) {
	set_value_on_register(ins -> op1, R10);
	create_instruction_2op(MOVQ, R10, ins -> result -> string_offset);
}

/*
	Generates a PUSH assembly instruction.
*/
void generate_assembly_push(InstructionNode * ins) {
	if (ins -> result -> is_defined)
		create_instruction_1op(PUSH_, create_constant_string(ins -> result -> value));
	else
		create_instruction_1op(PUSH_, ins -> result -> string_offset);
}

/*
	Generates the assembly instructions that moves the parameters to the corresponding registers before an extern function call.
*/
void generate_assembly_extern_param_passage(InstructionNode * ins, char * reg) {
	if (ins -> result -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> result -> value), ins -> result -> string_offset);
	create_instruction_2op(MOVQ, ins -> result -> string_offset, reg);
}

/*
	Generates the corresponding assembly instructions that executes a return.
*/
void generate_assembly_return(InstructionNode * ins) {
	set_value_on_register(ins -> result, RAX);
	create_instruction_1op(JUMP, get_current_end_asmlabel());
}

/*
	Generates the assembly instructions needed on the begin of a function.
*/
void generate_assembly_begin_fun(InstructionNode * ins) {
	if (ins -> result -> id == "main")
		fprintf(assembly_file, "\t%s\n", ".globl main");
	create_assembly_label(ins -> result -> id);
	fprintf(assembly_file, "\tenter\t$(%d), $0\n", ins -> result -> offset * -1);
}

/*
	Generates the assembly instructions needed on the end of a function.
*/
void generate_assembly_end_fun(InstructionNode * ins) {
	create_assembly_label(create_end_asmlabel());
	fprintf(assembly_file, "\tleave\n\tret\n");
}

/*
	Creates an assembly label.
*/
char * create_assembly_label(char * id) {
	fprintf(assembly_file, "%s:\n", id);
}

/*
	Creates an assembly instruction with 2 operands.
*/
void create_instruction_2op(char * instruction, char * op1, char * op2) {
	fprintf(assembly_file, "\t%s\t%s, %s \n", instruction, op1, op2);
}

/*
	Creates an assembly instruction with 1 operand.
*/
void create_instruction_1op(char * instruction, char * op1) {
	fprintf(assembly_file, "\t%s\t%s \n", instruction, op1);
}

/*
	Returns a string that represents a constant in assembler. 
*/
char * create_constant_string(int constant) {
	char constant_string[64];
	sprintf(constant_string, "$%d\0", constant);
	char * res = malloc(strlen(constant_string));
	sprintf(res, "%s", constant_string);
	return res;
}

/*
	Returns a string that represents a constant in assembler. 
*/
char * create_bool_string_const(bool value) {
	char constant_string[64];
	if (value)
		sprintf(constant_string, "$%d\0", 1);
	else
		sprintf(constant_string, "$%d\0", 0);
	char * res = malloc(strlen(constant_string));
	sprintf(res, "%s", constant_string);
	return res;
}

/*
	Returns a string that represents an assembly label.
*/
char * create_asmlabel() {
	char label_name[64];
	sprintf(label_name, "asmlabel%d\0", label_amount);
	char * res = malloc(strlen(label_name));
	sprintf(res, "%s", label_name);
	label_amount++;
	return res;
}

/*
	Returns a string that represents an assembly label used at the end of a function.
*/
char * create_end_asmlabel() {
	char label_name[64];
	sprintf(label_name, "endlabel%d\0", final_label_amount);
	char * res = malloc(strlen(label_name));
	sprintf(res, "%s", label_name);
	final_label_amount++;
	return res;
}

/*
	Returns a string that represents the end assembly label of the current function. 
*/
char * get_current_end_asmlabel() {
	char label_name[64];
	sprintf(label_name, "endlabel%d\0", final_label_amount);
	char * res = malloc(strlen(label_name));
	sprintf(res, "%s", label_name);
	return res;
}