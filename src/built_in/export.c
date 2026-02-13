/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/13 14:31:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env_format_check(char *str)
{
	if (!ft_isalpha(str[0]) || str[0] != '_')
	{
		// A MODIFIER : TRAITER LES MESSAGES D'ERREUR
		ft_printf("First char not conform");
		return (GENERAL_ERROR);
	}
	return (SUCCESS);
}

int	ft_export(t_minishell *minishell, int fd, char *str)
{
	t_env	*new;
	// SI !STR
	if (!str)
		ft_env_print(minishell->head_env, fd);
	// SI STR == ""
	else if (!str[0])
	return (ft_error(GENERAL_ERROR, "export:", "`': not a valid identifier\n"));
	else
	{
		// CHECKER SI STR A LE BON FORMAT
		if (!ft_env_format_check(str))
		{
			new = ft_env_new(minishell, str);
			if (!new)
				return (ft_error(MALLOC_FAIL, "Error malloc with export\n", NULL));
			if (ft_env_add(minishell, new) == GENERAL_ERROR)
				return (ft_error(GENERAL_ERROR, "Error adding to export\n", NULL));
		}
	}
	return (SUCCESS);
}