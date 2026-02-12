/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/12 20:55:04 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env_format_check(char *str)
{
	if (!ft_isalnum(str[0]))
	{
		// A MODIFIER : TRAITER LES MESSAGES D'ERREUR
		ft_printf("FIRST CHAR NOT ALNUM or _\n");
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
		ft_putstr_fd("minishell: export: `': not a valid identifier\n", 2);
	else
	{
		// CHECKER SI STR A LE BON FORMAT
		if (!ft_env_format_check(str))
		{
			new = ft_env_new(minishell, str);
			if (!new)
				return (ft_error(MALLOC_FAIL, "Error malloc with export\n"));
			if (ft_env_add(minishell, new) == GENERAL_ERROR)
				return (ft_error(GENERAL_ERROR, "Error adding to export\n"));
		}
	}
	return (SUCCESS);
}