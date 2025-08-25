#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

int    picoshell(char **cmds[])
{
	int fd[2];
	int fd_stdin = 0;
	int status = 0;
	int i = 0;
	pid_t pid;

	if (!cmds)
		return (1);

	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (close(fd_stdin), 1);
		else if(!cmds[i + 1])
			fd[0] = fd[1] = -1;

		pid = fork();
		if (pid < 0)
			return(close_fd(fd[0]), close_fd(fd[1]), close_fd(fd_stdin), 1);
		if (pid == 0)
		{
			if (fd_stdin != 0 && dup2(fd_stdin, 0) == -1)
				exit (1);
			if (fd[1] != -1 && dup2(fd[1], 1) == -1)
				exit (1);

			close(fd_stdin);
			close_fd(fd[0]);
			close_fd(fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit (1);
		}
		if (fd_stdin != 0)
			close(fd_stdin);
		close_fd(fd[1]);
		fd_stdin = fd[0];
		i++;
	}
	while (wait(&status) > 0)
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			return (1);
	return (0);
}

int main()
{
	char *cmd1[] = {"ls", "-la", NULL};
	char *cmd2[] = {"wc", NULL};
	char **cmds[] = {cmd1, cmd2, NULL};

	return(picoshell(cmds));
}


/*Assignment name:	picoshell
Expected files:		picoshell.c
Allowed functions:	close, fork, wait, exit, execvp, dup2, pipe
___________________________________________________________________

Write the following function:

int    picoshell(char **cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows
of cmds are an argv array directly usable for a call to execvp. The first
arguments of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur, The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain something to help you test your function.


Examples:
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb
*/
