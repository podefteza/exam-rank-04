#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

int g_error = 0;

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    }   type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *new_node(node n)
{
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n;
    return (ret);
}

void    destroy_tree(node *n)
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

void    unexpected(char c)
{
    if (c && !g_error)
        printf("Unexpected token '%c'\n", c);
    else if (!g_error)
        printf("Unexpected end of input\n");
	g_error = 1;
}

int eval_tree(node *tree)
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
	return (0);
}

node *parse_nb_or_group(char **str);
node *parse_multi(char **str);
node *parse_add(char **str);

node *parse_nb_or_group(char **str)
{
	node *res;
	node box;

	res = NULL;
	if (**str == '(')
	{
		(*str)++;
		res = parse_add(str);
		if (!res || **str != ')')
		{
			destroy_tree(res);
			unexpected(**str);
			return(NULL);
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
		return(res);
	}
	unexpected(**str);
	return (NULL);

}

node *parse_multi(char **str)
{
	node *left, *right;
	node box;

	left = parse_nb_or_group(str);
	if (!left)
		return (NULL);
	while(**str == '*')
	{
		(*str)++;
		right = parse_nb_or_group(str);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		box.l = left;
		box.r = right;
		box.type = MULTI;
		left = new_node(box);
	}
	return (left);
}

node *parse_add(char **str)
{
	node *left, *right;
	node box;

	left = parse_multi(str);
	if (!left)
		return (NULL);
	while(**str == '+')
	{
		(*str)++;
		right = parse_multi(str);
		if (!right)
		{
			destroy_tree(left);
			return (NULL);
		}
		box.l = left;
		box.r = right;
		box.type = ADD;
		left = new_node(box);
	}
	return (left);
}

int is_balanced(char *argv)
{
	int balanced = 0;
	int i = 0;

	while (argv[i])
	{
		if (argv[i] == '(')
			balanced++;
		else if (argv[i] == ')')
		{
			balanced --;
			if (balanced < 0)
			{
				unexpected(argv[i]);
				return (0);
			}
		}
		i++;
	}
	if (balanced > 0)
	{
		unexpected('(');
		return (0);
	}
	return (1);
}

int strange_chars(char *argv)
{
	int i = 0;

	while (argv[i])
	{
		if (isdigit(argv[i]))
			if (argv[i + 1])
				if (argv[i + 1] != '+' && argv[i + 1] != '*' && argv[i + 1] != ')')
				{
					unexpected(argv[i + 1]);
					return (1);
				}
		i++;
	}

	return (0);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
	if (!is_balanced(argv[1]))
		return (1);
	if (strange_chars(argv[1]))
		return (1);
    node *tree = parse_add(&argv[1]);
    if (!tree || g_error)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
	return (0);
}


/*Assignment name:		vbc
Expected files:		*.c *h
Allowed functions:	malloc, calloc, realloc, free, printf, isdigit, write

Write a program that prints the result of a mathematical expression given as argument.
It must handle addition, multiplication and parenthesis. All values are between 0 and 9 included.
In case of an unexpected symbol, you must print "Unexpected token '%c'\n".
If the expression ends unexpectedly, you must print "Unexpected end of input\n".
The same rule applies if finding an unexpected '(' or ')'.
In case of a syscall failure, you must exit with 1.

Examples:
$> ./vbc '1' | cat -e
1$
$> ./vbc '2+3' | cat -e
5$
$> ./vbc '3*4+5' | cat -e
17$
$> ./vbc '3+4*5' | cat -e
23$
$> ./vbc '(3+4)*5' | cat -e
35$
$> ./vbc '(((((2+2)*2+2)*2+2)*2+2)*2+2)*2' | cat -e
188$
$> ./vbc '1+'
Unexpected end of input
$> ./vbc '1+2)' | cat -e
Unexpected token ')'$

File provided: vbc.c, see below.
*/

/*
*/
