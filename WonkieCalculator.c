
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "expression.h"
#include "printer.h"
#include "simplify.h"
#include "evaluate.h"
#include "parser.h"

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
