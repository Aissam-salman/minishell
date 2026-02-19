/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 20:30:32 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 21:15:48 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

int	g_signal_glob = 0;

void	ft_minishell_reset(t_minishell *minishell)
{
	if (STDOUT_FILENO != 1)
	{
		close(STDOUT_FILENO);
		dup2(1, STDOUT_FILENO);
	}
	if (STDIN_FILENO != 0)
	{
		close(STDIN_FILENO);
		dup2(0, STDIN_FILENO);
	}
	minishell->head_token = NULL;
	minishell->head_cmd = NULL;
	minishell->state = NORMAL;
	minishell->cached_status = minishell->exit_status ;
	minishell->exit_status = 0;
}

int	ft_run_shell(t_minishell *minishell)
{
	ft_gc_add_node(&minishell->gc, minishell->line);
	add_history(minishell->line);
	if (ft_tokenize(minishell) == GENERAL_ERROR)
		return (GENERAL_ERROR);
	if (checker_token(minishell) == ERR_SYNTAX)
		return (GENERAL_ERROR);
	if (ft_cmd_lst_create(minishell) == GENERAL_ERROR)
		return (GENERAL_ERROR);
	if (minishell->head_cmd)
		ft_exec(minishell);
	return (SUCCESS);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void)argv;
	(void)argc;
	ft_bzero(&minishell, sizeof(t_minishell));
	ft_env_setup(&minishell, envp);
	while (1)
	{
		setup_signal();
		ft_minishell_reset(&minishell);
		minishell.line = readline("foo$> ");
		if (!minishell.line)
			ft_exit(&minishell, minishell.exit_status, NULL);
		if (!*minishell.line)
		{
			free(minishell.line);
			continue ;
		}
		if (ft_run_shell(&minishell) == GENERAL_ERROR)
			continue ;
	}
	ft_exit(&minishell, minishell.cached_status, NULL);
}
