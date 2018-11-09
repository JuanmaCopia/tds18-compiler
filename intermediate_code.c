#include "structs.h"

InstructionNode * head, *last;

char * get_type_node_string(TypeNode tn);
InstructionNode * create_instructions(ASTNode * root);
void print_instruction(InstructionNode * i);

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

/*
  Creates the needed instructions to compute an operation.
*/
InstructionNode * create_instruction_operation(ASTNode * root) {
  //printf("encuentra un operador: %c \n",root -> data);
  InstructionNode * instruction;
  if (root -> left_child == NULL) {
    if (root -> data == '-') {
      instruction = create_instructionNode(root -> data, create_temporal(), NULL, NULL);
      instruction -> op1 = create_temporal_with_value(0, false);
      instruction -> op2 = create_instructions(root -> right_child) -> result;
    }
    else {
      instruction = create_instructionNode(NEGAT, create_temporal(), NULL, NULL);
      instruction -> op1 = create_instructions(root -> right_child) -> result;
    }
  }
  else {
    instruction = create_instructionNode(root -> data, create_temporal(), NULL, NULL);
    instruction -> op1 = create_instructions(root -> left_child) -> result;
    instruction -> op2 = create_instructions(root -> right_child) -> result;
  }
  add_instruction(instruction);
  return instruction;
}

/*
  Creates the needed instructions to compute an assignment statement.
*/
InstructionNode * create_instruction_assignment(ASTNode * root) {
  printf("encuentra un assign \n");
  InstructionNode * instruction = create_instructionNode(ASSIGN, NULL, NULL, NULL);
  instruction -> result = create_instructions(root -> left_child) -> result;
  instruction -> op1 = create_instructions(root -> right_child) -> result;
  add_instruction(instruction);
  return instruction;
}

/*
  Creates the instructions of a sequense of statements.
*/
void add_statements_block_instructions(ASTNode * root) {
  ASTNode * aux = root;
  while (aux != NULL) {
    create_instructions(aux);
    aux = aux -> next_statement;
  }
}

/*
  Creates the needed instructions to compute an if statement.
*/
void create_instructions_if(ASTNode * root) {
  printf("encuentra un if \n");
  VarNode * cmp_temp = create_temporal();
  InstructionNode * condition = create_instructions(root -> left_child);
  add_instruction(create_instructionNode(CMP, cmp_temp, condition -> result, create_temporal_with_value(0, false)));
  InstructionNode * else_label_ins = create_instructionNode(LABEL, create_label(), NULL, NULL);
  InstructionNode * end_if_label_ins = create_instructionNode(LABEL, create_label(), NULL, NULL);
  add_instruction(create_instructionNode(JE, else_label_ins -> result, cmp_temp, NULL));
  add_statements_block_instructions(root -> right_child -> left_child);
  add_instruction(create_instructionNode(JMP, end_if_label_ins -> result, NULL, NULL));
  add_instruction(else_label_ins);
  add_statements_block_instructions(root -> right_child -> right_child);
  add_instruction(end_if_label_ins);
}

/*
  Creates the needed instructions to compute a while statement.
*/
void create_instructions_while(ASTNode * root) {
  printf("encuentra un while \n");
  VarNode * cmp_temp = create_temporal();
  InstructionNode * begin_label_ins = create_instructionNode(LABEL, create_label(), NULL, NULL);
  add_instruction(begin_label_ins);
  InstructionNode * condition = create_instructions(root -> left_child);
  add_instruction(create_instructionNode(CMP, cmp_temp, condition -> result, create_temporal_with_value(0, false)));
  InstructionNode * end_label_ins = create_instructionNode(LABEL, create_label(), NULL, NULL);
  add_instruction(create_instructionNode(JE, end_label_ins -> result, cmp_temp, NULL));
  add_statements_block_instructions(root -> right_child);
  add_instruction(create_instructionNode(JMP, begin_label_ins -> result, NULL, NULL));
  add_instruction(end_label_ins);
}

/*
  Creates the needed instructions to compute a return statement
*/
InstructionNode * create_instruction_return(ASTNode * root) {
  printf("encuentra un return \n");
  InstructionNode * return_ins = create_instructionNode(RETURN, create_instructions(root -> right_child) -> result, NULL, NULL);
  add_instruction(return_ins);
  return return_ins;
}

/*
  Creates the needed instructions to make a push instruction.
*/
void create_push_instructions(ASTNode * parameters) {
  printf("pushea parametros \n");
  ASTNode * aux = parameters;
  while (aux != NULL) {
    add_instruction(create_instructionNode(PUSH, create_instructions(aux) -> result, NULL, NULL));
    aux = aux -> right_child; 
  }
}

InstructionNode * create_instructions_method_call(ASTNode * root) {
  printf("encuentra un method_call \n");
  create_push_instructions(root -> right_child);
  InstructionNode * call_ins = create_instructionNode(CALL, create_temporal(), create_temporal_with_id(root -> function_data -> id), NULL);
  add_instruction(call_ins);
  return call_ins;
}

/*
  Creates and adds to the list the corresponding intermediate code instructions for an statement.
*/
InstructionNode * create_instructions(ASTNode * root) {
  //printf("entra a la rec, este nodo es un %s\n", get_type_node_string(root->node_type));
  if (root != NULL) {
    switch (root -> node_type) {
      case _if:
        create_instructions_if(root);
        break;
      case _while:
        create_instructions_while(root); 
        break;
      case _arith_op: case _boolean_op:
        return create_instruction_operation(root);
      case _assign:
        return create_instruction_assignment(root);
      case _method_call:
        return create_instructions_method_call(root);
        break;
      case _return:
        return create_instruction_return(root);
        break;
      case _id:
        return create_instructionNode(PSEUDO, root -> var_data, NULL, NULL);
      case _literal:
        return create_instructionNode(PSEUDO, create_temporal_with_value(root -> data, root -> is_boolean), NULL, NULL);
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
    aux = aux -> next_statement;
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
    case END_FUN:
      printf("\t%s   %s\n\n", get_operation_string(i), result_string);
      break;
    case LABEL:
      printf("%s\n", result_string);
      break;
    case BEGIN_FUN: case RETURN:
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      break;
    case PUSH: case POP: case JMP: 
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      break;
    case ASSIGN: case JE: case NEGAT: case CALL: 
      printf("\t%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      break;
    case PLUS: case MINUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN: case CMP:
      printf("\t%s   %s  %s  %s \n", get_operation_string(i), result_string, op1_string, op2_string);
      break;
    default:
      printf("\tUNKNOWN INSTRUCTION\n");
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