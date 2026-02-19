/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/19 21:18:42 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env_format_check(char *str)
{
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (GENERAL_ERROR);
	return (SUCCESS);
}

int	ft_create_or_update_env(t_minishell *minishell, char *str)
{
	t_env	*new;
	t_env	*old;

	new = ft_env_new(minishell, str);
	if (!new)
		return (ft_error(minishell, MALLOC_FAIL, "Error malloc with export",
				NULL));
	old = ft_env_find(minishell->head_env, new->name);
	if (!old)
	{
		if (ft_env_add(minishell, new) == GENERAL_ERROR)
			return (ft_error(minishell, GENERAL_ERROR, "Error adding to export",
					NULL));
	}
	else
		old->content = new->content;
	return (SUCCESS);
}

int	ft_is_valid_key_env(char *str)
{
	int	i;

	i = 0;
	while (str[i] && str[i] != '=')
	{
		if (str[i] == '-')
			return (GENERAL_ERROR);
		i++;
	}
	return (SUCCESS);
}

int	ft_export(t_minishell *minishell, int fd, char **args)
{
	int	i;

	if (!args || !*args || !args[1])
		ft_env_print_export_no_param(minishell->head_env, fd);
	else if (!args[1][0])
		return (ft_error(minishell, GENERAL_ERROR, "export:",
				"`': not a valid identifier"));
	i = 1;
	while (args[i])
	{
		if (ft_is_valid_key_env(args[i]) == 1 || ft_is_only(args[i], ft_isdigit)
			|| (args[i][0] == '=' && !args[i][1]))
			return (ft_error(minishell, GENERAL_ERROR, ft_strjoin_gc("export: ",
						args[i], &minishell->gc), ": not a valid identifier"));
		else if (!ft_env_format_check(args[i]))
			return (ft_create_or_update_env(minishell, args[i]));
		i++;
	}
	return (SUCCESS);
}
