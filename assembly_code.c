#include "assembly_code.h"

void initialize() {
	assembly_file = fopen("assembly_code.s" ,"w");
}

void finalize() {
	fclose(assembly_file);
}

void create_assembly_file(InstructionNode * ins) {
	if (assembly_file == NULL)
		initialize();

	if (ins != NULL) {
		//print_instruction(ins);
		//fprintf(assembly_file, "%s\n", generate_assembly_code(ins));
		generate_assembly_code(ins);
	}

	if (ins -> next == NULL)
		finalize();
	else
		create_assembly_file(ins -> next);
}

void generate_assembly_code(InstructionNode * ins) {
	if (ins != NULL)
		switch (ins -> operation) {
			case PLUS:
				generate_assembly_plus(ins);
				break;
			case MINUS:
				//return generate_assembly_minus(ins);
				break;
			case PROD:
				//return generate_assembly_prod(ins);
				break;
			case DIV:
				//return generate_assembly_div(ins);
				break;
			case MOD:
				//return generate_assembly_mod(ins);
				break;
			case BEGIN_FUN:
				generate_assembly_begin_fun(ins);
				break;
			case END_FUN:
				//return generate_assembly_end_fun(ins);
				break;
			case ASSIGN:
				generate_assembly_assign(ins);
				break;
		}
} 

void generate_assembly_plus(InstructionNode * ins) {
	create_movq_to_reg(ins -> op1 -> offset, RAX);
	create_movq_to_reg(ins -> op2 -> offset, RDX);
	create_add_reg_to_reg(RDX, RAX);
	create_movq_reg_to_stack(RAX, ins -> result -> offset);
}

void create_movq_reg_to_stack(char * reg, int dest_offset) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tmovq %s %d(%rbp)", reg, dest_offset);
	fprintf(assembly_file, "%s\n", ins);
}

void create_add_reg_to_reg(char * reg, char * dest_reg) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tadd %s %s", reg, dest_reg);
	fprintf(assembly_file, "%s\n", ins);
}

void generate_assembly_begin_fun(InstructionNode * ins) {
	char * res = malloc(24*sizeof(char));
	if (ins -> result -> id == "main")
		fprintf(assembly_file, "	.globl main\n");
	sprintf(res, "%s", create_assembly_label(ins -> result -> id));
	sprintf(res, "%s\tenter", res);
	sprintf(res, "%s $(%d), $0", res, ins -> result -> offset * -1);
	fprintf(assembly_file, "%s\n", res);
}

// char * generate_assembly_end_fun(InstructionNode * ins) {
// 	return create_assembly_ret();
// }

void generate_assembly_assign(InstructionNode * ins) {
	create_movq_stack(ins -> op1 -> offset, ins -> result -> offset);
}

// char * create_constant_movq(int val, int offset_val, char * reg) {
// 	char * ins = malloc(12*sizeof(char));
// 	sprintf(ins, "\tmovq $");
// 	sprintf(ins, "%s%d ", ins, val);
// 	if (offset_val != 0)
// 		sprintf(ins, "%s%d", ins, offset_val);
// 	sprintf(ins, "%s%s", ins, reg);
// 	//sprintf(ins, "%s\n", ins);
// 	return ins;
// }

void create_movq_stack(int src_offset, int dest_offset) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tmovq %d(%rbp) %d(%rbp)", src_offset, dest_offset);
	fprintf(assembly_file, "%s\n", ins);
}

void create_movq_to_reg(int src_offset, char * reg) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "\tmovq %d(%rbp) %s", src_offset, reg);
	fprintf(assembly_file, "%s\n", ins);
}

char * create_assembly_label(char * id) {
	char * ins = malloc(12*sizeof(char));
	sprintf(ins, "%s:\n", id);
	return ins;
}

// char * create_assembly_ret() {
// 	char * ins = malloc(12*sizeof(char));
// 	sprintf(ins, "\tret\n");
// }