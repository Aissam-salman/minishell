/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 19:32:38 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/20 18:36:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_env	*ft_env_last(t_env *head_env)
{
	if (!head_env)
		return (NULL);
	while (head_env->next)
		head_env = head_env->next;
	return (head_env);
}

t_env	*ft_env_new(t_minishell *minishell, char *str)
{
	t_env	*new;
	char	*string_trunc;

	new = ft_calloc_gc(1, sizeof(t_env), &minishell->gc);
	if (!new)
	{
		ft_error(NULL, MALLOC_FAIL, "Error malloc setup env", NULL);
		return (NULL);
	}
	string_trunc = ft_strchr(str, '=');
	if (string_trunc)
	{
		new->name = ft_substr_gc(str, 0, string_trunc - str, &minishell->gc);
		new->content = ft_strdup_gc(string_trunc + 1, &minishell->gc);
		if (!new->name || !new->content)
		{
			ft_error(NULL, MALLOC_FAIL, "Error malloc setup env", NULL);
			return (NULL);
		}
	}
	else
	{
		new->name = ft_strdup_gc(str, &minishell->gc);
		new->content = NULL;
		if (!new->name)
		{
			ft_error(minishell, MALLOC_FAIL, "Error malloc setup env", NULL);
			return (NULL);
		}
	}
	new->next = NULL;
	return (new);
}

// AJOUTE LE NOEUD ENV A LA LISTE
int	ft_env_add(t_minishell *minishell, t_env *new_env)
{
	t_env	*last;

	if (!minishell || !new_env)
		return (GENERAL_ERROR);
	if (!minishell->head_env)
	{
		minishell->head_env = new_env;
		return (SUCCESS);
	}
	last = ft_env_last(minishell->head_env);
	last->next = new_env;
	return (SUCCESS);
}

// TROUVE UN ENV A PARTIR DU NOM
t_env	*ft_env_find(t_env *head_env, char *to_find)
{
	while (head_env)
	{
		if (ft_strcmp(head_env->name, to_find) == 0)
			return (head_env);
		head_env = head_env->next;
	}
	return (NULL);
}

int	ft_env_setup(t_minishell *minishell, char **envp)
{
	int		i;
	t_env	*new;

	i = 0;
	if (!envp || !*envp)
	{
		ft_env_add(minishell, ft_env_new(minishell, "_=/usr/bin/env"));
		ft_env_add(minishell, ft_env_new(minishell, ft_strjoin_free("PWD=", getcwd (NULL, 0), SECOND)));
		return (SUCCESS);
	}
	while (envp[i])
	{
		new = ft_env_new(minishell, envp[i]);
		if (!new)
			ft_exit(minishell, GENERAL_ERROR, "Error setting up env");
		if (ft_env_add(minishell, new))
			ft_exit(minishell, GENERAL_ERROR, "Error setting up env");
		i++;
	}
	return (SUCCESS);
}
