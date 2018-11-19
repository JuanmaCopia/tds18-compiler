#include <stdio.h>

#define RBP "(%rbp)"
#define RAX "%rax"
#define RBX "%rbx"
#define RCX "%rcx"
#define RDX "%rdx"

#define MOVQ "movq "
#define ADD "add  "
#define SUB "sub  "
#define IMUL "imul "
#define IDIV "idiv "
#define COMP "cmp  "
#define JUMP "jmp  "
#define JUMPE "je  "

FILE * assembly_file = NULL;

void initialize();
void finalize();
void create_assembly_file(InstructionNode * ins);
void generate_assembly_code(InstructionNode * ins);
void create_movq_stack(int target_offset, int source_offset);
char * create_assembly_label(char * id);
char * create_assembly_ret();
void generate_assembly_begin_fun(InstructionNode * ins);



void create_instruction_reg_to_reg(char * instruction, char * reg1, char * reg2);
void create_instruction_stack_to_reg(char * instruction, int offset, char * reg);
void create_instruction_reg_to_stack(char * instruction, char * reg1, int offset);
void create_instruction_stack_to_stack(char * instruction, int offset1, int offset2);
void create_instruction_constant_to_stack(char * instruction, int constant, int offset);
void create_instruction_jump(char * instruction, char * label);
void create_instruction_constant_to_reg(char * instruction, int constant, char * reg);

void generate_assembly_assign(InstructionNode * ins);
void generate_assembly_operation(InstructionNode * ins, char * operation_string);
void generate_assembly_and(InstructionNode * ins);
void generate_assembly_or(InstructionNode * ins);