#include <stdio.h>

#define RBP "(%rbp)"
#define RAX "%rax"
#define RBX "%rbx"
#define RCX "%rcx"
#define RDX "%rdx"
#define SIZE 1

FILE * assembly_file = NULL;

void initialize();
void finalize();
void create_assembly_file(InstructionNode * ins);
void generate_assembly_code(InstructionNode * ins);
void  generate_assembly_plus(InstructionNode * ins);
char * generate_assembly_minus(InstructionNode * ins);
char * generate_assembly_prod(InstructionNode * ins);
char * generate_assembly_div(InstructionNode * ins);
char * generate_assembly_mod(InstructionNode * ins);
char * create_constant_movq(int val, int offset_val, char * reg);
void create_movq_stack(int target_offset, int source_offset);
char * create_assembly_label(char * id);
char * create_assembly_ret();
void generate_assembly_begin_fun(InstructionNode * ins);
char * generate_assembly_end_fun(InstructionNode * ins);
void generate_assembly_assign(InstructionNode * ins);
void create_movq_reg_to_stack(char * reg, int dest_offset);
void create_add_reg_to_reg(char * reg, char * dest_reg);
void create_movq_to_reg(int src_offset, char * reg);