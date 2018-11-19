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
	if (ins != NULL)
		generate_assembly_code(ins);
	if (ins -> next == NULL)
		finalize();
	else
		create_assembly_file(ins -> next);
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
				generate_assembly_operation(ins, IDIV);
				break;
			case AND:
				generate_assembly_and(ins);
				break;
			case OR:
				generate_assembly_or(ins);
				break;
			case GREATER_THAN:
				break;
			case LESSER_THAN:
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

void generate_assembly_operation(InstructionNode * ins, char * operation_string) {
	if (ins -> op1 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op1 -> value, ins -> op1 -> offset);
	if (ins -> op2 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op2 -> value, ins -> op2 -> offset);
	create_instruction_stack_to_reg(MOVQ, ins -> op1 -> offset, RAX);
	create_instruction_stack_to_reg(MOVQ, ins -> op2 -> offset, RDX);
	create_instruction_reg_to_reg(operation_string, RDX, RAX);
	create_instruction_reg_to_stack(MOVQ, RAX, ins -> result -> offset);
}

void generate_assembly_and(InstructionNode * ins) {
	char * label_false = ".false_label";
	char * label_true = ".true_label";
	if (ins -> op1 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op1 -> value, ins -> op1 -> offset);
	if (ins -> op2 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op2 -> value, ins -> op2 -> offset);
	create_instruction_stack_to_reg(MOVQ, ins -> op1 -> offset, RDX);
	create_instruction_constant_to_reg(COMP, 0, RDX);
	create_instruction_jump(JUMPE, label_false);
	create_instruction_stack_to_reg(MOVQ, ins -> op2 -> offset, RDX);
	create_instruction_constant_to_reg(COMP, 0, RDX);
	create_instruction_jump(JUMPE, label_false);
	create_instruction_jump(JUMP, label_true);
	create_assembly_label(label_false);
	create_instruction_constant_to_stack(MOVQ, 0, ins -> result -> offset);
	create_assembly_label(label_true);
	create_instruction_constant_to_stack(MOVQ, 1, ins -> result -> offset);
}

void generate_assembly_or(InstructionNode * ins) {
	char * label_false = ".false_label";
	char * label_true = ".true_label";
	if (ins -> op1 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op1 -> value, ins -> op1 -> offset);
	if (ins -> op2 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op2 -> value, ins -> op2 -> offset);
	create_instruction_stack_to_reg(MOVQ, ins -> op1 -> offset, RAX);
	create_instruction_stack_to_reg(MOVQ, ins -> op2 -> offset, RDX);
	create_instruction_reg_to_reg(ADD, RDX, RAX);
	create_instruction_constant_to_reg(COMP, 0, RAX);
	create_instruction_jump(JUMPE, label_false);
	create_instruction_jump(JUMP, label_true);
	create_assembly_label(label_false);
	create_instruction_constant_to_stack(MOVQ, 0, ins -> result -> offset);
	create_assembly_label(label_true);
	create_instruction_constant_to_stack(MOVQ, 1, ins -> result -> offset);
}

void generate_assembly_assign(InstructionNode * ins) {
	if (ins -> op1 -> is_defined)
		create_instruction_constant_to_stack(MOVQ, ins -> op1 -> value, ins -> op1 -> offset);
	create_instruction_stack_to_stack("movq ", ins -> op1 -> offset, ins -> result -> offset);
}

void generate_assembly_begin_fun(InstructionNode * ins) {
	if (ins -> result -> id == "main")
		fprintf(assembly_file, "	.globl main\n");
	create_assembly_label(ins -> result -> id);
	fprintf(assembly_file, "\tenter\t$(%d), $0\n", ins -> result -> offset * -1);
}

char * create_assembly_label(char * id) {
	fprintf(assembly_file, "%s:\n", id);
}

void create_instruction_reg_to_reg(char * instruction, char * reg1, char * reg2) {
	fprintf(assembly_file, "\t%s\t%s, %s \n", instruction, reg1, reg2);
}

void create_instruction_stack_to_reg(char * instruction, int offset, char * reg) {
	fprintf(assembly_file, "\t%s\t%d(%rbp), %s \n", instruction, offset, reg);
}

void create_instruction_reg_to_stack(char * instruction, char * reg1, int offset) {
	fprintf(assembly_file, "\t%s\t%s, %d(%rbp) \n", instruction, reg1, offset);
}

void create_instruction_stack_to_stack(char * instruction, int offset1, int offset2) {
	fprintf(assembly_file, "\t%s\t%d(%rbp), %d(%rbp) \n", instruction, offset1, offset2);
}

void create_instruction_constant_to_stack(char * instruction, int constant, int offset) {
	fprintf(assembly_file, "\t%s\t$%d, %d(%rbp) \n", instruction, constant, offset);
}

void create_instruction_constant_to_reg(char * instruction, int constant, char * reg) {
	fprintf(assembly_file, "\t%s\t$%d, %s \n", instruction, constant, reg);
}

void create_instruction_jump(char * instruction, char * label) {
	fprintf(assembly_file, "\t%s\t%s \n", instruction, label);
}