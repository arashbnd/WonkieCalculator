#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "expression.h"
#include "printer.h"

void print_expression_node(struct expression_node *nod, FILE *stream) {
	char opchar;

	switch (nod->type) {
		case EXPR_LITERAL:
			fprintf(stream, NUMBER_T_FMT, nod->expr.literal);
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

			fprintf(stream, "%c{%d}", opchar, nod->expr.operator.precedence);
		break;
	}
}

void print_expression(struct expression *expr, FILE *stream) {
	size_t i;

	for (i = 0; i < expr->n_nodes; ++i) {
		if (i > 0)
			fputc(' ', stream);
		print_expression_node(expr->nodes[i], stream);
	}

	fprintf(stream, "\n");
}

void print_expression_tree(struct expression *expr, FILE *stream) {
	size_t i;

	for (i = 0; i < expr->n_nodes; ++i)
		print_expression_node_tree(expr->nodes[i], 0, stream);
}

void print_expression_node_tree(struct expression_node *nod, unsigned indent_level, FILE *stream) {
	unsigned i;

	for (i = 0; i < indent_level; ++i)
		fputc('\t', stream);

	print_expression_node(nod, stream);
	fputc('\n', stream);

	if (nod->type == EXPR_OPERATOR) {
		if (nod->expr.operator.lhs)
			print_expression_node_tree(nod->expr.operator.lhs, indent_level + 1, stream);
		if (nod->expr.operator.rhs)
			print_expression_node_tree(nod->expr.operator.rhs, indent_level + 1, stream);
	}
}
