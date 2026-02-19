/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 20:42:27 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 21:19:59 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_unset(t_env **head_env, char **args)
{
	int	i;

	if (!args || !*args || !args[1])
		return ;
	i = 0;
	while (args[i])
	{
		ft_env_delone(head_env, args[i]);
		i++;
	}
}
