#ifndef VBC_NEW_H
# define VBC_NEW_H

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

node *parse_addition(char **str);
node	*new_node(node n);
void	destroy_tree(node *n);
void	unexpected(char c);
int		eval_tree(node *tree);

#endif
