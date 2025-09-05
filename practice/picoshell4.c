#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <string.h>

void close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

int	picoshell(char **cmds[])
{
	int		i = 0;
	int		fd[2];
	int		fd_stdin = 0;
	int		status = 0 ;
	pid_t	pid;

	if (!cmds)
		return (1);
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1) // pipe command if it's not the last
			return (close(fd_stdin), 1);
		else if (!cmds[i + 1]) // last command, no need to pipe
			fd[0] = fd[1] = -1;
		pid = fork();
		if (pid == -1) // cleanup on fork fail
			return(close_fd(fd_stdin), close_fd(fd[0]), close_fd(fd[1]), 1);
		if (pid == 0)
		{
			if (fd_stdin != 0) // if it's not the FIRST command in the pipeline...
				if (dup2(fd_stdin, 0) == -1) // connect previous command's output to this command's STDIN
					exit(1);
			if (fd[1] != -1) // if it's not the LAST command in the pipeline...
				if (dup2(fd[1], 1) == -1) // connect this command's STDOUT to the pipe for next command
					exit(1);
			close(fd_stdin), close_fd(fd[0]), close_fd(fd[1]); // close all FDs (child doesn't need them after dup2)
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd_stdin != 0) // if it's not the FIRST command in the pipeline...
			close(fd_stdin); // close previous input, no longer needed
		close_fd(fd[1]); // close write end (parent doesn't need it, child inherited it)
		fd_stdin = fd[0]; // pass the read end to next iteration as STDIN source
		i++;
	}
	while (wait(&status) > 0) // wait for all children
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) // check if any failed
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
