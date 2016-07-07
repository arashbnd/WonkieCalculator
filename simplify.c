
#include <stdlib.h>

#include "expression.h"

static void refresh_expression_node_completeness(struct expression_node *nod) {
	nod->complete = (
		nod->type == EXPR_LITERAL ||
		(
			nod->type == EXPR_OPERATOR &&
			(
				nod->expr.operator.lhs ||
				nod->expr.operator.operation == OPER_NEGATE
			) &&
			nod->expr.operator.rhs
		)
	);
}

int simplify_expression(struct expression *expr, struct expression **out) {
	size_t i;
	int ret = -1;
	struct expression_node *nod;
	struct expression *res;

	if (! (res = expression_allocate()))
		goto simplify_expression_end;

	for (i = 0; i < expr->n_nodes; ++i)
		refresh_expression_node_completeness(expr->nodes[i]);

	for (i = 0; i < expr->n_nodes; ++i) {
		nod = expr->nodes[i];

		if (nod->complete && expr->n_nodes > 1) {
			if (i == expr->n_nodes - 1)
				expr->nodes[i - 1]->expr.operator.rhs = nod;
			else if (i == 0)
				expr->nodes[i + 1]->expr.operator.lhs = nod;
			else if (expr->nodes[i - 1]->expr.operator.precedence >= expr->nodes[i + 1]->expr.operator.precedence)
				expr->nodes[i - 1]->expr.operator.rhs = nod;
			else
				expr->nodes[i + 1]->expr.operator.lhs = nod;
		} else if (expression_push_node(res, nod, &res))
			goto simplify_expression_end;
	}

	ret = 0;

simplify_expression_end:
	if (ret) {
		if (res)
			free(res);
	} else {
		*out = res;
		free(expr);
	}

	return ret;
}
