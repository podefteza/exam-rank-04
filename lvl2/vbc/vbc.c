#include "vbc.h"

// no changes
node	*new_node(node n)
{
	node	*ret;

	ret = calloc(1, sizeof(n));
	if (!ret)
		return (NULL);
	*ret = n;
	return (ret);
}

// no changes
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
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		// printf("Unexpected end of file\n");
		printf("Unexpected end of input\n");
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

int	check_parentheses(char *str)
{
	int	balance;
	int	i;

	balance = 0;
	i = -1;
	while (str[++i])
	{
		if (str[i] == '(')
			balance++;
		else if (str[i] == ')')
		{
			balance--;
			if (balance < 0)
				return (-1);
		}
	}
	return (balance);
}

node	*parse_mult(char **str)
{
	node	*l;
	node	*r;
	node	temp;

	l = parse_nb_or_group(str);
	if (!l)
		return (NULL);
	while (**str == '*')
	{
		(*str)++;
		r = parse_nb_or_group(str);
		if (!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		temp.type = MULTI;
		temp.l = l;
		temp.r = r;
		l = new_node(temp);
		if (!l)
		{
			destroy_tree(r);
			return (NULL);
		}
	}
	return (l);
}

node	*parse_add(char **str)
{
	node	*l;
	node	*r;
	node	temp;

	l = parse_mult(str);
	if (!l)
		return (NULL);
	while (**str == '+')
	{
		(*str)++;
		r = parse_mult(str);
		if (!r)
		{
			destroy_tree(l);
			return (NULL);
		}
		temp.type = ADD;
		temp.l = l;
		temp.r = r;
		l = new_node(temp);
		if (!l)
		{
			destroy_tree(r);
			return (NULL);
		}
	}
	return (l);
}

node	*parse_nb_or_group(char **str)
{
	node	*res;
	node	temp;

	res = NULL;
	if (**str == '(')
	{
		(*str)++;
		res = parse_add(str);
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
		temp.type = VAL;
		temp.val = (**str) - '0';
		res = new_node(temp);
		if (!res)
			return (NULL);
		(*str)++;
		return (res);
	}
	unexpected(**str);
	return (NULL);
}

int	check_consecutive_digits(char *input)
{
	int	i;

	i = -1;
	while (input[++i])
	{
		if (isdigit(input[i]) && isdigit(input[i + 1]))
		{
			unexpected(input[i + 1]);
			return (1);
		}
	}
	return (0);
}

int	main(int argc, char **argv)
{
	node	*tree;
	char	*input;
	int		paren_check;

	if (argc != 2)
		return (1);
	input = argv[1];
	if (check_consecutive_digits(input))
		return (1);
	paren_check = check_parentheses(input);
	if (paren_check < 0)
	{
		unexpected(')');
		return (1);
	}
	else if (paren_check > 0)
	{
		unexpected('(');
		return (1);
	}
	tree = parse_add(&input);
	if (!tree)
		return (1);
	if (*input)
	{
		unexpected(*input);
		destroy_tree(tree);
		return (1);
	}
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
