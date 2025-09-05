#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

int	picoshell(char **cmds[])
{
	int status = 0;
	int i = 0;
	int fd_stdin = 0;
	int fd[2];
	pid_t pid;

	if (!cmds)
		return (1);

	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) < 0)
			return (1);

		if (!cmds[i + 1])
			fd[0] = fd[1] = -1;

		pid = fork();

		if (pid < 0)
			return (close_fd(fd[0]), close_fd(fd[1]), close(fd_stdin), 1);

		if (pid == 0)
		{
			if (fd_stdin != 0)
				if (dup2(fd_stdin, 0) < 0)
					exit (1);
			if (fd[1] != -1)
				if (dup2(fd[1], 1) < 0)
					exit (1);
			close_fd(fd[0]), close_fd(fd[1]);
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
		if (!WIFEXITED(status) || WEXITSTATUS(status))
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
