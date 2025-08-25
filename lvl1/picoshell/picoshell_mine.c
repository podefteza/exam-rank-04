#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/* TODO:
* - validate input
* - create pipe if the current command isn't the last
* - if it is the last, set both FDs to -1 so later it doesn't dup2/close them
* - fork the process
* - child:
*		- dup the result of the previous command to STDIN
*		- dup the write end of the current pipe to STDOUT, so this command's output goes to the pipe
*		- closes previous input and FDs
*		- executes
* - parent:
*		- closes fd_stdin and the new output
*		- sets the new fd_stdin, passing pipe read end to next iteration
* - waits for all commands to complete
* - returns
*/

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
			if (fd_stdin != 0)
				if (dup2(fd_stdin, 0) == -1) // connect previous command's output to this command's STDIN
					exit(1);
			if (fd[1] != -1)
				if (dup2(fd[1], 1) == -1) // connect this command's STDOUT to the pipe for next command
					exit(1);
			close(fd_stdin); // close all FDs (child doesn't need them after dup2)
			close_fd(fd[0]);
			close_fd(fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd_stdin != 0)
			close(fd_stdin); // close previous input, no longer needed
		close_fd(fd[1]); // close write end (child uses it)
		fd_stdin = fd[0]; // pass the read end to next iteration as STDIN source
		i++;
	}
	while (wait(&status) > 0) // wait for all children
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) // check if any failed
			return (1);
	return (0);
}

/*int main()
{
	char *cmd1[] = {"ls", "-la", NULL};
	char *cmd2[] = {"grep", "picoshell", NULL};
	char **cmds[] = {cmd1, cmd2, NULL};

	return(picoshell(cmds));
}*/
