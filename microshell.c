#include <string.h> // strcmp
#include <stdlib.h> // malloc, free
#include <unistd.h> // chdir

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
	return (1);
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
}

static void exec_generic(char **argv, char **envp)
{
	if (!go_to_next_pipe(cmd)) // Case no pipes
		execute(cmd, envp);
	else // Case pipes
	{
	}
}

int main(int argc, char **argv, char **envp)
{
	int it = 0;
	char **cmd = NULL;

	while (++it < argc)
	{
		cmd = get_next_cmd(argv, &it); // i is modified so that argv[i] is set to the next ";"

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
