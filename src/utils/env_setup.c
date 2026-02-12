/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 19:32:38 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/12 20:23:32 by fardeau          ###   ########.fr       */
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

t_env *ft_env_new(t_minishell *minishell, char *str)
{
	t_env	*new;
	char	*string_trunc;

	new = ft_calloc_gc(1, sizeof(t_env), &minishell->gc);
	if (!new)
	{
		ft_error(MALLOC_FAIL, "Error malloc setup env");
		return (NULL);
	}
	// string_trunc = RECUPERER JUSQU'A =
	string_trunc = ft_strchr(str, '=');
	if (string_trunc)
	{
		// AFFECTER new_name A *name;
		new->name = ft_substr_gc(str, 0, string_trunc - str, &minishell->gc);
		// AFFECTER new_contet A *content;
		new->content = ft_strdup_gc(string_trunc + 1, &minishell->gc);
		if (!new->name || !new->content)
		{
			ft_error(MALLOC_FAIL, "Error malloc setup env");
			return (NULL);
		}
	}
	// SINON TOUT RENTRER DANS NAME
	else
	{
		new->name = ft_strdup_gc(str, &minishell->gc);
		new->content = NULL;
		if (!new->name)
		{
			ft_error(MALLOC_FAIL, "Error malloc setup env");
			return (NULL);
		}
	}
	new->next = NULL;
	return (new);
}

int	ft_env_add(t_minishell *minishell, t_env *new)
{
	t_env *last;

	if (!minishell || !new)
		return (GENERAL_ERROR);
	if (!minishell->head_env)
	{
		minishell->head_env = new;
		return (SUCCESS);
	}
	last = ft_env_last(minishell->head_env);
	last->next = new;
	return (SUCCESS);
}

t_env	*ft_env_find(t_env	*head_env, char *to_find)
{
	while (head_env)
	{
		if (!ft_strncmp(head_env->name, to_find, ft_strlen(to_find)))
			return (head_env);
		head_env = head_env->next;
	}
	return (NULL);
}

void	ft_env_setup(t_minishell *minishell, char **envp)
{
	int i;
	t_env	*new;

	i = 0;
	// CLASSICO CLASSIQUE BOUCLE D'INITIALISATION DE LISTE CHAINEE
	while (envp[i])
	{
		new = ft_env_new(minishell, envp[i]);
		if (!new)
			ft_exit(minishell, GENERAL_ERROR, "Error setting up env");
		if (ft_env_add(minishell, new))
			ft_exit(minishell, GENERAL_ERROR, "Error setting up env");
		i++;
	}
	// ft_env_print(minishell->head_env);
}