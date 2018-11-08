#include "structs.h"

InstructionNode * head, *last;
char * get_type_node_string(TypeNode tn);

/*
  Adds an instruction to the list.
*/
void add_instruction(InstructionNode * node) {
  if (head != NULL && last != NULL) {
    last -> next = node;
    node -> back = last;
    last = node;
  }
  else {
    head = node;
    last = node;
  }
}


InstructionNode * create_instructions(ASTNode * root);
void print_instruction(InstructionNode * i);

InstructionNode * create_instruction_operation(ASTNode * root) {
  printf("encuentra un operador: %c \n",root -> data);
  InstructionNode * instruction = create_instructionNode(root -> data, NULL, NULL, NULL);
  instruction -> result = create_temporal();
  instruction -> op1 = create_instructions(root -> left_child) -> result;
  printf("de la operacion: %c, el op1 es: %s \n",root -> data, get_temporal_string(instruction -> op1));
  instruction -> op2 = create_instructions(root -> right_child) -> result;
  printf("de la operacion: %c, el op2 es: %s \n",root -> data, get_temporal_string(instruction -> op2));
  add_instruction(instruction);
  return instruction;
}

InstructionNode * create_instruction_assignment(ASTNode * root) {
  printf("encuentra un assign \n");
  InstructionNode * instruction = create_instructionNode(ASSIGN, NULL, NULL, NULL);
  instruction -> result = create_instructions(root -> left_child) -> result;
  instruction -> op1 = create_instructions(root -> right_child) -> result;
  add_instruction(instruction);
  return instruction;
}

/*
  Creates and adds to the list the corresponding intermediate code instructions for an statement.
*/
InstructionNode * create_instructions(ASTNode * root) {
  //printf("entra a la rec, este nodo es un %s\n", get_type_node_string(root->node_type));
  if (root != NULL) {
    switch (root -> node_type) {
      case _if: 
        break;
      case _if_body: break;
      case _while: 
        break;
      case _arith_op: case _boolean_op:
        return create_instruction_operation(root);
      case _assign:
        return create_instruction_assignment(root);
      case _method_call: 
        break;
      case _return: 
        break;
      case _id:
        printf("encuentra un id: %s \n",get_temporal_string(root->var_data));
        return create_instructionNode(PSEUDO, root -> var_data, NULL, NULL);
      case _literal:
        printf("encuentra un literal: %s \n", get_temporal_string(create_temporal_with_value(root -> data, root -> is_boolean)));
        InstructionNode * result = create_instructionNode(PSEUDO, create_temporal_with_value(root -> data, root -> is_boolean), NULL, NULL);
        print_instruction(result);
        return result;
      default: 
        printf("ERORRRRRRRRR \n");
    }
  }
  return NULL;
}

/*
  Creates the intermediate code.
*/
void generate_intermediate_code(ASTNode * root, char * fun_name) {
  ASTNode * aux = root;
  InstructionNode * last_inst_created;
  while (aux != NULL) {
    last_inst_created = create_instructions(aux);
    aux = get_next_statement(aux);
  }
}

/*
  Prints a single instruction on cosole.
*/
void print_instruction(InstructionNode * i) {
  char op1_string[12];
  char op2_string[12];
  char result_string[12];
  strcpy(op1_string, get_temporal_string(i -> op1));
  strcpy(op2_string, get_temporal_string(i -> op2));
  strcpy(result_string, get_temporal_string(i -> result));
  switch (i -> operation) {
    case LABEL: case BEGIN_FUN: case PSEUDO:
      printf("%s   %s\n", get_operation_string(i), result_string);
      break;
    case CALL: case END_FUN: case PUSH: case POP: case JMP: 
      printf("%s   %s\n", get_operation_string(i), result_string);
      break;
    case ASSIGN:
      printf("%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      break;
    case PLUS: case MINUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN:
      printf("%s   %s  %s  %s \n", get_operation_string(i), result_string, op1_string, op2_string);
      break;
    default:
      printf("UNKNOWN INSTRUCTION\n");
      break;
  }
}

/*
  Prints all the instructions on console.
*/
void print_instructions() {
  InstructionNode * aux = head;
  printf("\n\n");
  while (aux != NULL) {
    print_instruction(aux);
    aux = aux -> next;
  }
  printf("\n\n\n");
}

/*
  Generates the intermediate code for all functions of the program.
*/
void generate_fun_code(FunctionNode * head) {
  FunctionNode * aux = head;
  while (aux != NULL) {
    add_instruction(create_instructionNode(BEGIN_FUN, create_temporal_with_id(aux -> id), NULL, NULL));
    generate_intermediate_code(aux -> body, aux -> id);
    add_instruction(create_instructionNode(END_FUN, create_temporal_with_id(aux -> id), NULL, NULL));
    aux = aux -> next;
  }
}