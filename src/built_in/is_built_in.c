/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_built_in.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/14 14:15:59 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/17 11:56:58 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int is_built_in(t_cmd *cmd)
{
	char *str;

	if (!cmd)
		return (0);
	if (!cmd->args || !cmd->args[0])
		return (0);
	str = cmd->args[0];
	if (ft_strcmp(str, "exit") == 0 ||
		ft_strcmp(str, "echo") == 0 ||
		ft_strcmp(str, "pwd") == 0 ||
		ft_strcmp(str, "cd") == 0 ||
		ft_strcmp(str, "export") == 0 || 
		ft_strcmp(str, "unset") == 0 ||
		ft_strcmp(str, "env") == 0 ||
		ft_strcmp(str, ":") == 0 ||
		ft_strcmp(str, "!") == 0)
		return (1);
	return (0);
}

void run_built_in(t_cmd *cmd, t_minishell *minishell)
{
	char *str;

	if (!cmd->args || !cmd->args[0])
		return;
	str = cmd->args[0];
	minishell->exit_status = 0;
	if (ft_strcmp(str, "exit") == 0)
		ft_buildin_exit(minishell, cmd->args[1]);
	else if (ft_strcmp(str, "export") == 0 )
		minishell->exit_status = ft_export(minishell, 2, cmd->args[1]);
	else if (ft_strcmp(str, "unset") == 0)
		ft_unset(&minishell->head_env, cmd->args[1]);
	else if (ft_strcmp(str, "env") == 0)
		ft_env(minishell->head_env, 1);
	else if (ft_strcmp(str, "echo") == 0)
			ft_echo(cmd->args);
	else if (ft_strcmp(str, "pwd") == 0)
		ft_pwd();
	else if(ft_strcmp(str, "cd") == 0)
		ft_cd(minishell, cmd->args[1]);
	else if (ft_strcmp(str, "!") == 0)
		ft_extra_bin(minishell, 1);
	else if (ft_strcmp(str, ":") == 0)
		ft_extra_bin(minishell, 0);
}
