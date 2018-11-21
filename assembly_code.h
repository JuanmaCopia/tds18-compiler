#include <stdio.h>

#define RBP "(%rbp)"
#define RAX "%rax"
#define RBX "%rbx"
#define RCX "%rcx"
#define RDX "%rdx"
#define AL  "%al"
#define DL	"%dl"

#define RDI "%rdi"
#define RSI "%rsi"
#define R8 "%r8"
#define R9 "%r9"

#define MOVQ "movq "
#define ADD "add  "
#define SUB "sub  "
#define IMUL "imul "
#define IDIV "idiv "
#define COMP "cmp  "
#define JUMP "jmp  "
#define JUMPE "je  "
#define JUMPNE "jne  "
#define SETG "setg"
#define SETL "setl"
#define SETE "sete"
#define PUSH_ "push "
#define POP_ "pop  "
#define CALL_ "call "

FILE * assembly_file = NULL;

void initialize();
void finalize();
void create_assembly_file(InstructionNode * ins);
void generate_assembly_code(InstructionNode * ins);
char * create_assembly_label(char * id);
char * create_asmlabel();

void create_instruction_reg_to_reg(char * instruction, char * reg1, char * reg2);
void create_instruction_stack_to_reg(char * instruction, int offset, char * reg);
void create_instruction_reg_to_stack(char * instruction, char * reg1, int offset);
void create_instruction_stack_to_stack(char * instruction, int offset1, int offset2);
void create_instruction_constant_to_stack(char * instruction, int constant, int offset);
void create_instruction_jump(char * instruction, char * label);
void create_instruction_constant_to_reg(char * instruction, int constant, char * reg);
void create_instruction_1reg(char * instruction, char * reg);
void create_instruction_1stack(char * instruction, int offset);
void create_instruction_1const(char * instruction, int constant);
void create_instruction_string(char * instruction, char * str);
void create_instruction_push_const(int constant);

void generate_assembly_begin_fun(InstructionNode * ins);
void generate_assembly_assign(InstructionNode * ins);
void generate_assembly_operation(InstructionNode * ins, char * operation_string);
void generate_assembly_and(InstructionNode * ins);
void generate_assembly_or(InstructionNode * ins);
void generate_assembly_end_fun(InstructionNode * ins);
void generate_assembly_div(InstructionNode * ins, char * reg);
void generate_assembly_bool_operation(InstructionNode * ins, char * operation_string);
void generate_assembly_compare(InstructionNode * ins);
void generate_assembly_push(InstructionNode * ins);
void generate_assembly_return(InstructionNode * ins);
void generate_assembly_extern_param_passage(InstructionNode * ins, char * reg);