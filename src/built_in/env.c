/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:22:25 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/18 09:55:20 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env(t_minishell *minishell)
{
	if (!minishell->head_cmd->args[1])
		ft_env_print(minishell->head_env, STDOUT_FILENO);
	else
		minishell->exit_status = GENERAL_ERROR;
	//WARN: handle if env cmd, error or execve the next
}
