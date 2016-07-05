
#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

enum operator {
	OPER_ADD,
	OPER_SUBTR,
	OPER_MULT,
	OPER_DIV,
};

enum expression_type {
	EXPR_EVAL,
	EXPR_LITERAL,
};

typedef double number_t;

struct expression {
	enum expression_type exprtype;
	union {
		struct {
			struct expression *lhs, *rhs;
			enum operator operation;
		} eval;
		number_t literal;
	} expr;
};

#endif /* _EXPRESSION_H_ */
