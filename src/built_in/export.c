/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/18 11:50:58 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env_format_check(char *str)
{
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (GENERAL_ERROR);
	return (SUCCESS);
}

// A MODIFIER : Doit pouvoir prendre en compte plusieurs exports dans la meme commande
int	ft_export(t_minishell *minishell, int fd, char *str)
{
	t_env	*new;
	t_env	*old;

	if (!str)
		ft_env_print_export_no_param(minishell->head_env, fd);
	else if (!str[0])
		return (ft_error(minishell, GENERAL_ERROR, "export:",
				"`': not a valid identifier"));
	else
	{
		if (!ft_env_format_check(str))
		{

			new = ft_env_new(minishell, str);
			if (!new)
				return (ft_error(minishell, MALLOC_FAIL,
						"Error malloc with export", NULL));
			old = ft_env_find(minishell->head_env, new->name);
			if (!old)
			{
				if (ft_env_add(minishell, new) == GENERAL_ERROR)
					return (ft_error(minishell, GENERAL_ERROR,
					  "Error adding to export", NULL));
			}
			else
				old->content = new->content;
			return (SUCCESS);
		}
		return (GENERAL_ERROR);
	}
	return (SUCCESS);
}
