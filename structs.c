#include "structs.h"

int temp_quantity = 0;
int label_quantity = 0;

/*
  Creates and returns a new VarNode.
*/
VarNode * create_VarNode(char * id, int value, bool is_boolean) {
  VarNode * new_node = malloc(sizeof(VarNode));
  new_node -> id = id;
  new_node -> value = value;
  new_node -> is_boolean = is_boolean;
  new_node -> next = NULL;
  return new_node;
}

/*
  Creates and returns a new VarNode created from a parameter.
*/
VarNode * create_varnode_from_param(Parameter * param) {
  VarNode * new_varnode = create_VarNode(param -> id, param -> value, param -> is_boolean);
  new_varnode -> is_defined = false;
  new_varnode -> offset = param -> offset;
  new_varnode -> kind = _parameter;
  return new_varnode;
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
  Creates and returns a new VarNode.
*/
VarNode * create_temporal_with_id(char * id) {
  VarNode * new_node = create_var_node();
  new_node -> id = id;
  new_node -> kind = _temporal;
  return new_node;
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
  Returns the string representation of a temporal.
*/
char * get_temporal_string(VarNode * temp) {
  if (temp == NULL)
    return "";
  if (temp -> is_defined) {
    if (temp -> is_boolean) {
      if (temp -> value == 0)
        return "false";
      else
        return "true";
    }
    else {
      int i = temp -> value;
      char str[12];
      sprintf(str, "%d", i);
      char * ret = str;
      return ret;
    }
  }
  return temp -> id;
}

/*
  Returns the string representation of a var.
*/
// char * get_var_string(VarNode * var) {
//   switch (var -> kind) {
//     case 0:
//       return _boolean;
//     case 1:
//       return _integer;
//     default:
//       return _void;
//   }
// }

/*
  Given an isntruction node returns the string representation of the operation.
*/
char * get_operation_string(InstructionNode * i) {
  switch (i -> operation) {
    case PLUS:          return "PLUS ";
    case PROD:          return "PROD ";
    case MOD:           return "MOD  ";
    case DIV:           return "DIV  ";
    case MINUS:         return "MINUS";
    case EQUALS:        return "EQUAL";
    case OR:            return "OR   ";
    case AND:           return "AND  ";
    case CALL:          return "CALL ";
    case END_FUN:       return "ENDF ";
    case PUSH:          return "PUSH ";
    case POP:           return "POP  ";
    case GREATER_THAN:  return "GRETR";
    case LESSER_THAN:   return "LESSR";
    case ASSIGN:        return "ASSGN";
    case LABEL:         return "LABEL";
    case JMP:           return "JUMP ";
    case BEGIN_FUN:     return "BEGIN";
    case JE:            return "JE   ";
    case JNE:           return "JNE  ";
    case CMP:           return "CMP  ";
    case RETURN:        return "RETRN";
    case NEGAT:         return "NEG  ";
    case EXTERN:        return "EXTRN";
    default:            return "------ UNKNOWN OPERATION ------";
  }
}

/*
  Creates a new label.
*/
VarNode * create_label() {
  char label_name[128];
  sprintf(label_name, "label%d\0", label_quantity);
  char * res = malloc(strlen(label_name));
  sprintf(res, label_name);
  VarNode * new_node = create_var_node();
  new_node -> id = res;
  new_node -> kind = _label;
  label_quantity++;
  return new_node;
}

/*
  Creates a new label and sets its id.
*/
VarNode * create_label_with_id(char * id) {
  VarNode * new_node = create_var_node();
  new_node -> id = id;
  new_node -> kind = _label;
  return new_node;
}

/*
  Returns the corresponding node type depending of the operator.
*/
TypeNode get_node_type(int op) {
  if (op == 'i')
    return _if;
  else if (op == '=')
    return _assign;
  else if (op == 'r')
    return _return;
  else if (op == 'm')
    return _method_call;
  else if (op == 'l')
    return _literal;
  else if (op == 'b')
    return _if_body;
  else if (op == 'w')
    return _while;
  else if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
    return _arith_op;
  else if (op == '<' || op == '>' || op == 'e' || op == '&' || op == '|' || op == '!')
    return _boolean_op;
  else
    return _id;
}

/*
  Takes an int as parameter and returns the represented ReturnType.
*/
ReturnType get_return_type(int type_int_value) {
  switch (type_int_value) {
    case 0:
      return _boolean;
    case 1:
      return _integer;
    default:
      return _void;
  }
}

/*
  Returns the string representation of a ReturnType enum.
*/
char * get_return_type_string(ReturnType value) {
  switch (value)
  {
    case _boolean: return "bool";
    case _integer: return "integer";
    case _void: return "void";
  }
}

/*
  Returns the string representation of a ReturnType enum.
*/
char * get_varnode_kind_string(VarNode * var) {
  switch (var -> kind)
  {
    case _global: return "global";
    case _local: return "local";
    case _parameter: return "parameter";
    case _temporal: return "temporal";
    case _label: return "label";
  }
}

/*
  Takes an operator and returns true if the operation results in a boolean value, false cc.
*/
bool is_boolean_operation(int op) {
  if (op == '+' || op == '-' || op == '*' || op == '/' || op == '%')
    return false;
  else if (op == '<' || op == '>' || op == 'e' || op == '&' || op == '|' || op == '!')
    return true;
  return false;
}

/*
  Returns the string representation of the enum TypeNode
*/
char * get_type_node_string(TypeNode tn) {
  switch (tn)
  {
    case _if: return "if";
    case _if_body: return "if body";
    case _while: return "while";
    case _arith_op: return "arith op";
    case _boolean_op: return "boolean op";
    case _assign: return "assign";
    case _method_call: return "method call";
    case _return: return "return";
    case _literal: return "literal";
    case _id: return "id";
  }
}

/*
  Returns the string representation of an ASTNode.
*/
char * get_string_representation(ASTNode * node) {
  char * aux;
  switch (node -> node_type) {
    case _if: return "if";
    case _if_body: return "if body";
    case _while: return "while";
    case _arith_op: return (char *) &(node -> data);
    case _boolean_op: return (char *) &(node -> data);
    case _assign: return "=";
    case _method_call:
      if (node -> function_data != NULL) {
        int fun_id_len = strlen(node -> function_data -> id);
        if (fun_id_len <= 10) {
          char str[12];
          sprintf(str, "%s%s", node -> function_data -> id, "()");
          char * ret = str;
          return ret;
        }
        else {
          char str[32];
          sprintf(str, "%s%s", node -> function_data -> id, "()");
          char * ret = str;
          return ret;
        }
      }
      else {
        return "method_call";
      }
      break;
    case _return: return "return";
    case _id:
      return node -> var_data -> id;
    case _literal:
      if (node -> is_boolean) {
        if (node -> data == 0)
          return "false";
        else
          return "true";
      }
      else {
        if (node -> var_data == NULL) {
          int i = node -> data;
          char str[8];
          sprintf(str, "%d", i);
          char * ret = str;
          return ret;
        }
        else {
          return node -> var_data -> id;
        }
      }
      break;
  }
}