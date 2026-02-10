/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/10 17:33:42 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	ft_exec(t_minishell *minishell)
{
	int pid;
	// int respipe;
	// int pipefd[2];

	// respipe = pipe(pipefd);
	// if (respipe == -1)
	// 	return;
	pid = fork();
	if (pid == 0)
	{

		execve(minishell->head_cmd->path, minishell->head_cmd->args,minishell->envs );
		perror("EXEC\n");
		exit(1);
	}
	wait(&pid);
	// else
	// {
	// 	waitpid(-1);
	// }
}
