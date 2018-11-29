#include "structs.h"

InstructionNode * head, *last;

int temp_quantity = 0;
int max_offset_current_function;
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
  increases the max_offset of the current function.
*/
void increase_function_maxoffset() {
  max_offset_current_function = max_offset_current_function - 8;
}

/*
  Creates a new temporal.
*/
VarNode * create_temporal() {
  char temp_name[128];
  sprintf(temp_name, "t%d\0", temp_quantity);
  char * res = malloc(strlen(temp_name));
  sprintf(res, "%s", temp_name);
  VarNode * new_node = create_var_node();
  increase_function_maxoffset();
  new_node -> offset = max_offset_current_function;
  new_node -> string_offset = create_string_offset(new_node -> offset);
  printf("string offset de temporal generado: %s\n", new_node -> string_offset);
  new_node -> id = res;
  new_node -> kind = _temporal;
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
  //printf("encuentra un assign \n");
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
  //printf("encuentra un if \n");
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
  //printf("encuentra un while \n");
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
  Creates the needed instructions to compute a return statement.
*/
InstructionNode * create_instruction_return(ASTNode * root) {
  //printf("encuentra un return \n");
  InstructionNode * return_ins;
  if (root -> right_child != NULL)
    return_ins = create_instructionNode(RETURN, create_instructions(root -> right_child) -> result, NULL, NULL);
  else 
    return_ins = create_instructionNode(BREAK, create_temporal(), NULL, NULL);
  add_instruction(return_ins);
  return return_ins;
}

/*
  Returns the amount of parameters on the list.
*/
int count_params(ASTNode * param_list) {
  int amount = 0;
  ASTNode * aux = param_list;
  while (aux != NULL) {
    amount++;
    aux = aux -> next_statement;
  }
  return amount;
}

/*
  Creates the needed instructions to make a push instruction.
*/
void create_push_instructions(ASTNode * parameters) {
  //printf("pushea parametro \n");
  int amount_parameters = count_params(parameters);
  ASTNode * reverse[amount_parameters];
  ASTNode * aux = parameters;
  int i = amount_parameters - 1;
  while (aux != NULL) { 
    reverse[i] = aux;
    i--;
    aux = aux -> next_statement; 
  }
  printf("\n\nel valor de i es: %d\n\n",i);
  i = 0;
  while ( i < amount_parameters) {
    add_instruction(create_instructionNode(PUSH, create_instructions(reverse[i]) -> result, NULL, NULL));
    i++;
  }
}


/*
  Creates a pop instruction for each parameter.
*/
void create_pop_instructions(ASTNode * parameters) {
  //printf("popea parametro \n");
  ASTNode * aux = parameters;
  while (aux != NULL) {
    add_instruction(create_instructionNode(POP, NULL , NULL, NULL));
    aux = aux -> next_statement; 
  }
}

/*
  Creates the needed instructions to make a push instruction.
*/
void create_extern_push_instructions(ASTNode * parameters, int param_amount) {
  //printf("pushea parametro \n");
  int counter = 0;
  ASTNode * aux = parameters;
  while (aux != NULL && counter < 6) {
    add_instruction(create_instructionNode(PUSH1 + counter, create_instructions(aux) -> result, NULL, NULL));
    aux = aux -> next_statement; 
    counter++;
  }
  if (param_amount > 6)
    create_push_instructions(aux);
}

/*
  Creates a pop instruction for each parameter.
*/
void create_extern_pop_instructions(ASTNode * parameters, int param_amount) {
  //printf("popea parametro \n");
  int counter = 0;
  if (param_amount > 6) {
    ASTNode * aux = parameters;
    while (aux != NULL && counter < 6) {
      aux = aux -> next_statement;
      counter++;
    }
    while (aux != NULL) {
      add_instruction(create_instructionNode(POP, NULL , NULL, NULL));
      aux = aux -> next_statement; 
    }
  }
}

/*
  Creates the needed instructions to make a call to a method.
*/
InstructionNode * create_instructions_method_call(ASTNode * root) {
  //printf("encuentra un method_call \n");
  InstructionNode * call;
  if (root -> function_data -> is_extern) {
    create_extern_push_instructions(root -> right_child, root -> function_data -> param_amount);
    call = create_instructionNode(CALL, create_temporal(), create_temporal_with_id(root -> function_data -> id), NULL);
    add_instruction(call);
    create_extern_pop_instructions(root -> right_child, root -> function_data -> param_amount);
  }
  else {
    create_push_instructions(root -> right_child);
    call = create_instructionNode(CALL, create_temporal(), create_temporal_with_id(root -> function_data -> id), NULL);
    add_instruction(call);
    create_pop_instructions(root -> right_child);
  }
  return call;
}

/*
  Creates and adds to the list the corresponding intermediate code instructions for an statement.
*/
InstructionNode * create_instructions(ASTNode * root) {
  printf("entra a la rec, este nodo es un %s\n", get_type_node_string(root->node_type));
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
        printf("por crear id ins \n");
        return create_instructionNode(PSEUDO, root -> var_data, NULL, NULL);
        printf("id instruction creada \n");
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
    printf("crea instruccion \n");
    last_inst_created = create_instructions(aux);
    aux = aux -> next_statement;
  }
}

/*
  Prints VarNode information.
*/
void print_varnode(VarNode * var) {
  switch (var -> kind) {
    case _local: case _parameter:
      printf("                                              %s  %s  offset: %d\n", var -> id, get_varnode_kind_string(var), var -> offset);
      break;
    case _temporal:
      printf("                                              %s  %s  offset: %d    value: %s\n", var -> id, get_varnode_kind_string(var), var -> offset, get_temporal_string(var));
      break;
    case _label:
        printf("                          %s  %s\n", var -> id, get_varnode_kind_string(var));
      break;
    case _global:
      printf("                                              %s  %s  offset: %s\n", var -> id, get_varnode_kind_string(var), var -> string_offset);
      break;
    default:
      printf(" ERROR, unknown varnode type \n");
      break;
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
    case BEGIN_FUN:
      printf("\n\n========================  START OF INSTRUCTIONS OF A NEW FUNCTION  ======================\n\n");
      printf("\t%s   %s", get_operation_string(i), result_string);
      //print_varnode(i -> result);
      printf("                          %s  %s   max_offset: %d\n", i -> result -> id, get_varnode_kind_string(i -> result), i -> result -> offset);
      printf("\n\n");
      break;
    case END_FUN:
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      print_varnode(i -> result);
      break;
    case LABEL:
      printf("%s\n", result_string);
      //print_varnode(i -> result);
      break;
    case RETURN:
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      print_varnode(i -> result);
      break;
    case PUSH: case PUSH1: case PUSH2: case PUSH3: case PUSH4: case PUSH5: case PUSH6:
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      print_varnode(i -> result);
      break;
    case ASSIGN: case NEGAT: case CALL: 
      printf("\t%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      print_varnode(i -> result);
      print_varnode(i -> op1);
      break;
    case PLUS: case MINUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN: case CMP:
      printf("\t%s   %s  %s  %s \n", get_operation_string(i), result_string, op1_string, op2_string);
      print_varnode(i -> result);
      print_varnode(i -> op1);
      print_varnode(i -> op2);
      break;
    case JMP: 
      printf("\t%s   %s\n", get_operation_string(i), result_string);
      //print_varnode(i -> result);
      break;
    case JE:
      printf("\t%s   %s  %s\n", get_operation_string(i), result_string, op1_string);
      //print_varnode(i -> result);
      print_varnode(i -> op1);
      break;
    case POP: case BREAK:
      printf("\t%s\n\n", get_operation_string(i));
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
void generate_functions_intermediate_code(FunctionNode * head) {
  FunctionNode * aux = head;
  while (aux != NULL) {
    InstructionNode * begin_ins = NULL;
    max_offset_current_function = aux -> max_offset;
    printf("FUNCION: %s   max_offset without temporals: %d\n", aux -> id, aux -> max_offset);
    if (!aux -> is_extern) {
      begin_ins = create_instructionNode(BEGIN_FUN, create_label_with_id(aux -> id), NULL, NULL);
      add_instruction(begin_ins);
      generate_intermediate_code(aux -> body, aux -> id);
      add_instruction(create_instructionNode(END_FUN, create_label_with_id(aux -> id), NULL, NULL));
      aux -> max_offset = max_offset_current_function;
      begin_ins -> result -> offset = max_offset_current_function;
    }
    printf("FUNCION: %s   Final max_offset: %d \n\n", aux -> id, aux -> max_offset);
    aux = aux -> next;
  }
}
