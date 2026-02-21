/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 20:42:27 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 17:37:45 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env_delone(t_env **head_env, char *target_name)
{
	t_env	*head;
	t_env	*prev;

	prev = NULL;
	head = *head_env;
	while (head)
	{
		if (ft_strcmp(head->name, target_name) == 0 && ft_strcmp(head->name,
				(*head_env)->name) == 0)
		{
			*head_env = head->next;
			head->next = NULL;
			return ;
		}
		else if (ft_strcmp(head->name, target_name) == 0)
		{
			prev->next = head->next;
			return ;
		}
		prev = head;
		head = head->next;
	}
}

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
