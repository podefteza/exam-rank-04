#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

int i = 0;
int error = 0;
char *str = NULL;

int parse_add();

void	unexpected(char c)
{
	if (c && !error)
		printf("Unexpected token '%c'\n", c);
	else if (!error)
		printf("Unexpected end of input\n");
	error = 1; // set of error so it does not duplicate the print if more errors were found
}

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

	if (str[i] == '(') // group of numbers or operations inside parenthisis
	{
		i++; // increment to the first one
		result = parse_add(); // recusively calls to calculate result
		if (result < 0)
			return (-1);
		if (str[i] != ')') // validate closing parenthesis after parsing expression inside
		{
			unexpected(str[i]);
			return (-1);
		}
		i++; // increment to get out of the nested operation
		return (result); // returns the result of the nested operation
	}
	return (parse_nb()); // if not, it's just a digit
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
			return (unexpected(')'), 0);
		j++;
	}
	if (balance > 0)
			return (unexpected('('), 0);
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
	int result = parse_add();
	if (str[i]) // check for leftover characters
	{
		unexpected(str[i]);
		return (1);
	}
	if (error) // check if any error occurred during parsing
		return (1);
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
