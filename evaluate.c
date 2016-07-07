
#include <stdio.h> // for size_t

#include "expression.h"

number_t eval(struct expression_node *node){
   if(node->type == EXPR_LITERAL)
      return node->expr.literal;
   else {
      switch (node->expr.operator.operation){
         case OPER_MULT:
         return eval(node->expr.operator.lhs)*eval(node->expr.operator.rhs);

         case OPER_DIV:
         return eval(node->expr.operator.lhs)/eval(node->expr.operator.rhs);

         case OPER_ADD:
         return eval(node->expr.operator.lhs)+eval(node->expr.operator.rhs);

         case OPER_SUBTR:
         return eval(node->expr.operator.lhs)-eval(node->expr.operator.rhs);

         case OPER_NEGATE:
         return -eval(node->expr.operator.rhs);
      }
   }
}
