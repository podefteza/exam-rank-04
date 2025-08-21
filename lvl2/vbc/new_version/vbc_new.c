#include "vbc_new.h"

node	*parse_number_or_group(char **str)
{
	node	*res;
	node	box;

	res = NULL;
	if (**str == '(')
	{
		(*str)++;
		res = parse_addition(str);
		if (!res || **str != ')')
		{
			destroy_tree(res);
			unexpected(**str);
			return (NULL);
		}
		(*str)++;
		return (res);
	}
	if (isdigit(**str))
	{
		box.type = VAL;
		box.val = **str - '0';
		res = new_node(box);
		(*str)++;
		return (res);
	}
	unexpected(**str);
	return (NULL);
}

node	*parse_multiplication(char **str)
{
	node	*left;
	node	*right;
	node	box;

	left = parse_number_or_group(str);
	if (!left)
		return (NULL);
	while (**str == '*')
	{
		(*str)++;
		right = parse_number_or_group(str);
		if (!right)
			return (destroy_tree(left), NULL);
		box.l = left;
		box.r = right;
		box.type = MULTI;
		left = new_node(box);
	}
	return (left);
}

node	*parse_addition(char **str)
{
	node	*left;
	node	*right;
	node	box;

	left = parse_multiplication(str);
	if (!left)
		return (NULL);
	while (**str == '+')
	{
		(*str)++;
		right = parse_multiplication(str);
		if (!right)
			return (destroy_tree(left), NULL);
		box.l = left;
		box.r = right;
		box.type = ADD;
		left = new_node(box);
	}
	return (left);
}

int	is_balanced(char *str)
{
	int	balanced = 0;
	int i = 0;

	while (str[i])
	{
		if (str[i] == '(')
			balanced++;
		else if (str[i] == ')')
		{
			if (!balanced)
				return (0);
			balanced--;
		}
		i++;
	}
	return (balanced == 0);
}

int	main(int argc, char **argv)
{
	node	*tree;

	if (argc != 2)
		return (1);
	if (!is_balanced(argv[1]))
		return (printf("Unexpected token ')'\n"), 1);
	tree = parse_addition(&argv[1]);
	if (!tree)
		return (1);
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
}
