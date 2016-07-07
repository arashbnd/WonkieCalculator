#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "expression.h"

void print_expression_node(struct expression_node *nod) {
	char opchar;

	switch (nod->type) {
		case EXPR_LITERAL:
			printf(NUMBER_T_FMT, nod->expr.literal);
		break;
		case EXPR_OPERATOR:
			switch (nod->expr.operator.operation) {
				case OPER_NULL:
					opchar = 'N';
				break;
				case OPER_SUBTR:
				case OPER_NEGATE:
					opchar = '-';
				break;
				case OPER_ADD:
					opchar = '+';
				break;
				case OPER_DIV:
					opchar = '/';
				break;
				case OPER_MULT:
					opchar = '*';
				break;
				case OPER_MAX:
					opchar = 'M';
				break;
			}

			printf("%c{%d}", opchar, nod->expr.operator.precedence);
		break;
	}
}

void print_expression(struct expression *expr) {
	size_t i;

	for (i = 0; i < expr->n_nodes; ++i) {
		if (i > 0)
			putchar(' ');
		print_expression_node(expr->nodes[i]);
	}

	printf("\n");
}
