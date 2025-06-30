#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

void	ft_puterror(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while(*arg)
			write(2, arg++, 1);
	write(2, "\n", 1);
}

void ft_execute(char **argv, int i, int tmp_fd, char **envp)
{
	argv[i] = NULL; // null-terminate argv at current separator
	dup2(tmp_fd, STDIN_FILENO); // set STDIN to previous pipe or original input
	close(tmp_fd); // close unused input
	execve(argv[0], argv, envp);
	ft_puterror("error: cannot execute ", argv[0]);
	exit(1);
}


int	main(int argc, char **argv, char **envp)
{
	int	i;
	int fd[2];
	int tmp_fd;
	(void)argc;

	i = 0;
	tmp_fd = dup(STDIN_FILENO); // save original STDIN
	while (argv[i] && argv[i + 1])
	{
		argv = argv + i + 1;
		i = 0;
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|")) // find the next separator
			i++;
		if (strcmp(argv[0], "cd") == 0) // builtin cd
		{
			if (i != 2)
				ft_puterror("error: cd: bad arguments", NULL);
			else if (chdir(argv[1]) != 0)
				ft_puterror("error: cd: cannot change directory to ", argv[1]);
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0)) // execute command without pipe (followed by ; or last command)
		{
			if (fork() == 0) // child process
				ft_execute(argv, i, tmp_fd, envp);
			else // parent process
			{
				close(tmp_fd); // close input from previous pipe
				while(waitpid(-1, NULL, WUNTRACED) != -1); // wait for all children
				tmp_fd = dup(STDIN_FILENO); // reset input for the next command
			}
		}
		else if(i != 0 && strcmp(argv[i], "|") == 0) // command with pipe
		{
			pipe(fd);
			if (fork() == 0) // child process
			{
				dup2(fd[1], STDOUT_FILENO); // set STDOUT to pipe write end
				close(fd[0]); // close unused read end
				close(fd[1]); // close original write end
				ft_execute(argv, i, tmp_fd, envp);
			}
			else // parent process
			{
				close(fd[1]); // close write end
				close(tmp_fd); // close previous input
				tmp_fd = fd[0]; // save read end for next command
			}
		}
	}
	close(tmp_fd);
	return (0);
}

/*
EXEC:
- null-terminate argv[i] to isolate the current command
- set stdin to previous pipe read-end or original stdin using dup2
- close tmp_fd (already duplicated)
- execve to run the command with given argv and envp
- if execve fails, print an error and exit

MAIN:
- save stdin by duplicating it into tmp_fd
- loop while there are more arguments
	- shift argv to start after last separator (argv = argv + i + 1)
	- scan forward to next separator (| or ;) and store index in i
- if command is "cd"
	- if number of arguments != 2, print error
	- else try to chdir(argv[1])
		- if chdir fails, print error with the path
- if command has no pipe (either end of argv or followed by ;)
	- fork a child process
		- in child: call ft_execute()
	- in parent:
		- close tmp_fd (was used as stdin)
		- wait for all child processes
		- reset tmp_fd = dup(STDIN_FILENO)
- if command is followed by a pipe:
	- create a pipe(fd)
		- if pipe fails, print "error: fatal" and exit
	- fork a child process
		- in child:
			- set stdout to pipe's write end (dup2)
			- close pipe fds and tmp_fd
			- exec the command
		- in parent:
			- close pipe write end and tmp_fd (old stdin)
			- store pipe read end in tmp_fd for next command's input
- after the loop, close tmp_fd to avoid fd leak
*/

/*Assignment name  : microshell
Expected files   : microshell.c
Allowed functions: malloc, free, write, close, fork, waitpid, signal, kill, exit, chdir, execve, dup, dup2, pipe, strcmp, strncmp
--------------------------------------------------------------------------------------

Write a program that will behave like executing a shell command
- The command line to execute will be the arguments of this program
- Executable's path will be absolute or relative but your program must not build a path (from the PATH variable for example)
- You must implement "|" and ";" like in bash
	- we will never try a "|" immediately followed or preceded by nothing or "|" or ";"
- Your program must implement the built-in command cd only with a path as argument (no '-' or without parameters)
	- if cd has the wrong number of argument your program should print in STDERR "error: cd: bad arguments" followed by a '\n'
	- if cd failed your program should print in STDERR "error: cd: cannot change directory to path_to_change" followed by a '\n' with path_to_change replaced by the argument to cd
	- a cd command will never be immediately followed or preceded by a "|"
- You don't need to manage any type of wildcards (*, ~ etc...)
- You don't need to manage envpironment variables ($BLA ...)
- If a system call, except execve and chdir, returns an error your program should immediatly print "error: fatal" in STDERR followed by a '\n' and the program should exit
- If execve failed you should print "error: cannot execute executable_that_failed" in STDERR followed by a '\n' with executable_that_failed replaced with the path of the failed executable (It should be the first argument of execve)
- Your program should be able to manage more than hundreds of "|" even if we limit the number of "open files" to less than 30.

for example this should work:
$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

Hints:
Don't forget to pass the envpironment variable to execve

Hints:
Do not leak file descriptors!*/
