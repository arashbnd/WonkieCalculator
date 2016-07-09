
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

enum operator {
	OPER_NULL = 0,
	OPER_SUBTR,
	OPER_ADD,
	OPER_DIV,
	OPER_MULT,
	OPER_NEGATE,
	OPER_MAX,
};

enum expression_type {
	EXPR_NONE,
	EXPR_OPERATOR,
	EXPR_LITERAL,
};

typedef double number_t;
#define NUMBER_T_FMT "%f"

struct expression_node {
	enum expression_type type;
	unsigned char complete;
	union {
		struct {
			struct expression_node *lhs, *rhs;
			enum operator operation;
			int precedence;
		} operator;
		number_t literal;
	} expr;
};

struct expression {
	size_t n_nodes;
	struct expression_node *nodes[0];
};

number_t eval(struct expression_node *node);
struct expression *expression_allocate(void);
int expression_push_node(struct expression *expr, struct expression_node *nod, struct expression **res);
void expression_free(struct expression *expr);
struct expression_node *expression_node_allocate(void);
struct expression_node *expression_node_allocate_literal(number_t x);
struct expression_node *expression_node_allocate_operator(enum operator oper, int paren_level);
void expression_node_free(struct expression_node *nod);
int parse_number(const char *str, number_t *res);
int parse_operator(char opchar, enum operator *res);
int parse_expression(const char *str, struct expression **out);
void print_expression_node(struct expression_node *nod, FILE *stream);
void print_expression(struct expression *expr, FILE *stream);
void print_expression_tree(struct expression *expr, FILE *stream);
void print_expression_node_tree(struct expression_node *nod, unsigned indent_level, FILE *stream);
int simplify_expression(struct expression *expr, struct expression **out);

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

         case OPER_NULL:
         case OPER_MAX:
         default:
         return NAN;
      }
   }
}

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
		case EXPR_NONE:
			fprintf(stream, "<none>");
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

int main(void) {
	char *line = NULL;
	size_t line_len = 0;
	struct expression *expr;
	int error;

	while (getline(&line, &line_len, stdin) >= 0) {
		if (parse_expression(line, &expr))
			continue;

		if (expr->n_nodes > 0) {
			for (error = 0; expr->n_nodes > 1;) {
				fprintf(stderr, "-------\n");
				print_expression_tree(expr, stderr);

				if (simplify_expression(expr, &expr)) {
					fprintf(stderr, "error simplifying expression'");
					error = 1;
					break;
				}
			}
			fprintf(stderr, "-------\n");
			if (error)
				continue;

			print_expression_tree(expr, stderr);

			fprintf(stderr, "-------\n");
			
			printf(NUMBER_T_FMT "\n", eval(expr->nodes[0]));
		}

		expression_free(expr);
	}

	return feof(stdin);
}
