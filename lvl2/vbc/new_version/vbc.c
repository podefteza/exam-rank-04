#include "vbc.h"
/*
 * Use the given functions as helpers:
 * node	*new_node(node n);
 * void	destroy_tree(node *n);
 * void	unexpected(char c) //printf("Unexpected end of file\n"); // change "file" -> "input"
 * int	eval_tree(node *tree) // return (0); // added this at the end
 */

/*
 * LOWEST PRECEDENCE: Parse numbers and parenthesized groups
 * This is the bottom of our precedence hierarchy
 * Handles: single digits (0-9) and expressions in parentheses
 */
node	*parse_number_or_group(char **str)
{
	node	*res;
	node	box;

	res = NULL;
	if (**str == '(') // Found opening parenthesis
	{
		(*str)++; // Skip '('
		res = parse_addition(str); // Recursively parse the content inside parentheses
		if (!res || **str != ')') // Check for matching closing parenthesis
		{
			destroy_tree(res);
			unexpected(**str);
			return (NULL);
		}
		(*str)++; // Skip ')'
		return (res);
	}
	if (isdigit(**str)) // Found a single digit
	{
		box.type = VAL; // Create value node
		box.val = **str - '0'; // Convert char to int
		res = new_node(box);
		(*str)++; // Move to next character
		return (res);
	}
	unexpected(**str); // Invalid character encountered
	return (NULL);
}

/*
 * MEDIUM PRECEDENCE: Parse multiplication operations
 * Handles: sequences like "2*3*4" (left-associative)
 * Calls parse_number_or_group for operands
 */
node	*parse_multiplication(char **str)
{
	node	*left;
	node	*right;
	node	box;

	left = parse_number_or_group(str); // Parse first operand (number or parentheses)
	if (!left)
		return (NULL);
	while (**str == '*') // Continue while we find multiplication operators
	{
		(*str)++; // Skip '*'
		right = parse_number_or_group(str); // Parse next operand
		if (!right)
			return (destroy_tree(left), NULL);
		// Create new multiplication node with left and right children
		box.l = left;
		box.r = right;
		box.type = MULTI;
		left = new_node(box); // This becomes the new left operand for next iteration
	}
	return (left); // Return the complete multiplication subtree
}

/*
 * HIGHEST PRECEDENCE: Parse addition operations
 * Handles: sequences like "1+2+3" (left-associative)
 * Calls parse_multiplication for operands (respecting operator precedence)
 * This is the entry point for parsing expressions
 */
node	*parse_addition(char **str)
{
	node	*left;
	node	*right;
	node	box;

	left = parse_multiplication(str); // Parse first operand (could be multiplication)
	if (!left)
		return (NULL);
	while (**str == '+') // Continue while we find addition operators
	{
		(*str)++; // Skip '+'
		right = parse_multiplication(str); // Parse next operand (respects * precedence)
		if (!right)
			return (destroy_tree(left), NULL);
		// Create new addition node with left and right children
		box.l = left;
		box.r = right;
		box.type = ADD;
		left = new_node(box); // This becomes the new left operand for next iteration
	}
	return (left); // Return the complete expression tree
}

/*
 * VALIDATION: Check if parentheses are balanced in the input string
 * Returns 1 if balanced, 0 if unbalanced
 * This prevents parsing invalid expressions like "((1+2)" or "1+2))"
 */
int	is_balanced(char *str)
{
	int	balanced = 0; // Counter for open parentheses
	int i = 0;

	while (str[i])
	{
		if (str[i] == '(')
			balanced++; // Increment for each opening parenthesis
		else if (str[i] == ')')
		{
			if (!balanced) // Found ')' without matching '('
				return (0);
			balanced--; // Decrement for each closing parenthesis
		}
		i++;
	}
	return (balanced == 0); // Should be 0 if all parentheses are matched
}

/*
 * MAIN FUNCTION: Entry point for the Very Basic Calculator
 *
 * FLOW:
 * 1. Validate command line arguments
 * 2. Check parentheses balance
 * 3. Parse expression into Abstract Syntax Tree (AST)
 * 4. Evaluate the AST and print result
 * 5. Clean up memory
 */
int	main(int argc, char **argv)
{
	if (argc != 2) // Expect exactly one expression argument
		return (1);
	if (!is_balanced(argv[1])) // Check parentheses balance first
		return (printf("Unexpected token ')'\n"), 1);

	// Parse the expression into an Abstract Syntax Tree
	// argv[1] is passed by reference so parser can advance through string
	node *tree = parse_addition(&argv[1]); // Start with highest precedence parser
	if (!tree) // Parsing failed (syntax error)
		return (1);

	// Evaluate the AST and print result
	printf("%d\n", eval_tree(tree));

	// Clean up allocated memory
	destroy_tree(tree);
	return (0); // Success
}
