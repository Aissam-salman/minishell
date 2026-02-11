/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/11 16:55:51 by tibras           ###   ########.fr       */
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
		// ft_cmd_print(minishell->head_cmd);
		// ft_printf("Coucou\n");
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
