#include <stdio.h>

#define RAX "%rax"
#define EAX "%eax"
#define RBX "%rbx"
#define RCX "%rcx"
#define RDX "%rdx"
#define AL  "%al"
#define DL	"%dl"

#define RDI "%rdi"
#define RSI "%rsi"
#define R8 "%r8"
#define R9 "%r9"
#define R10 "%r10"
#define R11 "%r11"
#define RIP "%rip"

#define MOVQ "movq "
#define ADD "add  "
#define SUB "sub  "
#define IMUL "imul "
#define IDIVQ "idivq"
#define CQTO "cqto "
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
#define XORQ "xorq "

FILE * assembly_file = NULL;

void initialize();
void finalize();
void create_assembly_file(InstructionNode * ins, VarNode * global_variables);
void generate_assembly_code(InstructionNode * ins);

char * create_assembly_label(char * id);
char * create_asmlabel();
char * get_current_end_asmlabel();
char * create_end_asmlabel();
char * create_constant_string(int constant);
char * create_bool_string_const(bool value);

void create_instruction_2op(char * instruction, char * op1, char * op2);
void create_instruction_1op(char * instruction, char * op1);

void generate_assembly_global_variables(VarNode * variables);
void generate_assembly_begin_fun(InstructionNode * ins);
void generate_assembly_assign(InstructionNode * ins);
void generate_assembly_minus(InstructionNode * ins);
void generate_assembly_plus(InstructionNode * ins);
void generate_assembly_prod(InstructionNode * ins);
void generate_assembly_and(InstructionNode * ins);
void generate_assembly_or(InstructionNode * ins);
void generate_assembly_end_fun(InstructionNode * ins);
void generate_assembly_div(InstructionNode * ins);
void generate_assembly_mod(InstructionNode * ins);
void generate_assembly_greater(InstructionNode * ins);
void generate_assembly_lesser(InstructionNode * ins);
void generate_assembly_equals(InstructionNode * ins);
void generate_assembly_compare(InstructionNode * ins);
void generate_assembly_push(InstructionNode * ins);
void generate_assembly_return(InstructionNode * ins);
void generate_assembly_extern_param_passage(InstructionNode * ins, char * reg);
void generate_assembly_not(InstructionNode * ins);


void set_value_on_register(VarNode * op, char * reg);
void create_optimized_instruction(char * instruction, VarNode * op, char * reg);
void create_optimized_instruction_1op(char * instruction, VarNode * op);