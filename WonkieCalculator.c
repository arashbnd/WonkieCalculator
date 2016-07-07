#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "expression.h"
#include "printer.h"

int main(void) {
	char *line = NULL;
	size_t line_len = 0;
	struct expression *expr;

	while (getline(&line, &line_len, stdin) >= 0) {
		if (parse_expression(line, &expr))
			continue;

		print_expression(expr);

		expression_free(expr);
	}

	return feof(stdin);
}
