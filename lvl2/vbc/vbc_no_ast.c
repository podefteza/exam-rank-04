#include <ctype.h>
#include <stdio.h>

/* TODO - VBC Flow:
* - validate input (argc == 2)
* - initialize global string pointer and index
* - parse_expr(): handle addition (lowest precedence)
*		- parse first term
*		- while '+': skip operator, parse next term, accumulate result
* - parse_term(): handle multiplication (medium precedence)
*		- parse first factor
*		- while '*': skip operator, parse next factor, accumulate result
* - parse_factor(): handle parentheses and numbers (highest precedence)
*		- if '(': skip '(', recursively parse expression, expect ')', skip ')'
*		- else: parse single digit number
* - parse_number(): validate and convert single digit
*		- check if current char is digit, convert to int, advance index
* - error handling:
*		- parsing functions report errors immediately when encountered
*		- main() reports leftover characters after successful parsing
* - return result or error code
*/

int		parse_factor(void);
int		parse_expr(void);

char *g_str; // Global pointer to input string (unchanged)
int g_i = 0; // Global index for current position

void	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);
	else
		printf("Unexpected end of input\n");

}

int	parse_number(void)
{
	if (!g_str[g_i] || !isdigit(g_str[g_i])) // Check if current character is a valid digit
	{
		unexpected(g_str[g_i]);
		return (-1);
	}
	return (g_str[g_i++] - '0'); // Convert char to int and advance index
}

// Parse highest precedence: numbers and parentheses
int	parse_factor(void)
{
	int	result;

	if (g_str[g_i] == '(') // Handle parentheses: (expression)
	{
		g_i++; // Skip opening '('
		result = parse_expr(); // Recursively parse the expression inside parentheses
		if (result == -1)
			return (-1);
		if (g_str[g_i] != ')') // Expect closing ')'
		{
			unexpected(g_str[g_i]);
			return (-1);
		}
		g_i++; // Skip closing ')'
		return (result);
	}
	return (parse_number()); // If not parentheses, must be a number
}

// Parse medium precedence: multiplication (factor * factor * ...)
int	parse_term(void)
{
	int	left;
	int	right;

	left = parse_factor(); // Start with first factor
	if (left == -1)
		return (-1);
	while (g_str[g_i] == '*') // Handle chain of multiplications
	{
		g_i++; // Skip '*' operator
		right = parse_factor(); // Get next factor
		if (right == -1)
			return (-1);
		left *= right; // Accumulate result (left-to-right evaluation)
	}
	return (left);
}

// Parse lowest precedence: addition (term + term + ...)
int	parse_expr(void)
{
	int	left;
	int	right;

	left = parse_term(); // Start with first term
	if (left == -1)
		return (-1);
	while (g_str[g_i] == '+') // Handle chain of additions
	{
		g_i++; // Skip '+' operator
		right = parse_term(); // Get next term
		if (right == -1)
			return (-1);
		left += right; // Accumulate result (left-to-right evaluation)
	}
	return (left);
}

int	main(int argc, char **argv)
{
	int	result;

	if (argc != 2) // Validate command line arguments
		return (1);
	g_str = argv[1]; // Initialize global pointer to input string
	g_i = 0; // Initialize global index to start position
	result = parse_expr(); // Parse the entire expression (starts at lowest precedence)
	if (result == -1) // Error during parsing - already reported
		return (1);
	if (g_str[g_i]) // Parsing succeeded but leftover characters remain
	{
		unexpected(g_str[g_i]);
		return (1);
	}
	printf("%d\n", result); // Success: print result
	return (0);
}

/*Assignment name:		vbc
Expected files:		*.c *h
Allowed functions:	malloc, calloc, realloc, free, printf, isdigit, write

Write a program that prints the result of a mathematical expression given as argument.
It must handle addition,
	multiplication and parenthesis. All values are between 0 and 9 included.
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
