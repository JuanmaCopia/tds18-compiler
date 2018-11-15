#include <stdio.h>

#define RBP "(%rbp)"
#define RAX "(%rax)"
#define SIZE 1

FILE * assembly_file = NULL;


void initialize();
void finalize();
void create_assembly_file(InstructionNode * ins);
char * generate_assembly_code(InstructionNode * ins);
char * generate_assembly_plus(InstructionNode * ins);
char * generate_assembly_minus(InstructionNode * ins);
char * generate_assembly_prod(InstructionNode * ins);
char * generate_assembly_div(InstructionNode * ins);
char * generate_assembly_mod(InstructionNode * ins);
char * create_constant_movq(int val, int offset_val, char * reg);
char * create_reg_movq(int target_offset, int source_offset, char * reg);
char * create_assembly_label(char * id);
char * create_assembly_ret();
char * generate_assembly_begin_fun(InstructionNode * ins);
char * generate_assembly_end_fun(InstructionNode * ins);
char * generate_assembly_assign(InstructionNode * ins);