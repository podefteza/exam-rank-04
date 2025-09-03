#ifndef VBC_H
# define VBC_H

# include <ctype.h>
# include <malloc.h>
# include <stdio.h>

extern int g_error;
extern char *g_str; // Global string pointer for parsing
extern int g_i; // Global index for current position

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

node	*parse_addition(void);
node	*new_node(node n);
void	destroy_tree(node *n);
void	unexpected(char c);
int		eval_tree(node *tree);

#endif
