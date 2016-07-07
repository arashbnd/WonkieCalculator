
#ifndef _PRINTER_H_
#define _PRINTER_H_

void print_expression_node(struct expression_node *nod, FILE *stream);
void print_expression(struct expression *expr, FILE *stream);
void print_expression_tree(struct expression *expr, FILE *stream);
void print_expression_node_tree(struct expression_node *nod, unsigned indent_level, FILE *stream);

#endif /* _PRINTER_H_ */
