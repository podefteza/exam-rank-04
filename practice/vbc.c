#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

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
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
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

node *parse_nr_or_group(char **str);
node *parse_multi(char **str);
node *parse_add(char **str);

node *parse_nr_or_group(char **str)
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
		return(res);
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

node *parse_multi(char **str)
{
	node *left;
	node *right;
	node box;

	left = parse_nr_or_group(str);
	if (!left)
		return (NULL);
	while(**str == '*')
	{
		(*str)++;
		right = parse_nr_or_group(str);
		if (!right)
			return (destroy_tree(left), NULL);
		box.l = left;
		box.r = right;
		box.type = MULTI;
		left = new_node(box);
	}
	return (left);
}

node *parse_add(char **str)
{
	node *left;
	node *right;
	node box;

	left = parse_multi(str);
	if (!left)
		return (NULL);
	while(**str == '+')
	{
		(*str)++;
		right = parse_multi(str);
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
			{
				unexpected(')');
				return (0);
			}
			balanced--;
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

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
	if (!is_balanced(argv[1]))
		return (1);
	node *tree = parse_add(&argv[1]);
    if (!tree)
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

/*#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

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
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");
}

int accept(char **s, char c)
{
    if (**s)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}

//...

node    *parse_expr(char *s)
{
    //...

    if (*s)
    {
        destroy_tree(ret);
        return (NULL);
    }
    return (ret);
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
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
	return (0);
}
*/
