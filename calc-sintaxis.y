%{
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"

VarNode * temporal_enviroment;                          // Holds the last closed enviroment
EnviromentNode *symbol_table = (EnviromentNode *) NULL; // Stack that contains all the open enviroment levels
FunctionNode *fun_list_head = (FunctionNode *) NULL;    // List of all the functions of the program

/*
  Adds a new variable to the current enviroment of the symbol table.
*/
void add_var_to_symbol_table(char * var_name, int value, bool is_boolean) {
  printf("add_var_to_symbol_table\n");
  VarNode * new_var = (VarNode *) malloc(sizeof(VarNode));
  if (new_var == NULL)
    printf( "no available memory!\n");
  new_var -> id = var_name;
  new_var -> value = value;
  new_var -> is_boolean = is_boolean;
  new_var -> next = NULL;
  if (symbol_table -> variables == NULL) {
    symbol_table -> variables = new_var;
  }
  else {
    new_var -> next = symbol_table -> variables;
    symbol_table -> variables = new_var;
  }
}

/*
  Returns a new partial varNode with the ID taken as parameter
*/
VarNode * partial_varnode(char * var_name) {
  printf("partial_varnode\n");
  VarNode * new_var = (VarNode *) malloc(sizeof(VarNode));
  if (new_var == NULL)
    printf( "no available memory!\n");
  new_var -> id = var_name;
  new_var -> is_defined = false;
  new_var -> next = NULL;
  return new_var;
}

//Checked for Segmentation Fault by Santi.
void add_new_parameter(Parameter * params_list_head, Parameter * to_add_param) {
  if (params_list_head == NULL)
    params_list_head = to_add_param;
  else {
    Parameter * parameterAuxNode = params_list_head;
    //Moving to last node position
    while (parameterAuxNode -> next != NULL) {
      parameterAuxNode = parameterAuxNode -> next;
    }
    //Appending to_add_param
    parameterAuxNode -> next = to_add_param;
  }
}

/*
  Appends a varNode to a list of VarNodes.
*/
void add_partial_varnode(VarNode * var_list_head, VarNode * to_add_node) {
  printf("add_partial_varnode\n");
  if (var_list_head == NULL)
    var_list_head = to_add_node;
  else {
    VarNode * varAuxNode = var_list_head;
    //Moving to last node position
    while (varAuxNode -> next != NULL) {
      varAuxNode = varAuxNode -> next;
    }
    //Appending to_add_node
    varAuxNode -> next = to_add_node;
  }
}

/*
  Closes the current enviroment, leaving the next enviroment on the top of the stack.
*/
void close_enviroment() {
  symbol_table = symbol_table -> next;
}

/*
  Sets a value to a varNode
*/
void set_value_to_varnode(VarNode * var_node, int value) {
  printf("add_value_to_var_node\n");
  if (var_node == NULL)
    printf("Cant add value to a non-existent variable.\n");
  else {
    var_node -> value = value;
    var_node -> is_defined = true;
  }
}

/*
  Opens a new enviroment level. It is placed on the top of the stack.
*/
void open_enviroment() {
  printf("open_enviroment\n");
  EnviromentNode * new_level = (EnviromentNode *) malloc(sizeof(EnviromentNode));
  new_level -> variables = NULL;
  new_level -> next = NULL;
  if (symbol_table == NULL)
    symbol_table = new_level;
  else {
    new_level -> next = symbol_table;
    symbol_table = new_level;
  }
}

//Checked for Segmentation Fault by Santi.
FunctionNode * add_function_to_funlist(int return_type, char * function_name, Parameter *parameters_list) {
                                               printf("add_function_to_funlist\n");

  FunctionNode * functionAuxNode = fun_list_head;

  //Create new function and load its data.
  FunctionNode * new_function = (FunctionNode *) malloc(sizeof(FunctionNode));

  new_function -> id = function_name;

  ReturnType ret_type;
  if (return_type == 0)
    ret_type = boolean;
  else if (return_type == 1)
    ret_type = integer;
  else
    ret_type = vid;


  new_function -> type = ret_type;
  new_function -> parameters = parameters_list;
  new_function -> enviroment = temporal_enviroment;

  if (fun_list_head == NULL)
    fun_list_head = new_function;
  else {
    //Moving to last function position
    while (functionAuxNode -> next != NULL) {
      functionAuxNode = functionAuxNode -> next;
    }

    //Loads new function in function list
    functionAuxNode -> next = new_function;
  }

  return new_function;

}

/*
  Searches for a variable on a list of variables
*/
VarNode * find_variable(VarNode * head, char * varname) {
  printf("find_variable\n");
  VarNode * varAuxNode = head;
  VarNode * result;
  while (varAuxNode != NULL) {
    //Checking if current var is the sought.-
    if (varAuxNode -> id == varname) {
      result = varAuxNode;
      return result;
    }
    else {
      varAuxNode = varAuxNode -> next;
    }
  }
  return NULL;
}

/*
  Searches for a variable on all the enviroments.
*/
VarNode * find_variable_in_enviroments(char * var_name) {
  printf("find_variable_in_enviroments\n");
  if (symbol_table == NULL) {
    printf("Symbol Table is null!\n\n\n");
    return NULL;
  }
  EnviromentNode * aux = symbol_table;
  VarNode * result = find_variable(aux -> variables, var_name);
  while (result == NULL && aux -> next != NULL) {
    aux = aux -> next;
    result = find_variable(aux -> variables, var_name);
  }
  return result;
}

/*
  Returns a new leave created from a varNode
*/
ASTNode * create_AST_leave_from_VarNode(VarNode * var_data) {
  printf("create_AST_leave_from_VarNode\n");
  if (var_data == NULL) {
    printf("Cannot create leave from null var.\n");
    return NULL;
  }
  else {
    ASTNode * new_leave = (ASTNode *) malloc(sizeof(ASTNode));
    new_leave -> data = var_data -> value;
    new_leave -> is_boolean = var_data -> is_boolean;
    new_leave -> is_if = false;
    new_leave -> is_if_body = false;
    new_leave -> is_while = false;
    new_leave -> is_arith_op = false;
    new_leave -> is_boolean_op = false;
    new_leave -> var_data = var_data;
    new_leave -> function_data = NULL;
    new_leave -> left_child = NULL;
    new_leave -> right_child = NULL;
    return new_leave;
  }
}

/*
  Returns a new leave created from a value.
*/
ASTNode * create_AST_leave_from_value(int value, bool is_boolean) {
  printf("create_AST_leave_from_value\n");
  ASTNode * new_leave = (ASTNode *) malloc(sizeof(ASTNode));
  new_leave -> data = value;
  new_leave -> is_boolean = is_boolean;
  new_leave -> is_if = false;
  new_leave -> is_if_body = false;
  new_leave -> is_while = false;
  new_leave -> is_arith_op = false;
  new_leave -> is_boolean_op = false;
  new_leave -> var_data = NULL;
  new_leave -> function_data = NULL;
  new_leave -> left_child = NULL;
  new_leave -> right_child = NULL;
  return new_leave;
}

/*
  Returns a new ASTNode
*/
ASTNode * create_AST_node(ASTNode * left_child, char op, ASTNode * right_child) {
  printf("create_AST_node\n");
  ASTNode * new_node = (ASTNode *) malloc(sizeof(ASTNode));
  new_node -> data = op;
  new_node -> is_boolean = false;
  if (op == 'i')
    new_node -> is_if = true;
  else
    new_node -> is_if = false;
  if (op == 'b')
    new_node -> is_if_body = true;
  if (op == 'w')
    new_node -> is_while = true;
  else
    new_node -> is_while = false;
  if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
    new_node -> is_arith_op = true;
  else
    new_node -> is_arith_op = false;
  if (op == '<' || op == '>' || op == 'e' || op == '&' || op == '|' || op == '!')
    new_node -> is_boolean_op = true;
  else
    new_node -> is_boolean_op = false;
  new_node -> var_data = NULL;
  new_node -> function_data = NULL;
  new_node -> left_child = left_child;
  new_node -> right_child = right_child;
  return new_node;
}

bool eval_bool_expr(ASTNode * root);
int eval_int_expr(ASTNode * root);

int eval_int_expr(ASTNode * root) {
  printf("eval_int_expr\n");
  if (root -> left_child == NULL && root -> right_child == NULL)
    return root->data;
  if (root -> is_arith_op) {
    if ((char) root->data == '+')
      return eval_int_expr(root->left_child) + eval_int_expr(root->right_child);
    else if ((char) root->data == '-')
      return eval_int_expr(root->left_child) - eval_int_expr(root->right_child);
    else if ((char) root->data == '*')
      return eval_int_expr(root->left_child) * eval_int_expr(root->right_child);
    else if ((char) root->data == '/')
      return (int) eval_int_expr(root->left_child) / eval_int_expr(root->right_child);
    else if ((char) root->data == '%')
      return (int) eval_int_expr(root->left_child) % eval_int_expr(root->right_child);
  }
}

bool eval_bool_expr(ASTNode * root) {
  printf("eval_bool_expr\n");
  if (root -> left_child == NULL && root -> right_child == NULL)
    return (bool) root -> data;
  if (root -> is_boolean_op) {
    if ((char) root->data == '<')
      return eval_int_expr(root->left_child) < eval_int_expr(root->right_child);
    else if ((bool) root->data == '>')
      return eval_int_expr(root->left_child) > eval_int_expr(root->right_child);
    else if ((bool) root->data == 'e') {
      if (root -> left_child -> is_boolean_op || root -> right_child -> is_boolean_op)
        return eval_bool_expr(root->left_child) == eval_bool_expr(root->right_child);
      else
        return eval_int_expr(root->left_child) == eval_int_expr(root->right_child);
    }
    else if ((char) root->data == '&')
      return (bool) eval_bool_expr(root->left_child) && eval_bool_expr(root->right_child);
    else if ((char) root->data == '|')
      return (bool) eval_bool_expr(root->left_child) || eval_bool_expr(root->right_child);
    else if ((char) root->data == '!')
      return (bool) !eval_bool_expr(root->left_child);
  }
}

//Checked for Segmentation Fault by Santi.
bool check_if_equals(Parameter * list1, Parameter * list2) {
  printf("check_if_equals\n");
  Parameter * paramAuxNode1 = list1;
  Parameter * paramAuxNode2 = list2;
  while (paramAuxNode1 != NULL) {
    if (paramAuxNode2 == NULL)
      return false;
    if (paramAuxNode1 -> is_boolean && !(paramAuxNode2 -> is_boolean))
      return false;
    if (paramAuxNode2 -> is_boolean && !(paramAuxNode1 -> is_boolean))
      return false;
    if (paramAuxNode1 -> value != paramAuxNode2 -> value)
      return false;
    paramAuxNode1 = paramAuxNode1 -> next;
    paramAuxNode2 = paramAuxNode2 -> next;
  }
  if (paramAuxNode2 != NULL)
    return false;
  return true;
}

//Checked for Segmentation Fault by Santi.
bool is_callable(char * function_name, Parameter * params) {
  printf("is_callable\n");
  FunctionNode * functionAuxNode = fun_list_head;
  while (functionAuxNode != NULL) {
    if (functionAuxNode -> id == function_name) {
      return check_if_equals(functionAuxNode -> parameters, params);
    }
  }
  return false;
}

//Checked for Segmentation Fault by Santi.
ASTNode * ast_from_parameters_list (Parameter * params_list) {
  printf("ast_from_parameters_list\n");
  ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));
  Parameter * paramAuxNode = params_list;

  if (paramAuxNode != NULL) {
    if (paramAuxNode -> id != NULL) {

      VarNode *var_data = find_variable_in_enviroments(paramAuxNode -> id);

      result -> data = var_data -> value;
      result -> is_boolean = var_data -> is_boolean;
      result -> is_if = false;
      result -> is_if_body = false;
      result -> is_while = false;
      result -> is_arith_op = false;
      result -> is_boolean_op = false;
      result -> var_data = var_data;
      result -> function_data = NULL;
      result -> left_child = NULL;
      result -> right_child = ast_from_parameters_list(params_list -> next);
    }
    else {
      VarNode * var_data = (VarNode *) malloc(sizeof(VarNode));

      var_data -> value = paramAuxNode -> value;
      var_data -> is_boolean = paramAuxNode -> is_boolean;
      var_data -> id = "temporal_var";
      var_data -> is_defined = true;


      result -> data = paramAuxNode -> value;
      result -> is_boolean = paramAuxNode -> is_boolean;
      result -> is_if = false;
      result -> is_if_body = false;
      result -> is_while = false;
      result -> is_arith_op = false;
      result -> is_boolean_op = false;
      result -> var_data = NULL;
      result -> function_data = NULL;
      result -> left_child = NULL;
      result -> right_child = ast_from_parameters_list(params_list -> next);
    }
    return result;
  }
  else {
    return NULL;
  }
}

//Checked for Segmentation Fault by Santi.
FunctionNode * find_function(char * function_name) {
                                               printf("find_function\n");

  FunctionNode * functionAuxNode = fun_list_head;

  while (functionAuxNode != NULL) {
    if (functionAuxNode -> id == function_name)
      return functionAuxNode;
    functionAuxNode = functionAuxNode -> next;
  }
  return NULL;
}

//Checked for Segmentation Fault by Santi.
void set_type(int type, VarNode * var_list_head) {
                                               printf("set_type\n");

  VarNode * varAuxNode = var_list_head;

  while (varAuxNode != NULL) {
    if (type == 0) {
      varAuxNode -> is_boolean = true;
    }
    else {
      varAuxNode -> is_boolean = false;
    }

    varAuxNode = varAuxNode -> next;
  }
}

/*
  Adds a variable list to the current enviroment.
*/
void add_varlist_to_enviroment(VarNode * var_list) {
  if (symbol_table != NULL) {
    symbol_table -> variables = var_list;
  }
  else {
    printf("You cant add variables to a null enviroment\n");
    yyerror();
  }
}

%}

%union { int i; char *s; ASTNode *node; VarNode *varnode; FunctionNode *functionnode; Parameter *parameternode;};

%token<i> _PROGRAM_
%token<i> _BEGIN_
%token<i> _END_
%token<i> _VOID_
%token<i> _IF_
%token<i> _ELSE_
%token<i> _INT_
%token<i> _INTEGER_
%token<i> _BOOL_
%token<i> _RETURN_
%token<i> _MAIN_
%token<i> _PRINT_
%token<i> _THEN_
%token<i> _SEMICOLON_
%token<i> _COMMA_
%token<i> _L_PARENTHESIS_
%token<i> _R_PARENTHESIS_
%token<i> _PLUS_
%token<i> _MINUS_
%token<i> _MULTIPLY_
%token<i> _DIVIDE_
%token<i> _MOD_
%token<i> _GREATER_THAN_
%token<i> _LESSER_THAN_
%token<i> _EQUALS_
%token<i> _AND_
%token<i> _OR_
%token<i> _NOT_
%token<i> _ASSIGNMENT_
%token<i> _WHILE_
%token<i> _TRUE_
%token<i> _FALSE_
%token<s> _ID_
%token<i> _EXTERN_

%start prog

%nonassoc _ASSIGNMENT_
%left _AND_ _OR_
%nonassoc _EQUALS_ _GREATER_THAN_ _LESSER_THAN_
%left _PLUS_ _MINUS_
%left _MULTIPLY_ _DIVIDE_ _MOD_
%right NEG


%type<varnode> vars_block
%type<varnode> id_list
%type<parameternode> params_def
%type<parameternode> params_call
%type<node> prog;
%type<node> prog_body;
%type<node> methods_block;
%type<node> method_decl;
%type<node> main_decl;
%type<node> method_call //method_call es tipo ASTNode porque forma parte del arbol.
%type<node> code_block //code_block es tipo ASTNode porque forma parte del arbol.
%type<node> code_block_body //code_block_body es tipo ASTNode porque forma parte del arbol.
%type<node> statements_block //statements_block es tipo ASTNode porque forma parte del arbol.
%type<node> statement //statement es tipo ASTNode porque forma parte del arbol.
%type<node> conditional_statement //conditional_statement es tipo ASTNode porque forma parte del arbol.
%type<node> expr //expr es tipo ASTNode porque forma parte del arbol.
%type<node> literal //literal es tipo ASTNode porque forma parte del arbol.
%type<node> integer_literal //integer_literal es tipo ASTNode porque forma parte del arbol.
%type<node> bool_literal //bool_literal es tipo ASTNode porque forma parte del arbol.
%type<i> type //type es tipo integer para el chequeo de tipos.

%%

prog: _PROGRAM_ scope_open prog_body scope_close 
  {
    printf("\nEncontre: prog");
    $$ = $3;
  }
;

scope_open: _BEGIN_ 
  {
    open_enviroment();
  }
;

scope_close: _END_ 
  {
    //Save Enviroment in temporal var
    temporal_enviroment = symbol_table -> variables;
    close_enviroment();
  }
;

prog_body: vars_block methods_block main_decl
    {
      printf("\nEncontre: vars_block -> methods_block -> main_decl");
      //Adding vars to enviroment
      add_varlist_to_enviroment($1);
      $2 -> right_child = $3;
      $$ = $2;
    }
  | methods_block main_decl 
    {
      printf("\nEncontre: methods_block -> main_decl");
      $1 -> right_child = $2;
      $$ = $1;
    }
  | main_decl 
    {
      printf("\nEncontre: main_decl");
      $$ = $1;
    }
;

vars_block: type id_list _SEMICOLON_
    {
      printf("\nEncontre: type id_list ;");
      set_type($1, $2);
      $$ = $2;
    }
  | vars_block type id_list _SEMICOLON_ 
    {
      printf("\nEncontre: type id_list;");
      set_type($2, $3);
      $$ = $3;
    }
;

id_list: _ID_ 
    {
      printf("\nEncontre: id");
      add_partial_varnode($$, partial_varnode($1));
    }
  | id_list _COMMA_ _ID_ 
    {
      printf("\nEncontre: Declaracion de Variable");
      add_partial_varnode($$, partial_varnode($3));
    }
;

methods_block: method_decl 
    {
      printf("\nEncontre: method_decl");
      $$ = $1;
    }
  | methods_block method_decl 
    {
      printf("\nEncontre: methods_block -> method_decl");
      $$ = $2;
    }
;

method_decl: type _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block  {
              printf("\nEncontre: declaracion de un metodo");

              FunctionNode * new_function = add_function_to_funlist($1, $2, $4);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = $6;

              
              
              
              $$ = result;
            }
           | type _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist($1, $2, NULL);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = $5;

              
              
              
              $$ = result;
            }
           | _VOID_ _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist(-1, $2, $4);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = $6;

              
              
              
              $$ = result;
            }
           | _VOID_ _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist(-1, $2, NULL);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = $5;

              
              
              
              $$ = result;
            }
           | type _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ _EXTERN_ {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist($1, $2, $4);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = NULL;

              
              

              $$ = result;
            }
           | type _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block _EXTERN_ {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist($1, $2, NULL);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = NULL;

              
              

              $$ = result;
            }
           | _VOID_ _ID_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ _EXTERN_ {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist(-1, $2, $4);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = NULL;

              
              

              $$ = result;
            }
           | _VOID_ _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ _EXTERN_ {
              printf("\nEncontre: declaracion de un metodo");
              FunctionNode * new_function = add_function_to_funlist(-1, $2, NULL);

              ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

              result -> is_boolean = false;
              result -> is_if = false;
              result -> is_if_body = false;
              result -> is_while = false;
              result -> is_arith_op = false;
              result -> is_boolean_op = false;
              result -> var_data = NULL;
              result -> function_data = new_function;
              result -> left_child = NULL;
              result -> right_child = NULL;

              
              

              $$ = result;
            }
  ;

main_decl: type _MAIN_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block {
            printf("\nEncontre: declaracion de main");

            FunctionNode * new_function = add_function_to_funlist($1, "main", $4);

            ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

            result -> is_boolean = false;
            result -> is_if = false;
            result -> is_if_body = false;
            result -> is_while = false;
            result -> is_arith_op = false;
            result -> is_boolean_op = false;
            result -> var_data = NULL;
            result -> function_data = new_function;
            result -> left_child = NULL;
            result -> right_child = $6;

            
            
            
            $$ = result;
          }
         | type _MAIN_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block {
            printf("\nEncontre: declaracion de main");

            FunctionNode * new_function = add_function_to_funlist($1, "main", NULL);

            ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

            result -> is_boolean = false;
            result -> is_if = false;
            result -> is_if_body = false;
            result -> is_while = false;
            result -> is_arith_op = false;
            result -> is_boolean_op = false;
            result -> var_data = NULL;
            result -> function_data = new_function;
            result -> left_child = NULL;
            result -> right_child = $5;

            
            
            
            $$ = result;
          }
         | _VOID_ _MAIN_ _L_PARENTHESIS_ params_def _R_PARENTHESIS_ code_block {
            printf("\nEncontre: declaracion de main");

            FunctionNode * new_function = add_function_to_funlist(-1, "main", $4);

            ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

            result -> is_boolean = false;
            result -> is_if = false;
            result -> is_if_body = false;
            result -> is_while = false;
            result -> is_arith_op = false;
            result -> is_boolean_op = false;
            result -> var_data = NULL;
            result -> function_data = new_function;
            result -> left_child = NULL;
            result -> right_child = $6;

            
            
            
            $$ = result;
          }
         | _VOID_ _MAIN_ _L_PARENTHESIS_ _R_PARENTHESIS_ code_block {
            printf("\nEncontre: declaracion de main");

            FunctionNode * new_function = add_function_to_funlist(-1, "main", NULL);

            ASTNode * result = (ASTNode *) malloc(sizeof(ASTNode));

            result -> is_boolean = false;
            result -> is_if = false;
            result -> is_if_body = false;
            result -> is_while = false;
            result -> is_arith_op = false;
            result -> is_boolean_op = false;
            result -> var_data = NULL;
            result -> function_data = new_function;
            result -> left_child = NULL;
            result -> right_child = $5;

            
            
            
            $$ = result;
          }
 ;

code_block: scope_open code_block_body scope_close
              {
                printf("\nEncontre: code_block");
                $$ = $2;
              }
  ;

code_block_body: vars_block statements_block {
                    printf("\nEncontre: vars_block -> statements_block");
                    //Adding vars to enviroment
                    add_varlist_to_enviroment($1);

                    $$ = $2;
                  }
               | statements_block {
                    printf("\nEncontre: statements_block");
                    $$ = $1;
                  }
               |  {
                    printf("\nEncontre: NULL code_block_body");
                    $$ = NULL;
                  }
  ;

statements_block: statement{
                    printf("\nEncontre: statements_block");
                    $$ = $1;
                  }
                | statements_block statement {
                    printf("\nEncontre: statements_block -> statement");
                    $$ = $2;
                  }
  ;

statement:  _ID_ _ASSIGNMENT_ expr _SEMICOLON_
              {
                printf("\nEncontre: asignacion en statement");

                VarNode *id_varnode = find_variable_in_enviroments($1);

                if (id_varnode == NULL) {
                  printf("Intenta definir una variable inexistente!\n");
                  yyerror();
                  return -1;
                }
                
                if (id_varnode -> is_boolean) {
                  set_value_to_varnode(id_varnode, (int) eval_bool_expr($3));
                }
                else {
                  int val = eval_int_expr($3);
                  printf("Value is %d.\n", val);

                  set_value_to_varnode(id_varnode, val);
                }

                ASTNode * node_from_id = create_AST_leave_from_VarNode(id_varnode);

                $$ = create_AST_node(node_from_id, 'e', $3);
              }
          | method_call _SEMICOLON_
              {
                printf("\nEncontre: llamado_a_metodo en statement");
                $$ = $1;
              }
          | conditional_statement
              {
                printf("\nEncontre: conditional en statement");
                
                $$ = $1;
              }
          | _WHILE_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ code_block
              {
                printf("\nEncontre: while block en statement");
                ASTNode * while_root = create_AST_node($3, 'w', $5);
                
                $$ = while_root;
              }
          | _RETURN_ expr _SEMICOLON_
              {
                printf("\nEncontre: return_expr_; en statement");
                $$ = $2;
              }
          | _RETURN_ _SEMICOLON_
              {
                printf("\nEncontre: return_; en statement");
                $$ = NULL;
              }
          | _SEMICOLON_
              {
                printf("\nEncontre: ; en statement");
                //Lo dejo a criterio de mis compañeros porque es obvio (?) jajajaj
                $$ = NULL;
              }
          | code_block
              {
                printf("\nEncontre: codeblock en statement");
                
                $$ = $1;
              }
  ;

conditional_statement: _IF_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ _THEN_ code_block {
                        printf("\nEncontre: if-then block\n");
                        ASTNode * if_root = create_AST_node($3, 'i', $6);

                        $$ = if_root;
                      }
                     | _IF_ _L_PARENTHESIS_ expr _R_PARENTHESIS_ _THEN_ code_block _ELSE_ code_block {
                        printf("\nEncontre: if-then-else block\n");
                        ASTNode * if_body = create_AST_node($6, 'b', $8);
                        ASTNode * if_root = create_AST_node($3, 'i', if_body);

                        $$ = if_root;
                      }
  ;

method_call: _ID_ _L_PARENTHESIS_ params_call _R_PARENTHESIS_ {
              printf("\nEncontre: llamado a metodo\n");
              if (!is_callable($1, $3)) {
                yyerror("Function not defined");
                return -1;
              }

              ASTNode * parameters_as_childs = ast_from_parameters_list($3);
              ASTNode * head = (ASTNode *) malloc(sizeof(ASTNode));

              head -> is_boolean = false;
              head -> is_if = false;
              head -> is_if_body = false;
              head -> is_while = false;
              head -> is_arith_op = false;
              head -> is_boolean_op = false;
              head -> var_data = NULL;
              head -> function_data = find_function($1);
              head -> left_child = NULL;
              head -> right_child = parameters_as_childs;


              $$ = head;
            }

           | _ID_ _L_PARENTHESIS_ _R_PARENTHESIS_ {
              printf("\nEncontre: llamado a metodo\n");
              if (!is_callable($1, NULL)) {
                yyerror("Function not defined");
                return -1;
              }

              ASTNode * head = (ASTNode *) malloc(sizeof(ASTNode));

              head -> is_boolean = false;
              head -> is_if = false;
              head -> is_if_body = false;
              head -> is_while = false;
              head -> is_arith_op = false;
              head -> is_boolean_op = false;
              head -> var_data = NULL;
              head -> function_data = find_function($1);
              head -> left_child;
              head -> right_child = NULL;

              $$ = head;
            }
  ;

params_call: expr {
              printf("\nEncontre: expr in params_call");
              Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));

              if ($1 -> var_data != NULL) {
                new_param -> is_boolean = $1 -> var_data -> is_boolean;
                new_param -> value = $1 -> var_data -> value;
                new_param -> id = $1 -> var_data -> id;
              }
              else {
                new_param -> is_boolean = $1 -> is_boolean;
                new_param -> value = $1 -> data;
                new_param -> id = NULL;
              }

              add_new_parameter($$, new_param);
            }
           | params_call _COMMA_ expr {
              printf("\nEncontre: parametros de llamada");
              Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));

              if ($3 -> var_data != NULL) {
                new_param -> is_boolean = $3 -> var_data -> is_boolean;
                new_param -> value = $3 -> var_data -> value;
                new_param -> id = $3 -> var_data -> id;
              }
              else {
                new_param -> is_boolean = $3 -> is_boolean;
                new_param -> value = $3 -> data;
                new_param -> id = NULL;
              }

              add_new_parameter($$, new_param);
            }
  ;

params_def: type _ID_
    {
      printf("\nEncontre: Parametros de definicion");
      Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));

      new_param -> id = $2;
      new_param -> is_boolean = ($1 == 0);
      new_param -> next = NULL;

      $$ = new_param;
    }
  | params_def _COMMA_ type _ID_ 
    {
      Parameter * new_param = (Parameter *) malloc(sizeof(Parameter));

      new_param -> id = $4;
      new_param -> is_boolean = ($3 == 0);
      new_param -> next = NULL;

      $$ = new_param;
    }
;

type: _INTEGER_
    {
      printf("\nEncontre: type_INTEGER");
      $$ = 1;
    }
  | _BOOL_
    {
      printf("\nEncontre: type_BOOL");
      $$ = 0;
    }
;

expr: _ID_
    {
      printf("\nEncontre: id_expr");
      char * var_name = $1;
      VarNode * var_data = find_variable_in_enviroments(var_name);
      if (var_data != NULL && var_data -> is_defined) {
        $$ = create_AST_leave_from_VarNode(var_data);
      }
      else {
        $$ = NULL;
        yyerror("Variable no declarada o definida");
        return -1;
      }
    }
  | literal
    {
      printf("\nEncontre: literal");
      $$ = $1;
    }
  | method_call
    {
      printf("\nEncontre: llamado a metodo en expr");
      $$ = $1;
    }
  | expr _PLUS_ expr
    {
      printf("\nEncontre: expr + expr");
      $$ = create_AST_node($1, '+', $3);
    }
  | expr _MINUS_ expr
    {
      printf("\nEncontre: expr - expr");
      $$ = create_AST_node($1, '-', $3);
    }
  | expr _MULTIPLY_ expr
    {
      printf("\nEncontre: expr x expr");
      $$ = create_AST_node($1, '*', $3);
    }
  | expr _DIVIDE_ expr
    {
      printf("\nEncontre: expr / expr");
      $$ = create_AST_node($1, '/', $3);
    }
  | expr _MOD_ expr
    {
      printf("\nEncontre: expr MOD expr");
      $$ = create_AST_node($1, '%', $3);
    }
  | expr _LESSER_THAN_ expr
    {
      printf("\nEncontre: expr < expr");
      $$ = create_AST_node($1, '<', $3);
    }
  | expr _GREATER_THAN_ expr
    {
      printf("\nEncontre: expr > expr");
      $$ = create_AST_node($1, '>', $3);
    }
  | expr _EQUALS_ expr
    {
      printf("\nEncontre: expr == expr");
      $$ = create_AST_node($1, 'e', $3);
    }
  | expr _AND_ expr
    {
      printf("\nEncontre: expr && expr");
      $$ = create_AST_node($1, '&', $3);
    }
  | expr _OR_ expr
    {
      printf("\nEncontre: expr || expr");
      $$ = create_AST_node($1, '|', $3);
    }
  | _MINUS_ expr %prec NEG
    {
      printf("\nEncontre: -expr");
      $$ = create_AST_node(NULL, '-', $2);
    }
  | _NOT_ expr %prec NEG
    {
      printf("\nEncontre: !expr");
      $$ = create_AST_node(NULL, '!', $2);
    }
  | _L_PARENTHESIS_ expr _R_PARENTHESIS_
    {
      printf("\nEncontre: (expr)");
      $$ = $2;
    }
;

literal: integer_literal
    {
      printf("\nEncontre: literal_integer");
      $$ = $1;
    }
  | bool_literal
    {
      printf("\nEncontre: literal_bool");
      $$ = $1;
    }
;

bool_literal: _TRUE_
    {
      printf("\nEncontre: un literal_bool TRUE");
      $$ = create_AST_leave_from_value(1, true);
    }
  | _FALSE_
    {
      printf("\nEncontre: un literal_bool FALSE");
      $$ = create_AST_leave_from_value(0, true);
    }
;

integer_literal: _INT_
    {
      printf("\nEncontre: un literal_integer");
      $$ = create_AST_leave_from_value($1, false);
    }
;

%%
