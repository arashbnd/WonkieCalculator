
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>

int find_highest_level_parenthisis_body(const char *str, char **out) {
	unsigned i;
	unsigned paren_level = 0;
	unsigned paren_count = 0;
	char *res;
	unsigned char first_paren = 0;

	if (! (res = malloc(strlen(str) + 1)))
		return -1;

	for (i = 0; str[i]; ++i) {
		switch (str[i]) {
			case '(':
				if (paren_level++ == 0)
					first_paren = 1;
			break;
			case ')':
				if (paren_level > 0) {
					if (--paren_level == 0) {
						res[paren_count] = '\0';
						break;
					}
				} else {
					free(res);
					errno = EINVAL;
					return -1;
				}
			break;
		}

		if (paren_level > 0 && !first_paren)
			res[paren_count++] = str[i];

		first_paren = 0;
	}

	if (paren_level > 0 || paren_count == 0) {
		free(res);
		errno = EINVAL;
		return -1;
	}

	*out = res;

	return paren_count;
}

int main(void) {
	char *line = NULL;
	size_t line_len = 0;
	char *parenthisis_body;

	while (getline(&line, &line_len, stdin) >= 0) {
		line[strlen(line) - 1] = '\0';

		if (find_highest_level_parenthisis_body(line, &parenthisis_body) < 0) {
			warn("could not find a matching parenthisis group in '%s'", line);
			continue;
		}

		printf("%s\n", parenthisis_body);

		free(parenthisis_body);
	}

	return 0;
}
