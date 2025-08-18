#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

/*
** ft_popen
** --------------------------------------------
** Launches an executable with argv via execvp and connects a pipe
** either to its stdout (type 'r') or stdin (type 'w').
**
** Parameters:
**  - file: path or name of the executable (resolved by execvp using PATH)
**  - argv: null-terminated argument vector (argv[0] should be the program name)
**  - type: 'r' to read the child's stdout, 'w' to write to the child's stdin
**
** Returns:
**  - on success: a file descriptor usable by the parent
**      * type 'r' -> FD for reading child's stdout (read end of the pipe)
**      * type 'w' -> FD for writing to child's stdin (write end of the pipe)
**  - on error: -1
**
** Notes:
**  - We carefully close the unused ends of the pipe in both parent and child
**    to avoid file descriptor leaks and to ensure EOF semantics work as expected.
**  - In the child, after dup2 we close both pipe FDs before execvp.
*/
int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];     // fd[0] = read end, fd[1] = write end
	pid_t	pid;

	// Validate inputs and mode
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	// Create a unidirectional pipe: parent and child will each keep one end
	if (pipe(fd) == -1)
		return (-1);
	// Fork the process: child will exec the command, parent will return the FD
	pid = fork();
	if (pid < 0)
		return (close(fd[0]), close(fd[1]), -1); // fork failed: clean up both ends
	if (pid == 0)
	{
		// Child process:
		//  - For 'r': parent wants to READ the child's OUTPUT
		//    => connect pipe's write end (fd[1]) to stdout (fd 1)
		//  - For 'w': parent wants to WRITE to the child's INPUT
		//    => connect pipe's read end (fd[0]) to stdin (fd 0)
		if ((type == 'r' && dup2(fd[1], 1) < 0) || (type == 'w' && dup2(fd[0], 0) < 0))
			exit(1);
		// Close both original pipe FDs in the child; they are duplicated now
		close(fd[0]);
		close(fd[1]);
		// Replace the child process image with the target program
		execvp(file, argv);
		// If execvp returns, it failed; exit with error as required
		exit(1);
	}
	// Parent process:
	//  - For 'r': keep the read end (fd[0]), close the write end (fd[1])
	//  - For 'w': keep the write end (fd[1]), close the read end (fd[0])
	if (type == 'r')
		return (close(fd[1]), fd[0]);
	return (close(fd[0]), fd[1]);
}

//test type 'r'
/*int	main()
{
	int fd = ft_popen("ls", (char *const[]){"ls", "-l", NULL}, 'r');
	char buf[1];
	while (read(fd, buf, 1))
		write(1, buf, 1);
	close(fd);
	return (0);
}*/

//test type 'w'
/*int main()
{
	int fd = ft_popen("wc", (char *const[]){"wc", "-w", NULL}, 'w');
	char *input = "Hello world\nThis is a test\nthird line mofo\n";
	write(fd, input, strlen(input));
	close(fd);
	return (0);
}*/

int main()
{
	int fd_ls = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
	int fd_grep = ft_popen("grep", (char *const[]){"grep", "p", NULL}, 'w');
	char buffer[1024];
	int bytes;

	if (fd_grep == -1 || fd_ls == -1)
		return 1;
	while ((bytes = read(fd_ls, buffer, 1024)) > 0)
		write(fd_grep, buffer, bytes);
	close(fd_ls);
	close(fd_grep);
	return 0;
}
