#include "expression.h"

number_t eval(struct expression_node *node){
   if(node->type == EXPR_LITERAL)
      return node->expr.literal;
}
else {
   
   switch (node.operator.operation){
      case OPER_MULT:
      return eval(node.operator.lhs)*eval(node.operator.rhs);
 
      case OPER_DIV:
      return eval(node.operator.lhs)/eval(node.operator.rhs);

      case OPER_ADD:
      return eval(node.operator.lhs)+eval(node.operator.rhs);

      case OPER_SUBTR:
      return eval(node.operator.lhs)-eval(node.operator.rhs);

      case OPER_NEGATE:
      return -eval(node.operator.rhs);
   }


}