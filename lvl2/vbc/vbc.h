#ifndef VBC_H
# define VBC_H

# include <ctype.h>
# include <malloc.h>
# include <stdio.h>

// no changes
typedef struct node
{
	enum
	{
		ADD,
		MULTI,
		VAL
	} type;
	int			val;
	struct node	*l;
	struct node	*r;
}				node;

node			*new_node(node n);
void			destroy_tree(node *n);
void			unexpected(char c);
int				check_parentheses(char *str);
node			*parse_nb_or_group(char **str);
node			*parse_mult(char **str);
node			*parse_add(char **str);
int				eval_tree(node *tree);

#endif
