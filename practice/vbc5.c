#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

int g_error = 0;
char *g_str = NULL;
int g_i = 0;

int parse_add();
int parse_multi();
int group();

void    unexpected(char c)
{
    if (c && !g_error)
        printf("Unexpected token '%c'\n", c);
    else if (!g_error)
        printf("Unexpected end of input\n");
	g_error = 1;
}

int number()
{
	if (!g_str[g_i] || !isdigit(g_str[g_i]))
	{
		unexpected(g_str[g_i]);
		return (-1);
	}
	int nr = 0;
	nr = g_str[g_i] - '0';
	g_i++;
	return (nr);
}

int group()
{
	int result;

	if (g_str[g_i] == '(')
	{
		g_i++;
		result = parse_add();
		if (result < 0)
			return (-1);
		if (g_str[g_i] != ')')
		{
			unexpected(g_str[g_i]);
			return(-1);
		}
		g_i++;
		return(result);
	}
	return (number());
}

int parse_multi()
{
	int left;
	int right;

	left = group();
	if (left < 0)
		return (-1);
	while (g_str[g_i] == '*')
	{
		g_i++;
		right = group();
		if (right < 0)
			return (-1);
		left = left * right;
	}
	return (left);
}

int parse_add()
{
	int left;
	int right;

	left = parse_multi();
	if (left < 0)
		return (-1);
	while (g_str[g_i] == '+')
	{
		g_i++;
		right = parse_multi();
		if (right < 0)
			return (-1);
		left = left + right;
	}
	return (left);
}

int is_balanced()
{
	int i = 0;
	int balanced = 0;

	while (g_str[i])
	{
		//printf("now checking: %c\n", g_str[i]);
		if (g_str[i] == '(')
			balanced++;
		else if (g_str[i] == ')')
			balanced--;
		//printf("balanced: %d\n", balanced);
		if (balanced < 0)
		{
			unexpected(g_str[i]);
			return (0);
		}
		i++;
	}
	if (balanced > 0)
		return (unexpected('('), 0);
	return (1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
	g_str = argv[1];
	//printf("%s\n", g_str);
	g_i = 0;
	if (!is_balanced())
		return (1);
	int result = parse_add();
    /*node *tree = parse_expr(argv[1]);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);*/
	if (result < 0 || g_error)
		return (-1);
	printf("%d\n", result);
	return (0);
}


/*Assignment name : vbc
Expected files : *.c *.h
Allowed functions: malloc, calloc, realloc, free, printf, isdigit, write

Write a program that prints the result of a mathematical expression given as argument.

It must handle addition (+), multiplication (*), and parentheses.
All values are single digits between 0 and 9 (inclusive).
Error handling:

In case of an unexpected symbol, you must print:
"Unexpected token '%c'\n"
If the expression ends unexpectedly, you must print:
"Unexpected end of input\n"
The same rule applies if you find an unexpected '(' or ')'.
In case of a syscall failure, you must exit with status code 1.
Examples:

./vbc '1' | cat -e
out: 1$

./vbc '2+3' | cat -e
out: 5$

./vbc '3*4+5' | cat -e
out: 17$

./vbc '3+4*5' | cat -e
out: 23$

./vbc '(3+4)*5' | cat -e
out: 35$

./vbc '(((((2+2)*2+2)*2+2)*2+2)*2+2)*2' | cat -e
out: 188$

./vbc '1+'
out: Unexpected end of input

./vbc '1+2)' | cat -e
out: Unexpected token ')'
File provided: vbc.c*/
