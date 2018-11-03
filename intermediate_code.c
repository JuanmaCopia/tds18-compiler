#include <stdio.h>
#include <stdlib.h>
#include "structs.h"



#define PLUS '+'
#define MINUS '-'
#define PROD '*'
#define DIV '/'
#define MOD '%'

#define ASSIGN '='
#define EQUALS 'e'

#define GREATER_THAN '>'
#define LESSER_THAN '<'

#define AND '&'
#define OR '|'
#define NOT '!'

#define TMP1 't' + '1'
#define TMP2 't' + '2'
#define CALL 'c'
#define END_FUN 'e' + 'f'

InstructionNode * head, *last;
int temp_quantity = 0;

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

/*
  Creates a new instruction.
*/
InstructionNode * create_instructionNode(int operation, VarNode * result, VarNode * op1, VarNode * op2) {
  InstructionNode * new_node = malloc(sizeof(InstructionNode));
  new_node -> operation = operation;
  new_node -> op1 = op1;
  new_node -> op2 = op2;
  new_node -> result = result;
  new_node -> next= NULL;
  new_node -> back= NULL;
  return new_node;
}

/*
  Creates a new varnode
*/
VarNode * create_var_node() {
  VarNode * new_node = malloc(sizeof(VarNode));
  new_node -> is_defined = false;
  new_node -> next = NULL;
  return new_node;
}

/*
  Creates a new temporal.
*/
VarNode * create_temporal() {
  char temp_name[128];
  sprintf(temp_name, "t%d\0", temp_quantity);
  char * res = malloc(strlen(temp_name));
  sprintf(res, temp_name);
  VarNode * new_node = create_var_node();
  new_node -> id = res;
  temp_quantity++;
  return new_node;
}

/*
  Creates a new temporal and sets its value and type.
*/
VarNode * create_temporal_with_value(int value, bool is_boolean) {
  VarNode * new_node = create_temporal();
  new_node -> value = value;
  new_node -> is_boolean = is_boolean;
  new_node -> is_defined = true;
  return new_node;
}

/*
  Creates and returns a new VarNode.
*/
VarNode * create_temporal_with_id(char * id) {
  VarNode * new_node = create_var_node();
  new_node -> id = id;
  return new_node;
}

/*
  Returns the operation of an ASTNode.
*/
int get_operation(ASTNode * node) {
  switch (node -> node_type) {
    //case _if: return IF;
    //case _if_body: return IF_BODY;
    //case _while: return WHILE;
    case _arith_op: return node -> data;
    case _boolean_op: return node -> data;
    case _assign: return ASSIGN;
    //case _method_call: return METH_CALL;
    //case _return: return RETURN;
    case _id: return TMP1;
    case _literal: return TMP2;
    default: printf("entra por el default \n");;
  }
}

/*
  Recorre el cuerpo de una funcion y crea las instrucciones en codigo iget_temporal_string(i ntermedio
*/
InstructionNode * create_instruction_from_ASTNode(ASTNode * root) {
  return create_instructionNode(get_operation(root), NULL, NULL, NULL);
}

/*
  Returns the next statement.
*/
ASTNode * get_next_statement(ASTNode * statement) {
  return statement -> next_statement;
}

/*
  Creates an instruction that represents the creation of a temporal value.
*/
InstructionNode * create_TEMP_instruction(VarNode * var_data) {
  printf("encuentra un idl \n");
  if (var_data -> is_defined)
    return create_instructionNode(TMP2, create_temporal_with_id(var_data -> id), var_data, NULL);
  else 
    return create_instructionNode(TMP1, var_data, NULL, NULL);
}

InstructionNode * create_statement_instructions(ASTNode * root);

InstructionNode * create_instruction_2op_operation(ASTNode * root) {
  printf("encuentra un operador \n");
  InstructionNode * instruction = create_instruction_from_ASTNode(root);
  instruction -> result = create_temporal();
  instruction -> op1 = create_statement_instructions(root -> left_child) -> result;
  instruction -> op2 = create_statement_instructions(root -> right_child) -> result;
  add_instruction(create_TEMP_instruction(instruction -> result));
  add_instruction(instruction);
  return instruction;
}

InstructionNode * create_instruction_assignment(ASTNode * root) {
  printf("encuentra un assign \n");
  InstructionNode * instruction = create_instruction_from_ASTNode(root);
  instruction -> result = create_statement_instructions(root -> left_child) -> result;
  instruction -> op1 = create_statement_instructions(root -> right_child) -> result;
  add_instruction(instruction);
  return instruction;
}

InstructionNode * create_instruction_literal(ASTNode * root) {
  printf("encuentra un literal \n");
  InstructionNode * instruction = create_instruction_from_ASTNode(root);
  instruction = create_TEMP_instruction(create_temporal_with_value(root -> data, root -> is_boolean));
  add_instruction(instruction);
}

/*
  Creates and adds to the list the corresponding intermediate code instructions for an statement.
*/
InstructionNode * create_statement_instructions(ASTNode * root) {
  printf("entra a la rec, este nodo es un %s\n", get_type_node_string(root->node_type));
  if (root != NULL) {
    switch (root -> node_type) {
      case _if: break;
      case _if_body: break;
      case _while: break;
      case _arith_op: case _boolean_op:
        return create_instruction_2op_operation(root);
      case _assign:
        return create_instruction_assignment(root);
      case _method_call: break;
      case _return: break;
      case _id:
        return create_TEMP_instruction(root -> var_data);
      case _literal:
        return create_instruction_literal(root);
      default: 
        printf("ERORRRRRRRRR \n");
    }
  }
  return NULL;
}

/*
  Creates the intermediate code.
*/
void generate_intermediate_code(ASTNode * root) {
  ASTNode * aux = root;
  while (aux != NULL) {
    create_statement_instructions(aux);
    aux = get_next_statement(aux);
  }
}

/*
  Given an isntruction node returns the string representation of the operation.
*/
char * get_operation_string(InstructionNode * i) {
  switch (i -> operation) {
    case PLUS:   return "PLUS";
    case PROD:   return "PROD";
    case MOD:    return "MOD";
    case DIV:    return "DIV";
    case MINUS:  return "MINS";
    case EQUALS: return "EQUL";
    case OR:     return "OR";
    case AND:    return "AND";
    case CALL:   return "CALL";
    case END_FUN:   return "ENDF";
    case TMP1:   return "TMP1";
    case TMP2:   return "TMP2";
    case GREATER_THAN: return "GRTR";
    case LESSER_THAN:  return "LSSR";
    case ASSIGN: return "ASGN";
  }
}

/*
  Returns the string representation of a temporal.
*/
char * get_temporal_string(VarNode * temp) {
  if (temp -> is_defined) {
    if (temp -> is_boolean) {
      if (temp -> value == 0)
        return "false";
      else
        return "true";
    }
    else {
      int i = temp -> value;
      char str[8];
      sprintf(str, "%d", i);
      char * ret = str;
      return ret;
    }
  }
  return temp -> id;
}

/*
  Prints a single instruction on cosole.
*/
void print_instruction(InstructionNode * i) {
  switch (i -> operation) {
    case TMP1: case CALL: case END_FUN:       // aca se imprimen instrucciones con un solo operador
      printf("%s   %s\n", get_operation_string(i), i -> result -> id);
      break;
    case TMP2: case ASSIGN:                  // aca se imprime instrucciones con 2 operadores
      printf("%s   %s  %s\n", get_operation_string(i), i -> result -> id, get_temporal_string(i -> op1));
      break;
    case PLUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN: // aca con 3 operadores
      printf("%s   %s  %s  %s\n", get_operation_string(i), i -> result -> id, i -> op1 -> id, i -> op2 -> id);
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
  while (aux != NULL) {
    if (aux -> operation == CALL)
      printf("\n\n\n =================  CODIGO INTERMEDIO DE: %s  ================= \n\n\n", aux -> result -> id);
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
    add_instruction(create_instructionNode(CALL, create_temporal_with_id(aux -> id), NULL, NULL));
    generate_intermediate_code(aux -> body);
    add_instruction(create_instructionNode(END_FUN, create_temporal_with_id(aux -> id), NULL, NULL));
    aux = aux -> next;
  }
}