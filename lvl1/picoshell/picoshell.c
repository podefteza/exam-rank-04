#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	picoshell(char **cmds[])
{
	int		i = 0;
	int		fd[2];
	int		in = 0;
	pid_t	pid;
	int		status, res = 0;

	if (!cmds)
		return (1);
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1)
			return (close(in), 1);
		else if (!cmds[i + 1])
			fd[0] = fd[1] = -1;
		pid = fork();
		if (pid < 0)
		{
			if (in != 0)
				close(in);
			if (fd[0] != -1)
				close(fd[0]);
			if (fd[1] != -1)
				close(fd[1]);
			return (1);
		}
		if (pid == 0)
		{
			if (in != 0 && dup2(in, STDIN_FILENO) == -1)
				exit(1);
			if (fd[1] != -1 && dup2(fd[1], STDOUT_FILENO) == -1)
				exit(1);
			if (in != 0)
				close(in);
			if (fd[0] != -1)
				close(fd[0]);
			if (fd[1] != -1)
				close(fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (in != 0)
			close(in);
		if (fd[1] != -1)
			close(fd[1]);
		in = fd[0];
		i++;
	}
	while (wait(&status) > 0)
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			res = 1;
	return (res);
}

/*int main()
{
	char *cmd1[] = {"ls", "-la", NULL};
	char *cmd2[] = {"wc", NULL};
	char **cmds[] = {cmd1, cmd2, NULL};

	return(picoshell(cmds));
}*/
