#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void close_fd(int fd)
{
	if (fd != -1)
		close(fd);
}

int	picoshell_debug(char **cmds[])
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
	
	// Close final fd_stdin
	if (fd_stdin != 0)
		close(fd_stdin);
		
	int child_count = 0;
	while (wait(&status) > 0) {
		child_count++;
		printf("Child %d: status=%d, WIFEXITED=%d, WEXITSTATUS=%d\n", 
			   child_count, status, WIFEXITED(status), WEXITSTATUS(status));
		if (!WIFEXITED(status) || WEXITSTATUS(status))
			return (1);
	}
	printf("All %d children waited for\n", child_count);
	return (0);
}

int main()
{
    printf("=== Detailed debugging ===\n");
    
    // Test the multi-stage pipeline first (clean state)
    printf("--- Testing multi-stage pipeline FIRST ---\n");
    char *cmd_slow1[] = {"echo", "slow test", NULL};
    char *cmd_slow2[] = {"cat", NULL};  // Pass through the input
    char *cmd_slow3[] = {"cat", NULL};  // Another pass through
    char **cmds_slow[] = {cmd_slow1, cmd_slow2, cmd_slow3, NULL};

    int result = picoshell_debug(cmds_slow);
    printf("Multi-stage pipeline result (clean): %d %s\n\n", result, result == 0 ? "✅" : "❌");
    
    // Now test invalid command
    printf("--- Testing invalid command ---\n");
    char *cmd_invalid[] = {"this_command_absolutely_does_not_exist_xyz123", NULL};
    char **cmds_invalid[] = {cmd_invalid, NULL};

    result = picoshell_debug(cmds_invalid);
    printf("Invalid command result: %d %s\n\n", result, result == 1 ? "✅" : "❌");

    // Test the multi-stage pipeline again (after invalid command)
    printf("--- Testing multi-stage pipeline AFTER invalid ---\n");
    result = picoshell_debug(cmds_slow);
    printf("Multi-stage pipeline result (after invalid): %d %s\n", result, result == 0 ? "✅" : "❌");
    
    return 0;
}
