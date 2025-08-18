#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int		fd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid < 0)
		return (close(fd[0]), close(fd[1]), -1);
	if (pid == 0)
	{
		if ((type == 'r' && dup2(fd[1], 1) < 0) || (type == 'w' && dup2(fd[0], 0) < 0))
			exit(1);
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(1);
	}
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
