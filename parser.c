
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "expression.h"

// ugly hack to let us use isspace(3) and isdigit(3) [TODO]
extern int isspace(int c);
extern int isdigit(int c);

int parse_number(const char *str, number_t *res) {
	number_t s, abs_s;
	char *endp;

	s = strtod(str, &endp);

	abs_s = fabs(s);
	if (
		endp == str ||
		abs_s == INFINITY ||
		abs_s == NAN ||
		abs_s == HUGE_VAL
	)
		return -1;

	*res = s;

	return endp - str;
}

int parse_operator(char opchar, enum operator *res) {
	switch (opchar) {
		case '+':
			*res = OPER_ADD;
		break;
		case '-':
			*res = OPER_SUBTR;
		break;
		case '*':
			*res = OPER_MULT;
		break;
		case '/':
			*res = OPER_DIV;
		break;
		default:
			return -1;
		break;
	}

	return 0;
}

static void print_error_pointer(unsigned column) {
	unsigned i;

	for (i = 0; i < column; ++i)
		fputc(' ', stderr);

	fprintf(stderr, "^\n");
}

int parse_expression(const char *str, struct expression **out) {
	unsigned i;
	int ret = -1, paren_level = 0, s;
	struct expression *res = NULL;
	struct expression_node *node = NULL;
	enum expression_type last_token = EXPR_NONE;
	enum operator oper;
	number_t number;

	if (! (res = expression_allocate()))
		goto parse_expression_end;

	for (i = 0; str[i]; ++i) {
		if (isspace(str[i]))
			continue;

		if (isdigit(str[i]) && (s = parse_number(str + i, &number)) > 0) {
			i += s - 1;

			if (last_token == EXPR_LITERAL) {
				print_error_pointer(i);
				fprintf(stderr, "number found where operator expected\n");
				goto parse_expression_end;
			}

			if (! (node = expression_node_allocate_literal(number)))
				goto parse_expression_end;

			last_token = EXPR_LITERAL;
		} else if (parse_operator(str[i], &oper) == 0) {
			if (last_token != EXPR_LITERAL) {
				if (oper == OPER_SUBTR) {
					oper = OPER_NEGATE;
				} else {
					print_error_pointer(i);
					fprintf(stderr, "operator found where number expected\n");
					goto parse_expression_end;
				}
			}

			if (! (node = expression_node_allocate_operator(oper, paren_level)))
				goto parse_expression_end;

			last_token = EXPR_OPERATOR;
		} else if (str[i] == '(') {
			++paren_level;
			last_token = EXPR_NONE;
		} else if (str[i] == ')') {
			if (--paren_level < 0) {
				print_error_pointer(i);
				fprintf(stderr, "too many closing parentheses\n");
				goto parse_expression_end;
			}
			last_token = EXPR_LITERAL;
		} else {
			print_error_pointer(i);
			fprintf(stderr, "unexpected token\n");
			goto parse_expression_end;
		}

		if (node && expression_push_node(res, node, &res))
			goto parse_expression_end;

		node = NULL;
	}

	if (last_token == EXPR_OPERATOR || paren_level > 0) {
		print_error_pointer(i);
		fprintf(stderr, "unexpected end of expression\n");
		goto parse_expression_end;
	}

	ret = 0;

parse_expression_end:
	if (ret) {
		expression_node_free(node);
		expression_free(res);
	} else
		*out = res;

	return ret;
}
