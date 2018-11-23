#include "assembly_code.h"

int label_amount = 0;

void initialize() {
	assembly_file = fopen("assembly.s" ,"w");
	if (assembly_file == NULL)
		assembly_file = fopen("assembly.s", "wb");
}

void finalize() {
	fclose(assembly_file);
}

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

void generate_assembly_global_variables(VarNode * variables) {
	fprintf(assembly_file, "\t%s\n", ".text");
	VarNode * aux = variables;
	while (aux != NULL) {
		fprintf(assembly_file, "\t.comm\t%s,%d\n", aux -> id, 8);
		aux = aux -> next;
	}
}

void generate_assembly_code(InstructionNode * ins) {
	if (ins != NULL)
		switch (ins -> operation) {
			case PLUS:
				generate_assembly_operation(ins, ADD);
				break;
			case MINUS:
				generate_assembly_operation(ins, SUB);
				break;
			case PROD:
				generate_assembly_operation(ins, IMUL);
				break;
			case DIV:
				generate_assembly_div(ins, RAX);
				break;
			case EQUALS:
				generate_assembly_bool_operation(ins, SETE);
				break;
			case AND:
				generate_assembly_and(ins);
				break;
			case OR:
				generate_assembly_or(ins);
				break;
			case GREATER_THAN:
				generate_assembly_bool_operation(ins, SETG);
				break;
			case LESSER_THAN:
				generate_assembly_bool_operation(ins, SETL);
				break;
			case MOD:
				generate_assembly_div(ins, RDX);
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
					break;
				case EXTERN:
					break;
			}
} 

void generate_assembly_compare(InstructionNode * ins) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RAX);
	create_instruction_2op(MOVQ, ins -> op2 -> string_offset, RDX);
	create_instruction_2op(COMP, RDX, RAX);
}

void generate_assembly_operation(InstructionNode * ins, char * operation_string) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RAX);
	create_instruction_2op(MOVQ, ins -> op2 -> string_offset, RDX);
	create_instruction_2op(operation_string, RDX, RAX);
	create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
}

void generate_assembly_bool_operation(InstructionNode * ins, char * operation_string) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RAX);
	create_instruction_2op(MOVQ, create_constant_string(0), RDX);
	create_instruction_2op(COMP, ins -> op2 -> string_offset, RAX);
	create_instruction_1op(operation_string, DL);
	create_instruction_2op(MOVQ, RDX, RAX);
	create_instruction_2op(MOVQ, RAX, ins -> result -> string_offset);
}

void generate_assembly_div(InstructionNode * ins, char * reg) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RAX);
	create_instruction_1op(CQTO, "");
	create_instruction_1op(IDIVQ, ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, reg, ins -> result -> string_offset);
}

char * create_asmlabel() {
	char label_name[64];
	sprintf(label_name, "asmlabel%d\0", label_amount);
	char * res = malloc(strlen(label_name));
	sprintf(res, "%s", label_name);
	label_amount++;
	return res;
}

void generate_assembly_and(InstructionNode * ins) {
	char * label_false = create_asmlabel();
	char * label_true = create_asmlabel();
		char * label_end = create_asmlabel();
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RDX);
	create_instruction_2op(COMP, create_constant_string(0), RDX);
	create_instruction_1op(JUMPE, label_false);
	create_instruction_2op(MOVQ, ins -> op2 -> string_offset, RDX);
	create_instruction_2op(COMP, create_constant_string(0), RDX);
	create_instruction_1op(JUMPE, label_false);
	create_instruction_1op(JUMP, label_true);
	create_assembly_label(label_false);
	create_instruction_2op(MOVQ, create_constant_string(0), ins -> result -> string_offset);
	create_instruction_1op(JUMP, label_end);
	create_assembly_label(label_true);
	create_instruction_2op(MOVQ, create_constant_string(1), ins -> result -> string_offset);
	create_instruction_1op(JUMP, label_end);
	create_assembly_label(label_end);
}

void generate_assembly_or(InstructionNode * ins) {
	char * label_false = create_asmlabel();
	char * label_true = create_asmlabel();
	char * label_end = create_asmlabel();
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	if (ins -> op2 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op2 -> value), ins -> op2 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, RAX);
	create_instruction_2op(MOVQ, ins -> op2 -> string_offset, RDX);
	create_instruction_2op(ADD, RDX, RAX);
	create_instruction_2op(COMP, create_constant_string(0), RAX);
	create_instruction_1op(JUMPE, label_false);
	create_instruction_1op(JUMP, label_true);
	create_assembly_label(label_false);
	create_instruction_2op(MOVQ, create_constant_string(0), ins -> result -> string_offset);
	create_instruction_1op(JUMP, label_end);
	create_assembly_label(label_true);
	create_instruction_2op(MOVQ, create_constant_string(1), ins -> result -> string_offset);
	create_assembly_label(label_end);
}

void generate_assembly_assign(InstructionNode * ins) {
	if (ins -> op1 -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> op1 -> value), ins -> op1 -> string_offset);
	create_instruction_2op(MOVQ, ins -> op1 -> string_offset, R10);
	create_instruction_2op(MOVQ, R10, ins -> result -> string_offset);
}

void generate_assembly_push(InstructionNode * ins) {
	if (ins -> result -> is_defined)
	create_instruction_1op(PUSH_, create_constant_string(ins -> result -> value));
	else
	create_instruction_1op(PUSH_, ins -> result -> string_offset);
}

void generate_assembly_extern_param_passage(InstructionNode * ins, char * reg) {
	if (ins -> result -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> result -> value), ins -> result -> string_offset);
	create_instruction_2op(MOVQ, ins -> result -> string_offset, reg);
}

void generate_assembly_return(InstructionNode * ins) {
	if (ins -> result -> is_defined)
		create_instruction_2op(MOVQ, create_constant_string(ins -> result -> value), RAX);
	else
		create_instruction_2op(MOVQ, ins -> result -> string_offset, RAX);
}

void generate_assembly_begin_fun(InstructionNode * ins) {
	if (ins -> result -> id == "main")
		fprintf(assembly_file, "\t%s\n", ".globl main");
	create_assembly_label(ins -> result -> id);
	fprintf(assembly_file, "\tenter\t$(%d), $0\n", ins -> result -> offset * -1);
}

void generate_assembly_end_fun(InstructionNode * ins) {
	fprintf(assembly_file, "\tleave\n\tret\n");
}

char * create_assembly_label(char * id) {
	fprintf(assembly_file, "%s:\n", id);
}

void create_instruction_2op(char * instruction, char * op1, char * op2) {
	fprintf(assembly_file, "\t%s\t%s, %s \n", instruction, op1, op2);
}

void create_instruction_1op(char * instruction, char * op1) {
	fprintf(assembly_file, "\t%s\t%s \n", instruction, op1);
}

char * create_constant_string(int constant) {
	char constant_string[64];
	sprintf(constant_string, "$%d\0", constant);
	char * res = malloc(strlen(constant_string));
	sprintf(res, "%s", constant_string);
	return res;
}
