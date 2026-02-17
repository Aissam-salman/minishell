/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:22:25 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/13 20:46:51 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_env(t_minishell *minishell, int outfd)
{
	if (!minishell->head_cmd->args[1])
		ft_env_print(minishell->head_env, outfd);
	//WARN: handle if env cmd, error or execve the next
}
