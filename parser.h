
#ifndef _PARSER_H_
#define _PARSER_H_

int parse_number(const char *str, number_t *res, char **endp);
int parse_operator(char opchar, enum operator *res);
int parse_expression(const char *str, struct expression **out);

#endif /* _PARSER_H_ */
