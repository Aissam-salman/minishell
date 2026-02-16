/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/16 12:26:10 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_env_format_check(char *str)
{
	ft_printf("format: %s\n", str);
	if (!ft_isalpha(str[0]) || str[0] == '-')
	{
		// A MODIFIER : TRAITER LES MESSAGES D'ERREUR
		ft_putstr_fd("First char not conform\n", STDERR_FILENO);
		return (GENERAL_ERROR);
	}
	return (SUCCESS);
}

int	ft_export(t_minishell *minishell, int fd, char *str)
{
	t_env	*new;
	// SI !STR  print env avec export devant chaque 
	// -> export KEY=foo
	// -> export KEY=boo
	if (!str)
		ft_env_print(minishell->head_env, fd);
	// SI STR == ""
	else if (!str[0])
	  return (ft_error(GENERAL_ERROR, "export:", "`': not a valid identifier"));
	else
	{
		// CHECKER SI STR A LE BON FORMAT
		if (!ft_env_format_check(str))
		{
			new = ft_env_new(minishell, str);
			if (!new)
				return (ft_error(MALLOC_FAIL, "Error malloc with export", NULL));
			if (ft_env_add(minishell, new) == GENERAL_ERROR)
				return (ft_error(GENERAL_ERROR, "Error adding to export", NULL));
		}
	}
	return (SUCCESS);
}
