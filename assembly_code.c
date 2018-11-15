#include "assembly_code.h"

void initialize() {
	assembly_file = fopen("assembly_code.s" ,"w");
	fprintf(assembly_file, "	.globl main\n");
}

void finalize() {
	fclose(assembly_file);
}

void create_assembly_file(InstructionNode * ins) {
	if (assembly_file == NULL)
		initialize();

	if (ins != NULL) {
		print_instruction(ins);
		fprintf(assembly_file, "%s\n", generate_assembly_code(ins));
	}

	if (ins -> next == NULL)
		finalize();
	else
		create_assembly_file(ins -> next);
}

char * generate_assembly_code(InstructionNode * ins) {
	if (ins != NULL)
		switch (ins -> operation) {
			case PLUS:
				return generate_assembly_plus(ins);
				break;
			case MINUS:
				return generate_assembly_minus(ins);
				break;
			case PROD:
				return generate_assembly_prod(ins);
				break;
			case DIV:
				return generate_assembly_div(ins);
				break;
			case MOD:
				return generate_assembly_mod(ins);
				break;
			case BEGIN_FUN:
				return generate_assembly_begin_fun(ins);
				break;
			case END_FUN:
				return generate_assembly_end_fun(ins);
				break;
			case ASSIGN:
				return generate_assembly_assign(ins);
				break;
		}
	return NULL;
}

char * generate_assembly_plus(InstructionNode * ins) {
	if (ins -> op1 -> kind == _constant && ins -> op2 -> kind == _constant)
		return create_constant_movq(ins -> op1 -> value + ins -> op2 -> value, (ins -> result -> offset)*SIZE, RBP);
}

char * generate_assembly_minus(InstructionNode * ins) {
	if (ins -> op1 -> kind == _constant && ins -> op2 -> kind == _constant)
		return create_constant_movq(ins -> op1 -> value - ins -> op2 -> value, (ins -> result -> offset)*SIZE, RBP);
}

char * generate_assembly_prod(InstructionNode * ins) {
	if (ins -> op1 -> kind == _constant && ins -> op2 -> kind == _constant)
		return create_constant_movq(ins -> op1 -> value * ins -> op2 -> value, (ins -> result -> offset)*SIZE, RBP);
}

char * generate_assembly_div(InstructionNode * ins) {
	if (ins -> op1 -> kind == _constant && ins -> op2 -> kind == _constant)
		return create_constant_movq(ins -> op1 -> value / ins -> op2 -> value, (ins -> result -> offset)*SIZE, RBP);
}

char * generate_assembly_mod(InstructionNode * ins) {
	if (ins -> op1 -> kind == _constant && ins -> op2 -> kind == _constant)
		return create_constant_movq(ins -> op1 -> value % ins -> op2 -> value, (ins -> result -> offset)*SIZE, RBP);
}

char * generate_assembly_begin_fun(InstructionNode * ins) {
	char * res = malloc(24*sizeof(char));
	sprintf(res, "%s", create_assembly_label(ins -> result -> id));
	sprintf(res, "%s\tenter $", res);
	sprintf(res, "%s(%d), $0", res, ins -> result -> offset);
	return res;
}

char * generate_assembly_end_fun(InstructionNode * ins) {
	return create_assembly_ret();
}

char * generate_assembly_assign(InstructionNode * ins) {
	return create_reg_movq((ins -> op1 -> offset)*SIZE, (ins -> result -> offset)*SIZE, RBP);
}

char * create_constant_movq(int val, int offset_val, char * reg) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tmovq $");
	sprintf(ins, "%s%d ", ins, val);
	if (offset_val != 0)
		sprintf(ins, "%s%d", ins, offset_val);
	sprintf(ins, "%s%s", ins, reg);
	//sprintf(ins, "%s\n", ins);
	return ins;
}

char * create_reg_movq(int target_offset, int source_offset, char * reg) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tmovq ");
	if (source_offset != 0)
		sprintf(ins, "%s%d", ins, source_offset);
	sprintf(ins, "%s%s ", ins, reg);
	if (target_offset != 0)
		sprintf(ins, "%s%d", ins, target_offset);
	sprintf(ins, "%s%s", ins, reg);
	//sprintf(ins, "%s\n", ins);
	return ins;	
}

char * create_assembly_label(char * id) {
	char * ins = malloc(12*sizeof(char));
	strcpy(ins, id);
	sprintf(ins, "%s:\n", id);
	return ins;
}

char * create_assembly_ret() {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tret\n");
}