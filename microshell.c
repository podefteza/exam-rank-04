#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int	error(char *str)
{
	while (*str)
		write(2, str++, 1);
	return (1);
}

int	cd(char **argv, int i)
{
	if (i != 2) // cd must be followed by exactly one argument
		return (error("error: cd: bad arguments\n"));
	else if (chdir(argv[1]) == -1)
		return (error("error: cd: cannot change directory to "), error(argv[1]),
			error("\n"));
	return (0);
}

int	execute(char **argv, char **envp, int i)
{
	int	fd[2];  // fd[0] = read end of pipe; fd[1] = write end of pipe
	int	status;
	int	has_pipe;
	int	pid;

	has_pipe = argv[i] && !strcmp(argv[i], "|");    // check if it has pipe
	if (!has_pipe && !strcmp(*argv, "cd"))          // no pipe and is cd
		return (cd(argv, i));
	if (has_pipe && pipe(fd) == -1)                 // error in pipe
		return (error("error: fatal\n"));
	pid = fork();
	if (pid == 0)                                       // child process
	{
		argv[i] = 0;	// null terminate to break the command list
		if (has_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) // dup2 replaces stdout with pipe's write end; closes both ends of pipe after duplicating
			return (error("error: fatal\n"));
		if (!strcmp(*argv, "cd"))   // if it's cd (should not happen in a child process... added defensively)
			return (cd(argv, i));
		execve(*argv, argv, envp);
		return (error("error: cannot execute "), error(*argv), error("\n"));
	}
	waitpid(pid, &status, 0);	// wait for the child process to finish
	if (has_pipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1)) // parent process: redirect stdin to read from pipe, so the next command will receive the previous output
		return (error("error: fatal\n"));
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int	main(int argc, char **argv, char **envp)
{
	int	i;
	int	status;

	i = 0;
	status = 0;
	if (argc > 1)
	{
		while (argv[i] && argv[++i])
		{
			argv += i;	// skip past the current command and its separator
			i = 0;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))	// skip until NULL, pipe or semicolon
				i++;
			if (i)
				status = execute(argv, envp, i);
		}
	}
	return (status);
}

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
- You don't need to manage environment variables ($BLA ...)
- If a system call, except execve and chdir, returns an error your program should immediatly print "error: fatal" in STDERR followed by a '\n' and the program should exit
- If execve failed you should print "error: cannot execute executable_that_failed" in STDERR followed by a '\n' with executable_that_failed replaced with the path of the failed executable (It should be the first argument of execve)
- Your program should be able to manage more than hundreds of "|" even if we limit the number of "open files" to less than 30.

for example this should work:
$>./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell
microshell
i love my microshell
$>

Hints:
Don't forget to pass the environment variable to execve

Hints:
Do not leak file descriptors!*/
