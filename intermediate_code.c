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

#define LABEL 'l'
#define TMP1 't' + '1'
#define TMP2 't' + '2'
#define CALL 'c'
#define END_FUN 'e' + 'f'
#define PUSH 'p' + 'u'
#define POP 'p' + 'o'

#define JMP 'j'
#define JZ 'j' + 'z' 
#define JNZ 'j' + 'n' + 'z'
#define JE  'j' + 'e'
#define JNE 'j' + 'n' + 'e'
#define JG  'j' + 'g'
#define JL  'j' + 'l'
#define JGE 'j' + 'g' + 'e'
#define JLE 'j' + 'l' + 'e'

InstructionNode * head, *last;
int temp_quantity = 0;
int label_quantity = 0;
//This is a queue of function labels where can be stored and added to instructions list when necessary
InstructionNode * fun_label_queue = NULL;

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

VarNode * create_label() {
  char label_name[128];
  sprintf(label_name, "l%d\0", label_quantity);
  char * res = malloc(strlen(label_name));
  sprintf(res, label_name);
  VarNode * new_node = create_var_node();
  new_node -> id = res;
  label_quantity++;
  return new_node;
}

VarNode * create_label_with_id(char * id) {
  VarNode * new_node = create_var_node();
  new_node -> id = id;
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

void add_label_to_queue(InstructionNode * fun_label) {
  if (fun_label_queue == NULL)
    fun_label_queue = fun_label;
  else {
    fun_label -> next = fun_label_queue;
    fun_label_queue -> back = fun_label;
    fun_label_queue = fun_label;
  }
}

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
  This function add a instruction from queue to the intermediate-code list of instructions
*/
void add_instruction_from_queue(InstructionNode * fun_label) {
  InstructionNode * pre, * next;
  pre = fun_label -> back;
  next = fun_label -> next;
  if (pre != NULL)
    pre -> next = next;
  if (next != NULL)
    next -> back = pre;
  fun_label -> next = NULL;
  fun_label -> back = NULL;
  add_instruction(fun_label);
}

/*
  This function search for a label in the intermediat-code list or in the queue and returns its instructions
*/
InstructionNode * find_label(char * id) {
  InstructionNode * aux = head;
  while (aux != NULL) {

    if ((strcmp(aux -> result -> id, id) == 0) && (aux -> operation == LABEL)) {
      return aux;
    }
    aux = aux -> next;
  }
  //The label is not in the added instructions.- It may be in queque
  aux = fun_label_queue;
  while (aux != NULL) {
    if ((strcmp(aux -> result -> id, id) == 0) && (aux -> operation == LABEL)) {
      return aux;
    }
    aux = aux -> next;
  }
  return NULL;
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

int get_jump_instruction_from_operation(int op) {
  switch(op) {
    case EQUALS: return JNE;
    case GREATER_THAN: return JLE;
    case LESSER_THAN: return JGE;
    case AND: return JZ;
    case OR: return JZ;
    case NOT: return JZ;
    default: return JZ;
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

/*
  Creates an instruction that represents the creation of a label
*/
InstructionNode * create_LABEL_instruction(VarNode * var_data) {
  return create_instructionNode(LABEL, var_data, NULL, NULL);
}

/*
  Creates an instruction that represents a PUSH
*/
InstructionNode * create_PUSH_instruction(VarNode * var_data) {
  return create_instructionNode(PUSH, var_data, NULL, NULL);
}

/*
  Creates an instruction that represents a POP
*/
InstructionNode * create_POP_instruction(VarNode * var_data) {
  return create_instructionNode(POP, var_data, NULL, NULL);
}

InstructionNode * create_statement_instructions(ASTNode * root);

InstructionNode * create_instruction_conditional_jump(int operation, VarNode * jmp_target, VarNode * cond_result) {
  InstructionNode * condition_inst = create_instructionNode(operation, jmp_target, cond_result, NULL);
  return condition_inst;
}

void add_then_instructions(ASTNode * root) {
  create_statement_instructions(root -> right_child -> left_child);
}

void add_else_instructions(ASTNode * root) {
  create_statement_instructions(root -> right_child -> right_child);
}

void add_while_instructions(ASTNode * root) {
  create_statement_instructions(root -> right_child);
}

void create_instruction_if(ASTNode * root) {
  InstructionNode * condition_expr = create_statement_instructions(root -> left_child);
  //Condition_expr_added
  InstructionNode * condition_jmp = create_instruction_conditional_jump(get_jump_instruction_from_operation(condition_expr -> operation), NULL, condition_expr -> result);
  InstructionNode * else_label = create_LABEL_instruction(create_label());
  InstructionNode * end_if_label = create_LABEL_instruction(create_label());
  condition_jmp -> result = else_label -> result;
  add_instruction(condition_jmp);
  //condition_jmp_added
  add_then_instructions(root);
  //THEN_instructions_added
  add_instruction(create_instruction_conditional_jump(JMP, end_if_label -> result, NULL));
  //added_if_escape
  add_instruction(else_label);
  //added_ELSE_label
  add_else_instructions(root);
  //ELSE_instructions_added
  add_instruction(end_if_label);
  //added end_of_if_label
}

void create_instruction_while(ASTNode * root) {
  InstructionNode * init_of_while = create_LABEL_instruction(create_label());
  InstructionNode * end_of_while = create_LABEL_instruction(create_label());
  add_instruction(init_of_while);
  //added while label
  InstructionNode * condition_expr = create_statement_instructions(root -> left_child);
  //Condition_expr_added
  InstructionNode * condition_jmp = create_instruction_conditional_jump(get_jump_instruction_from_operation(condition_expr -> operation), NULL, condition_expr -> result);
  condition_jmp -> result = end_of_while -> result;
  add_instruction(condition_jmp);
  //condition_jmp_added
  add_while_instructions(root);
  //WHILE_instructions_added
  add_instruction(create_instruction_conditional_jump(JMP, init_of_while -> result, NULL));
  //added_while_escape
  add_instruction(end_of_while);
  //added_END_OF_WHILE_label
}

InstructionNode * returned_data() {
  InstructionNode * data_retrieved_from_stack_label = create_LABEL_instruction(create_label());
  //created label to jmp if no data was returned
  InstructionNode * popped_data = create_TEMP_instruction(create_temporal());
  //created temp to save the popped
  InstructionNode * data_was_returned = create_POP_instruction(popped_data -> result);
  //created pop instruction so we can know if some data was returned (0 is false and 1 is true)
  InstructionNode * jmp_if_no_data_returned = create_instruction_conditional_jump(JZ, data_retrieved_from_stack_label -> result, popped_data -> result);
  //created jump to be used if no data was returned from function
  InstructionNode * retrieve_data = create_POP_instruction(popped_data -> result);
  ////created pop instruction to retrieve the returned data
  add_instruction(popped_data);
  add_instruction(data_was_returned);
  add_instruction(jmp_if_no_data_returned);
  add_instruction(retrieve_data);
  add_instruction(data_retrieved_from_stack_label);

  return popped_data;
}

InstructionNode * create_instruction_method_call(ASTNode * root) {
  InstructionNode * return_label = create_LABEL_instruction(create_label());
  //Created return label
  add_instruction(create_PUSH_instruction(return_label -> result));
  //Pushed Return Label
  int parameters_quantity = 0;
  ASTNode * aux = root -> right_child;
  while (aux != NULL) {
    InstructionNode * param_temp_instruct = create_TEMP_instruction(create_temporal_with_value(aux -> var_data -> value, aux -> var_data -> is_boolean));
    //Created temporal with current param
    add_instruction(param_temp_instruct);
    //Added temporal instruction
    add_instruction(create_PUSH_instruction(param_temp_instruct -> result));
    //Added push instruction of current param
    parameters_quantity++;
    //Incremented the quantity of parameters
    aux = aux -> right_child;
  }
  InstructionNode * params_quantity_temp_instruct = create_TEMP_instruction(create_temporal_with_value(parameters_quantity, false));
  add_instruction(params_quantity_temp_instruct);
  //Added temporal with quantity of parameters
  add_instruction(create_PUSH_instruction(params_quantity_temp_instruct -> result));
  //Added push instruction of quantity of current params
  InstructionNode * fun_called_label = find_label(root -> function_data -> id);
  if (fun_called_label == NULL) {
    //This means the function is not created yet
    fun_called_label = create_LABEL_instruction(create_label_with_id(root -> function_data -> id));
    add_label_to_queue(fun_called_label);
    //The label was created and saved in an alternative queue.- So it can be added later
  }
  add_instruction(create_instruction_conditional_jump(JMP, fun_called_label -> result, NULL));
  add_instruction(return_label);
  //Added label to return after function
  InstructionNode * data = returned_data();
  return data;
}

void create_instruction_return(ASTNode * root) {
  InstructionNode * data_to_return = create_statement_instructions(root -> right_child);
  add_instruction(create_PUSH_instruction(data_to_return -> result));
}

InstructionNode * create_instruction_2op_operation(ASTNode * root) {
  printf("encuentra un operador \n");
  InstructionNode * instruction = create_instruction_from_ASTNode(root);
  instruction -> result = create_temporal();
  if ((instruction -> operation == MINUS) && (root -> left_child == NULL)) {
    InstructionNode * zero_temp = create_TEMP_instruction(create_temporal_with_value(0, false));
    add_instruction(zero_temp);
    instruction -> op1 = zero_temp -> result;
  }
  else {
    instruction -> op1 = create_statement_instructions(root -> left_child) -> result;
  }
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
      case _if: 
        create_instruction_if(root);
        break;
      case _if_body: break;
      case _while: 
        create_instruction_while(root);
        break;
      case _arith_op: case _boolean_op:
        return create_instruction_2op_operation(root);
      case _assign:
        return create_instruction_assignment(root);
      case _method_call: 
        return create_instruction_method_call(root);
        break;
      case _return: 
        create_instruction_return(root);
        break;
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

void push_data_and_return(ASTNode * last_node, bool is_main) {
  InstructionNode * return_label = create_TEMP_instruction(create_temporal());
  InstructionNode * load_return_label = create_POP_instruction(return_label -> result);
  InstructionNode * return_instruction = create_instruction_conditional_jump(JMP, return_label -> result, NULL);
  InstructionNode * need_to_pop;
  if ((last_node -> node_type != _return) || (last_node -> right_child == NULL))
    need_to_pop = create_TEMP_instruction(create_temporal_with_value(0, true));
  else
    need_to_pop = create_TEMP_instruction(create_temporal_with_value(1, true));
  if (!is_main) {
    add_instruction(return_label);
    add_instruction(load_return_label);
  }
  add_instruction(need_to_pop);
  add_instruction(create_PUSH_instruction(need_to_pop -> result));
  if (!is_main)
    add_instruction(return_instruction);
}

/*
  Creates the intermediate code.
*/
void generate_intermediate_code(ASTNode * root, char * fun_name) {
  ASTNode * aux = root;
  ASTNode * pre_aux;
  while (aux != NULL) {
    create_statement_instructions(aux);
    pre_aux = aux;
    aux = get_next_statement(aux);
    if (aux == NULL)
      push_data_and_return(pre_aux, fun_name == "main");
  }
}

/*
  Given an isntruction node returns the string representation of the operation.
*/
char * get_operation_string(InstructionNode * i) {
  switch (i -> operation) {
    case PLUS:          return "PLUS";
    case PROD:          return "PROD";
    case MOD:           return "MOD";
    case DIV:           return "DIV";
    case MINUS:         return "MINS";
    case EQUALS:        return "EQUL";
    case OR:            return "OR";
    case AND:           return "AND";
    case CALL:          return "CALL";
    case END_FUN:       return "ENDF";
    case PUSH:          return "PUSH";
    case POP:           return "POP";
    case TMP1:          return "TMP1";
    case TMP2:          return "TMP2";
    case GREATER_THAN:  return "GRTR";
    case LESSER_THAN:   return "LSSR";
    case ASSIGN:        return "ASGN";
    case LABEL:         return "LABL";
    case JMP:           return "JMP";
    case JZ:            return "JZ";
    case JNZ:           return "JNZ";
    case JE:            return "JE";
    case JNE:           return "JNE";
    case JG:            return "JG";
    case JL:            return "JL";
    case JGE:           return "JGE";
    case JLE:           return "JLE";
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
    case LABEL:
      printf("%s   %s\n", get_operation_string(i), i -> result -> id);
      break;
    case TMP1: case CALL: case END_FUN: case PUSH: case POP: case JMP: case JZ: case JNZ: case JE: case JNE: case JG: case JL: case JGE: case JLE:      // aca se imprimen instrucciones con un solo operador
      printf("\t%s   %s\n", get_operation_string(i), i -> result -> id);
      break;
    case TMP2: case ASSIGN:                  // aca se imprime instrucciones con 2 operadores
      printf("\t%s   %s  %s\n", get_operation_string(i), i -> result -> id, get_temporal_string(i -> op1));
      break;
    case PLUS: case MINUS: case PROD: case DIV: case MOD: case EQUALS: case OR: case AND: case GREATER_THAN: case LESSER_THAN: // aca con 3 operadores
      printf("\t%s   %s  %s  %s\n", get_operation_string(i), i -> result -> id, i -> op1 -> id, i -> op2 -> id);
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
  while (aux != NULL) {
    if (aux -> operation == CALL)
      printf("\n\n\n =================  CODIGO INTERMEDIO DE: %s  ================= \n\n\n", aux -> result -> id);
    print_instruction(aux);
    aux = aux -> next;
  }
  printf("\n\n\n");
}

/*
  Generates the necessary instructions to manage function parameters
*/
void generate_fun_parameters_treatment(FunctionNode * f) {
  if (f -> id != "main") {
    //Esta es la cantidad de parametros que fueron popeados (Originalmente 0)
    InstructionNode * current_param = create_TEMP_instruction(create_temporal_with_value(0, false));
    //Esta es la cantidad de parametros que fueron pusheados antes del jmp a la funcion
    InstructionNode * parameters_quantity = create_TEMP_instruction(create_temporal());
    //En esta instruccion se guardara el resultado de comparar los dos datos anteriores
    InstructionNode * compare_params_quantity = create_TEMP_instruction(create_temporal());
    //Esta instruccion es una simple constante 1, para incrementar current_param en el futuro.-
    InstructionNode * constant_one = create_TEMP_instruction(create_temporal_with_value(1, false));
    //Esta instruccion setea compare_params_quantity
    InstructionNode * param_comparer = create_instructionNode(EQUALS, compare_params_quantity -> result, parameters_quantity -> result, current_param -> result);
    //Esta instruccion incrementa en 1 current_param
    InstructionNode * param_counter = create_instructionNode(PLUS, current_param -> result, current_param -> result, constant_one -> result);
    //Estos son labels del ciclo que se encargara de hacer pop a todos los parametros actuales
    InstructionNode * init_of_while = create_LABEL_instruction(create_label());
    InstructionNode * end_of_while = create_LABEL_instruction(create_label());
    InstructionNode * temp_param;
    add_instruction(current_param);
    add_instruction(parameters_quantity);
    add_instruction(create_POP_instruction(parameters_quantity -> result));
    add_instruction(compare_params_quantity);
    add_instruction(constant_one);
    add_instruction(init_of_while);
    //added while label
    add_instruction(param_comparer);
    add_instruction(create_instruction_conditional_jump(JE, end_of_while -> result, param_comparer -> result));
    add_instruction(param_counter);
    temp_param = create_TEMP_instruction(create_temporal());
    add_instruction(temp_param);
    add_instruction(create_POP_instruction(temp_param -> result));
    add_instruction(create_instruction_conditional_jump(JMP, init_of_while -> result, NULL));
    add_instruction(end_of_while);
    //added_END_OF_WHILE_label
  }
}

/*
  Generates the label of a function. So it can be accessed by a jump.
*/
void generate_fun_label(FunctionNode * f) {
  InstructionNode * fun_label = find_label(f -> id);
  if (fun_label != NULL) {
    //Si la variable fue encontrada, significa que ya existe el label creado.- Pero no fue agregado al programa aun
    add_instruction_from_queue(fun_label);
  }
  else {
    fun_label = create_LABEL_instruction(create_label_with_id(f -> id));
    add_instruction(fun_label);
  }
}

/*
  Generates the intermediate code for all functions of the program.
*/
void generate_fun_code(FunctionNode * head) {
  FunctionNode * aux = head;
  while (aux != NULL) {
    generate_fun_label(aux);
    //Created a label to access function
    generate_fun_parameters_treatment(aux);
    //All params pop generated
    generate_intermediate_code(aux -> body, aux -> id);
    add_instruction(create_instructionNode(END_FUN, create_temporal_with_id(aux -> id), NULL, NULL));
    aux = aux -> next;
  }
}