#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/*
** picoshell
** ---------------------------------------------------------------
** Execute a pipeline of commands. Each cmds[i] is a null-terminated
** argv vector suitable for execvp, with cmds[i][0] being the program.
**
** Example: cmds = { {"echo","hello",NULL}, {"wc","-c",NULL}, NULL }
** Behavior: echo ... | wc -c
**
** Returns:
**  - 0 if all children exited normally with status 0
**  - 1 on any error (syscall failure or non-zero child exit)
**
** Notes:
**  - We chain processes with pipes. For each command except the last, we
**    create a pipe. The child's stdout is dup2'ed to the write end, and the
**    next command will read from the read end.
**  - We carefully close unused FDs in both parent and child to avoid leaks
**    and to ensure EOF propagates correctly.
*/
int	picoshell(char **cmds[])
{
	int		i = 0;
	int		fd[2];        // current pipe: fd[0] = read end, fd[1] = write end
	int		in = 0;        // the read-end FD that becomes stdin for the next cmd
	pid_t	pid;
	int		status, res = 0;

	// Validate input list
	if (!cmds)
		return (1);
	while (cmds[i])
	{
		if (cmds[i + 1] && pipe(fd) == -1) // If there is a next command, we need a pipe to connect to it.
			return (close(in), 1);
		else if (!cmds[i + 1]) // Last command in pipeline: no pipe needed; mark FDs invalid so dup2/close are skipped.
			fd[0] = fd[1] = -1;
		pid = fork();
		if (pid < 0)
		{
			// Fork failed: close any open FDs and bail out.
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
			// Child: wire up stdin/stdout, close unused ends, then exec.
			if (in != 0 && dup2(in, STDIN_FILENO) == -1)
				exit(1);
			if (fd[1] != -1 && dup2(fd[1], STDOUT_FILENO) == -1)
				exit(1);
			close(in);
			if (fd[0] != -1)
				close(fd[0]);
			if (fd[1] != -1)
				close(fd[1]);
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		// Parent: close ends we don't need anymore.
		if (in != 0)
			close(in);
		if (fd[1] != -1)
			close(fd[1]);
		in = fd[0];  // pass the read end to next iteration as its stdin source
		i++;
	}
	// Reap all children and detect any non-zero exit status.
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
