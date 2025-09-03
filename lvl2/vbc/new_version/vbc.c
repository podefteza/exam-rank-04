#include "vbc.h"

int g_error = 0; // Define the global variable
char *g_str = NULL; // Global string pointer for parsing
int g_i = 0; // Global index for current position

node	*parse_number_or_group(void)
{
	node	*res;
	node	box;

	res = NULL;
	if (g_str[g_i] == '(')
	{
		g_i++;
		res = parse_addition();
		if (!res || g_str[g_i] != ')')
		{
			destroy_tree(res);
			unexpected(g_str[g_i]);
			return (NULL);
		}
		g_i++;
		return (res);
	}
	if (isdigit(g_str[g_i]))
	{
		box.type = VAL;
		box.val = g_str[g_i] - '0';
		res = new_node(box);
		g_i++;
		return (res);
	}
	unexpected(g_str[g_i]);
	return (NULL);
}

node	*parse_multiplication(void)
{
	node	*left, *right;
	node	box;

	left = parse_number_or_group();
	if (!left)
		return (NULL);
	while (g_str[g_i] == '*')
	{
		g_i++;
		right = parse_number_or_group();
		if (!right)
			return (destroy_tree(left), NULL);
		box.l = left;
		box.r = right;
		box.type = MULTI;
		left = new_node(box);
	}
	return (left);
}

node	*parse_addition(void)
{
	node	*left, *right;
	node	box;

	left = parse_multiplication();
	if (!left)
		return (NULL);
	while (g_str[g_i] == '+')
	{
		g_i++;
		right = parse_multiplication();
		if (!right)
			return (destroy_tree(left), NULL);
		box.l = left;
		box.r = right;
		box.type = ADD;
		left = new_node(box);
	}
	return (left);
}

int is_balanced(char *str)
{
	int balanced = 0;
	int i = 0;

	while(str[i])
	{
		if (str[i] == '(')
			balanced++;
		else if (str[i] == ')')
		{
			if (!balanced)
				return (unexpected(')'), 0);
			balanced--;
		}
		i++;
	}
	if (balanced > 0)
		return (unexpected(')'), 0);
	return (1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (1);
	if (!is_balanced(argv[1]))
		return (1);
	g_str = argv[1]; // Initialize global string pointer
	g_i = 0; // Initialize global index
	node *tree = parse_addition();
	if (!tree || g_error)
		return (1);
	if (g_str[g_i]) // Check if entire string was consumed
	{
		unexpected(g_str[g_i]);
		destroy_tree(tree);
		return (1);
	}
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
