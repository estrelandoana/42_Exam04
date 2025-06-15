/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaula-l <apaula-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 14:00:04 by apaula-l          #+#    #+#             */
/*   Updated: 2025/06/15 15:20:12 by apaula-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


void	err(char *str)
{
	while (*str)
		write(2, str++, 1);
}

int	cd(char **argv, int i)
{
	if (i < 2 || !argv[1])
		return (err("error: cd: bad arguments\n"), 1);
	if (chdir(argv[1]) < 0)
  {
		err("error: cd: cannot change directory to ");
		err(argv[1]);
	        err("\n");
        	return (1);
  }
	return (0);
}

void	set_pipe(int has_pipe, int *fd, int end)
{
	if (has_pipe && (dup2(fd[end], end) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
	{
		err("error: fatal\n");
		exit (1);
	}
}

int	exec(char **argv, int i, char **envp)
{
	int	has_pipe;
	int	fd[2];
	int	status;
	int	pid;

	has_pipe = argv[i] && !strcmp(argv[i], "|");
	if (!has_pipe && !strcmp(argv[0], "cd"))
		return (cd(argv, i));
	if (has_pipe && pipe(fd) == -1)
	{
		err("error: fatal\n");
		exit (1);
	}
	if ((pid = fork()) == -1)
	{
		err("error: faltal\n");
		exit (1);
	}
	if (!pid)
	{
		argv[i] = 0;
		set_pipe(has_pipe, fd, 1);
		if (!strcmp(argv[0], "cd"))
			exit(cd(argv, i));
		execve(argv[0], argv, envp);
		err("error: cannot execute ");
		err(argv[0]);
		err("\n");
		exit (1);
	}
	waitpid(pid, &status, 0);
	set_pipe(has_pipe, fd, 0);
	return (WIFEXITED(status) && WEXITSTATUS(status));
}

int main (int argc, char **argv, char **envp)
{
	(void)argc;
	int i = 0;
	int status = 0;

	while (argv[i])
	{
		argv += i + 1;
		i = 0;
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (i)
			status = exec(argv, i, envp);
	}
	return (status);
}
