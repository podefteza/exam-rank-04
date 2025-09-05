#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

int i = 0;
int error = 0;
char *str = NULL;

void    unexpected(char c)
{
	if (c && !error)
		printf("Unexpected token '%c'\n", c);
	else if (!error)
		printf("Unexpected end of input\n");
	error = 1;
}

int parse_add();

int parse_nb()
{
	if (!str[i] || !isdigit(str[i]))
	{
		unexpected(str[i]);
		return (-1);
	}
	return (str[i++] - '0');
}

int parse_group()
{
	int result = 0;

	if (str[i] == '(')
	{
		i++;
		result = parse_add();
		if (result < 0)
			return (-1);
		if (str[i] != ')')
		{
			unexpected(str[i]);
			return (-1);
		}
		i++;
		return (result);
	}
	return (parse_nb());
}

int parse_multi()
{
	int left = 0;
	int right = 0;

	left = parse_group();
	if (left < 0)
		return (-1);
	while (str[i] == '*')
	{
		i++;
		right = parse_group();
		if (right < 0)
			return (-1);
		left *= right;
	}
	return (left);
}

int parse_add()
{
	int left = 0;
	int right = 0;

	left = parse_multi();
	if (left < 0)
		return (-1);
	while (str[i] == '+')
	{
		i++;
		right = parse_multi();
		if (right < 0)
			return (-1);
		left += right;
	}
	return (left);
}

int is_balanced()
{
	int j = 0;
	int balance = 0;

	while (str[j])
	{
		if (str[j] == '(')
			balance++;
		else if (str[j] == ')')
			balance--;
		if (balance < 0)
		{
			unexpected(')');
			return (0);
		}
		j++;
	}
	if (balance > 0)
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
	i = 0;
	str = argv[1];
	if (!is_balanced())
		return (1);
	int result = 0;
	result = parse_add();
	if (str[i])
	{
		unexpected(str[i]);
		return (1);
	}
	if (!error && result >= 0)
	{
		printf("%d\n", result);
		return (0);
	}
	return (1);
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
