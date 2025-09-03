#include "vbc.h"

extern int g_error;

node	*new_node(node n)
{
	node	*ret;

	ret = calloc(1, sizeof(n));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

void	destroy_tree(node *n)
{
	if (!n)
		return ;
	if (n->type != VAL)
	{
		destroy_tree(n->l);
		destroy_tree(n->r);
	}
	free(n);
}

void	unexpected(char c)
{
	if (c && !g_error)
		printf("Unexpected token '%c'\n", c);
	else if (!g_error)
		printf("Unexpected end of input\n");
		//printf("Unexpected end of file\n"); // change "file" -> "input"
	g_error = 1;
}

int	eval_tree(node *tree)
{
	switch (tree->type)
	{
	case ADD:
		return (eval_tree(tree->l) + eval_tree(tree->r));
	case MULTI:
		return (eval_tree(tree->l) * eval_tree(tree->r));
	case VAL:
		return (tree->val);
	}
	return (0); // added this
}
