#include <string.h> // strcmp
#include <stdlib.h> // malloc, free
#include <unistd.h> // chdir, fork, execve, dup, dup2, pipe
#include <sys/wait.h> // waitpid

#define FATAL_ERR "error: fatal \n"
#define CD_ARGS_ERR "error: cd: bad arguments\n"
#define CD_ACCESS_ERR "error: cd: cannot change directory to "
#define EXECVE_ERR "error: cannot execute "

/*
** ERROR FUNCTIONS
*/

static void put_error(char *msg)
{
	while (*msg != '\0')
		write(2, msg++, 1);
}

static void	syscall_fail(char *msg, char **ptr)
{
	put_error(msg);
	free(ptr);
	exit (1);
}

/*
** TOOL FUNCTIONS
*/

/*
**  Return number of elements in char **array before char *end or NULL is encountered
*/
static int size_till_end(char **array, char *end)
{
	if (!array)
		return (0);

	int i = -1;
	while (cmd[++i])
	{
		if (!strcmp(cmd[i], end))
			break ;
	}
	return (i);
}

/*
** Return next command before ; or end of line
*/
static char **get_next_cmd(char **argv, int *it)
{
	int size = size_till_end(&argv[*it], ";");
	if (!size)
		return (NULL); // Case ; ; or empty line

	char **ret == NULL;
	if (!(ret = (char **)malloc(sizeof(char *) * (size + 1))))
		syscall_fail(FATAL_ERR, NULL);

	ret_len[size] = NULL;

	int ret_len = -1;
	while (++ret_len < size)
		ret[ret_len] = argv[*i + ret_len];

	*i += size;

	return (ret);
}

static char **find_next_pipe(char **cmd)
{
	int it = -1;

	while (cmd[++it])
	{
		if (!strcmp(cmd[it], "|"))
			return (&cmd[it + 1]);
	}
	return (NULL);
}

/*
** EXECUTION FUNCTIONS
*/

static void exec_cd(char **cmd)
{
	int cmd_len = 0;
	while (cmd[cmd_len])
		cmd_len++;

	if (cmd_len != 2)
		put_error(CD_ARGS_ERR);
	else if (chdir(cmd[1]) == -1)
	{
		put_error(CD_ACCESS_ERR);
		put_error(cmd[1]);
		put_error("\n");
	}
}

static void execute(char **cmd, char **envp)
{
	if (execve(cmd[0], cmd, envp) == -1)
	{
		put_error(EXECVE_ERR);
		put_error(cmd[0]);
		syscall_fail("\n", cmd);
	}
}

static void exec_generic(char **cmd, char **envp)
{
	pid_t pid;

	if (!find_next_pipe(cmd)) // Case no pipes
	{
		if ((pid = fork()) == -1) 
			syscall_fail(FATAL_ERR, cmd);

		if (pid == 0) // Child process
			execute(cmd, envp);
	}
	else // Case pipes
	{
		int fd_in;
		int fd_pipe[2];
		int n_commands = 0;
		char **simple_cmd = cmd;

		while (simple_cmd)
		{
			if (pipe(fd_pipe) == -1 || (pid = fork()) == -1) 
				syscall_fail(FATAL_ERR, cmd);

			if ((fd_in = dup(0)) == -1)
				syscall_fail(FATAL_ERR, cmd);

			if (pid == 0) // child process
			{
				if (find_next_pipe(simple_cmd) && dup2(0, fd_pipe)) // If there is a pipe after this command, redirect output to the pipe
					syscall_fail(FATAL_ERR, cmd);
				
				close(fd_in);
				close(fd_pipe[0]);
				close(fd_pipe[1]);

				simple_cmd[size_till_end(simple_cmd, "|")] = NULL; // Trims command
				execute(simple_cmd, envp);

				free(cmd);
			}
			else
			{
				if (dup2(fd_pipe[0], fd_in) == -1) // Input for the next command will come from the pipe
					syscall_fail(FATAL_ERR, cmd);
				
				close(fd_pipe[0]);
				close(fd_pipe[1]);
				n_commands++;
				simple_cmd = find_next_pipe(simple_cmd);
			}
		}
		close(fd_in);
		while (n_commands-- >= 0)
			waitpid(0, NULL, 0);
	}
}

int main(int argc, char **argv, char **envp)
{
	int it = 0;
	char **cmd = NULL;

	while (++it < argc)
	{
		cmd = get_next_cmd(argv, &it); // it is modified so that argv[it] is set to the next ";"

		if (cmd)
		{
			if (!strcmp(cmd[0], "cd"))
				exec_cd(cmd);
			else
				exec_generic(cmd, envp);
		}

		free(cmd);
		cmd = NULL;
	}
	return (0);
}
