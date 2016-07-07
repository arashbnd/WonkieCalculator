#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "expression.h"

struct expression *expression_allocate(void) {
	struct expression *ret;

	if (! (ret = malloc(sizeof(*ret))))
		return NULL;

	memset(ret, '\0', sizeof(*ret));

	return ret;
}

int expression_push_node(struct expression *expr, struct expression_node *nod, struct expression **res) {
	if (! (expr = realloc(expr, sizeof(*expr) + (expr->n_nodes + 1) * sizeof(*expr->nodes))))
		return -1;

	expr->nodes[expr->n_nodes++] = nod;
	*res = expr;

	return 0;
}

void expression_free(struct expression *expr) {
	unsigned i;

	if (expr) {
		for (i = 0; i < expr->n_nodes; ++i)
			expression_node_free(expr->nodes[i]);

		free(expr);
	}
}

struct expression_node *expression_node_allocate(void) {
	struct expression_node *ret;

	if (! (ret = malloc(sizeof(*ret))))
		return NULL;

	memset(ret, '\0', sizeof(*ret));

	return ret;
}

struct expression_node *expression_node_allocate_literal(number_t x) {
	struct expression_node *ret;

	if (! (ret = expression_node_allocate()))
		return NULL;

	ret->type = EXPR_LITERAL;
	ret->expr.literal = x;

	return ret;
}

struct expression_node *expression_node_allocate_operator(enum operator oper, int paren_level) {
	struct expression_node *ret;

	if (! (ret = expression_node_allocate()))
		return NULL;

	ret->type = EXPR_OPERATOR;
	ret->expr.operator.operation = oper;
	ret->expr.operator.precedence = (unsigned) oper + paren_level * (unsigned) OPER_MAX;

	return ret;
}

void expression_node_free(struct expression_node *nod) {
	if (nod) {
		if (nod->type == EXPR_OPERATOR) {
			expression_node_free(nod->expr.operator.lhs);
			expression_node_free(nod->expr.operator.rhs);
		}

		free(nod);
	}
}
