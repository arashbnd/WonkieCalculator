
#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

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

struct expression *expression_allocate(void);
int expression_push_node(struct expression *expr, struct expression_node *nod, struct expression **res);
void expression_free(struct expression *expr);
struct expression_node *expression_node_allocate(void);
struct expression_node *expression_node_allocate_literal(number_t x);
struct expression_node *expression_node_allocate_operator(enum operator oper, int paren_level);
void expression_node_free(struct expression_node *nod);

#endif /* _EXPRESSION_H_ */
