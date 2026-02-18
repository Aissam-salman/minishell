/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wait.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:46:29 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 18:05:52 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handler_status(int status, t_cmd *cmd, t_minishell *minishell)
{
	int	sig;
	int i;

	if (WIFSIGNALED(status))
	{
		minishell->exit_status = WEXITSTATUS(status);
		sig = WTERMSIG(status);
		if (sig == SIGQUIT)
		{
			ft_printf("Quit");
			if (WCOREDUMP(status))
			{
				ft_printf("\t\t\t\t%s", "(core dumped)");
			}
			i= 0;
			while (cmd->args[i])
			{
				ft_printf(" %s", cmd->args[i]);
				i++;
			}
			ft_printf("\n");
		}
		else if (sig == SIGINT)
		{
			ft_printf("\n");
		}
	}
	// if (WIFEXITED(status) > 0)
	// 	ft_error(minishell, WEXITSTATUS(status), strerror(WEXITSTATUS(status)), NULL);
}

void	ft_wait_subprocess(t_minishell *minishell, int size_cmd, int *pids)
{
	int		status;
	t_cmd	*cmd;
	int		i;

	cmd = minishell->head_cmd;
	i = 0;
	while (i < size_cmd)
	{
		waitpid(pids[i], &status, 0);
		handler_status(status, cmd, minishell);
		if (WIFEXITED(status))
			minishell->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			minishell->exit_status = 128 + WTERMSIG(status);
		cmd = cmd->next;
		i++;
	}
}
