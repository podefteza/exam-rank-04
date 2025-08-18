#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/* TODO:
* - validate input
* - create pipe
* - fork: child will exec the command, parent will return the FD
* - child:
*		- for 'r' dup child STDOUT fd[1]
*		- for 'w' dup child STDIN fd[0]
*		- close FDs
*		- execvp
* - parent:
*		- for 'r' return read end, close write end (fd[1])
*		- for 'w' return write end, close read end (fd[0])
*/

int ft_popen(const char *file, char *const argv[], char type)
{
	int fd[2];
	pid_t pid;

	if ((type != 'w' && type != 'r') || !file || !argv) // validate input
		return (-1);

	if (pipe(fd) == -1) // create pipe
		return (-1);

	pid = fork(); // fork: child will exec the command, parent will return the FD
	if (pid < 0) // fork fail
		return (close(fd[0]), close(fd[1]), -1);
	if (pid == 0)  // child
	{
		if (type == 'r')
			if (dup2(fd[1], 1) < 0) // dup pipe's write end to STDOUT
				exit (1);
		if (type == 'w')
			if (dup2(fd[0], 0) < 0) // dup pipe's read end to STDIN
				exit (1);
		close(fd[0]); // close both FDs since they were duplicated
		close(fd[1]);
		execvp(file, argv); // exec the program
		exit (1);
	}
	if (type == 'r')
		return (close(fd[1]), fd[0]); // parent: for 'r' return read end, close write end (fd[1])
	return (close(fd[0]), fd[1]); // parent: for 'w' return write end, close read end (fd[0])
}


/*Assignment name  : ft_popen
Expected files   : ft_popen.c
Allowed functions: pipe, fork, dup2, execvp, close, exit
--------------------------------------------------------------------------------------

Write the following function:

int ft_popen(const char *file, char *const argv[], char type);

The function must launch the executable file with the arguments argv (using execvp).
If type is 'r' the function must return a file descriptor connected to the output of the command.
If type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

For example, the function could be used like that:

int main()
{
    int  fd;
    char *line;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    while ((line = get_next_line(fd)))
        ft_putstr(line);
    return (0);
}

int	main() {
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	dup2(fd, 0);
	fd = ft_popen("grep", (char *const []){"grep", "c", NULL}, 'r');
	char	*line;
	while ((line = get_next_line(fd)))
		printf("%s", line);
}

Hints:
Do not leak file descriptors!
This exercise is inspired by the libc's popen().
*/
