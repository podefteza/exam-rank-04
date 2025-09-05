#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

int	picoshell_fixed(char **cmds[])
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
			return (close(fd_stdin), 1);

		if (!cmds[i + 1])
			fd[0] = fd[1] = -1;

		pid = fork();
		if (pid < 0)
			return(close_fd(fd[0]), close_fd(fd[1]), close(fd_stdin), 1);

		if (pid == 0)
		{
			if (fd_stdin != 0)
				if (dup2(fd_stdin, 0) < 0)
					exit (1);
			if (fd[1] != -1)
				if (dup2(fd[1], 1) < 0)
					exit (1);
			close_fd(fd[0]), close_fd(fd[1]), close(fd_stdin);
			execvp(cmds[i][0], cmds[i]);
			exit (1);
		}
		if (fd_stdin != 0)
			close(fd_stdin);
		close_fd(fd[1]);
		fd_stdin = fd[0];
		i++;
	}

	// Make sure to close the final fd_stdin
	if (fd_stdin != 0)
		close(fd_stdin);

	while (wait(&status) > 0)
		if (!WIFEXITED(status) || WEXITSTATUS(status))
			return (1);
	return (0);
}

int main()
{
    printf("=== Testing sequence with FIXED version ===\n");

    // First run the invalid command test (like in the main test)
    printf("--- Testing completely invalid command ---\n");
    char *cmd_invalid[] = {"this_command_absolutely_does_not_exist_xyz123", NULL};
    char **cmds_invalid[] = {cmd_invalid, NULL};

    int result = picoshell_fixed(cmds_invalid);
    printf("Invalid command result: %d %s\n", result, result == 1 ? "✅" : "❌");

    // Test pipeline with invalid command in middle
    printf("--- Testing pipeline with invalid command in middle ---\n");
    char *cmd_good1[] = {"echo", "test", NULL};
    char *cmd_bad[] = {"invalid_command_xyz", NULL};
    char *cmd_good2[] = {"cat", NULL};
    char **cmds_mixed[] = {cmd_good1, cmd_bad, cmd_good2, NULL};

    result = picoshell_fixed(cmds_mixed);
    printf("Mixed pipeline result: %d %s\n", result, result == 1 ? "✅" : "❌");

    // NOW test the multi-stage pipeline (exactly as in the main test)
    printf("--- Testing with multi-stage pipeline ---\n");
    char *cmd_slow1[] = {"echo", "slow test", NULL};
    char *cmd_slow2[] = {"cat", NULL};  // Pass through the input
    char *cmd_slow3[] = {"cat", NULL};  // Another pass through
    char **cmds_slow[] = {cmd_slow1, cmd_slow2, cmd_slow3, NULL};

    result = picoshell_fixed(cmds_slow);
    printf("Multi-stage pipeline result: %d %s\n", result, result == 0 ? "✅" : "❌");

    return 0;
}
