#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void	error(char *msg, char *arg)
{
	while (*msg) write(2, msg++, 1);
	if (arg) while (*arg) write(2, arg++, 1);
	write(2, "\n", 1);
}

void	exec_cmd(char **argv, int i, int tmp_fd, char **env)
{
	argv[i] = NULL;
	dup2(tmp_fd, 0);
	close(tmp_fd);
	execve(argv[0], argv, env);
	error("error: cannot execute ", argv[0]);
	exit(1);
}

int	main(int argc, char **argv, char **env)
{
	int	i = 0, fd[2], tmp_fd = dup(0);
	(void)argc;

	while (argv[i] && argv[i + 1])
	{
		argv += i + 1;
		for (i = 0; argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"); i++);

		if (!strcmp(argv[0], "cd"))
		{
			if (i != 2) error("error: cd: bad arguments", NULL);
			else if (chdir(argv[1])) error("error: cd: cannot change directory to ", argv[1]);
		}
		else if (i != 0 && (!argv[i] || !strcmp(argv[i], ";")))
		{
			if (!fork())
			{
				exec_cmd(argv, i, tmp_fd, env);
				close(tmp_fd);
			}
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, 2) != -1);
				tmp_fd = dup(0);
			}
		}
		else if (i != 0 && (!argv[i] || !strcmp(argv[i], "|")))
		{
			pipe(fd);
			int pid = fork();
			if (pid == 0)
			{
				dup2(fd[1], 1);
				close(fd[0]);
				close(fd[1]);
				exec_cmd(argv, i, tmp_fd, env);
				close(tmp_fd);
				exit(1);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
				waitpid(pid, NULL, 0);
			}
			close(fd[0]);
		}
	}
	close(tmp_fd);
	return (0);
}
