int allocate_expression(struct expression **out) {
	struct expression *ret;

	if (! (ret = malloc(sizeof(*ret))))
		return -1;

	memset(ret, '\0', sizeof(*ret));
	ret->type = EXPR_LITERAL;

	*out = ret;

	return 0;
}

void free_expression(struct expression *expr) {
	if (expr) {
		switch (expr->type) {
			case EXPR_BINARY:
				free_expression(expr->expr->lhs);
				free_expression(expr->expr->rhs);
			break;
			case EXPR_UNARY:
				free_expression(expr->expr->unary);
			break;
		}

		if (expr->str)
			free(expr->str);

		free(expr);
	}
}

