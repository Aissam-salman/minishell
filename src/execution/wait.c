/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:46:29 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 15:23:14 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void handler_status(int status, t_cmd *cmd)
{
	int sig;

	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGQUIT)
		{
			ft_printf("Quit");

			if (WCOREDUMP(status))
			{
				ft_printf("\t\t\t\t%s","(core dumped)");
			}
			ft_printf(" %s %s\n", cmd->args[0], cmd->args[1]);
		}
		else if (sig == SIGINT)
		{
			ft_printf("\n");
		}
		if (WIFEXITED(status) > 0)
			ft_printf("Error: %s\n", strerror(status));
	}
}

void ft_wait_subprocess(t_minishell *minishell, int size_cmd, int *pids)
{
	int status;
	t_cmd *cmd;

	cmd = minishell->head_cmd;
	int i = 0;
	while (i < size_cmd)
	{
		waitpid(pids[i], &status, 0);
		handler_status(status, cmd);
		cmd = cmd->next;
		i++;
	}
}
