#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

int i = 0;
int error = 0;
char *str = NULL;

int parse_add();

void    unexpected(char c)
{
    if (c && !error)
        printf("Unexpected token '%c'\n", c);
    else if (!error)
        printf("Unexpected end of input\n");
	error = 1;
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
	int balanced = 0;

	while (str[j])
	{
		if (str[j] == '(')
			balanced++;
		else if (str[j] == ')')
			balanced--;
		if (balanced < 0)
		{
			unexpected(')');
			return (0);
		}
		j++;
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
	i = 0;
	str = argv[1];
	if (!is_balanced())
		return (1);
	int result = parse_add();
	if (result < 0 || error)
		return (1);
	if (str[i])  // Check if entire string was consumed
	{
		unexpected(str[i]);
		return (1);
	}
	printf("%d\n", result);
	return (0);
}

/*Assignment name  : vbc
Expected files   : *.c *.h
Allowed functions: malloc, calloc, realloc, free, printf, isdigit, write
--------------------------------------------------------------------------------

Write a program that will print the result of a math expression given as
argument.
You must handle the operations + * and the parenthesis.
You don't have to handle whitespaces in the expression.
All the values in the expression will be between 0 and 9 included.
In case of unexpected symbol or inappropriate parenthesis, you will
print "Unexpected token '%c'\n" and exit with the code 1 (if the
symbol is the end of input you will print: "Unexpected end of input\n").
In case of a syscall failure you will just exit with the code 1.

You will find in this directory the beginning of the code you need to write.

For example this should work:
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
$> ./vbc '1+' | cat -e
Unexpected end of input$
$> ./vbc '1+2)' | cat -e
Unexpected token ')'$
$> ./vbc '1+2+3+4+5' | cat -e
15$
$> ./vbc '2*4+9+3+2*1+5+1+6+6*1*1+8*0+0+5+0*4*9*5*8+9*7+5*1+3+1+4*5*7*3+0*3+4*8+8+8+4*0*5*3+5+4+5*7+9+6*6+7+9*2*6*9+2+1*3*7*1*1*5+1+2+7+4+3*4*2+0+4*4*2*2+6+7*5+9+0+8*4+6*7+5+4*4+2+5*5+1+6+3*5*9*9+7*4*3+7+4*9+3+0+1*8+1+2*9*4*5*1+0*1*9+5*3*5+9*6+5*4+5+5*8*6*4*9*2+0+0+1*5*3+6*8*0+0+2*3+7*5*6+8+6*6+9+3+7+0*0+5+2*8+2*7*2+3+9*1*4*8*7*9+2*0+1*6*4*2+8*8*3*1+8+2*4+8*3+8*3+9*5+2*3+9*5*6*4+3*6*6+7+4*8+0+2+9*8*0*6*8*1*2*7+0*5+6*5+0*2+7+2+3+8*7+6+1*3+5+4*5*4*6*1+4*7+9*0+4+9*8+7+5+6+2+6+1+1+1*6*0*9+7+6*2+4*4+1*6*2*9+3+0+0*1*8+4+6*2+6+2*7+7+0*9+6+2*1+6*5*2*3*5*2*6*4+2*9*2*4*5*2*2*3+8+8*3*2*3+0*5+9*6+8+3*1+6*9+8+9*2*0+2' | cat -e
94305$
$> ./vbc '(1)' | cat -e
1$
$> ./vbc '(((((((3)))))))' | cat -e
3$
$> ./vbc '(1+2)*3' | cat -e
9$
$> ./vbc '((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)' | cat -e
2254$
$> ./vbc '((1+3)*12+(3*(2+6))' | cat -e
Unexpected token '2'$*/
