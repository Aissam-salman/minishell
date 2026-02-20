/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 21:07:51 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 21:12:12 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_pwd(t_env **head_env, t_minishell *minishell)
{
	char	*buff;
	t_env	*env_pwd;

	buff = getcwd(NULL, 0);
	if (buff)
	{
		env_pwd = ft_env_find(*head_env, "PWD");
		if (!env_pwd)
		{
			free(buff);
			ft_error(minishell, errno, ERRS_PWD_NOTFOUND, NULL);
			return ;
		}
		env_pwd->content = ft_strdup_gc(buff, &minishell->gc);
		free(buff);
		return ;
	}
	ft_error(minishell, errno, "pwd", NULL);
}

void	update_old_pwd(t_env **head_env, char *old_pwd,
		t_minishell *minishell)
{
	t_env	*env_old_pwd;

	env_old_pwd = ft_env_find(*head_env, "OLDPWD");
	if (!env_old_pwd)
	{
		env_old_pwd = ft_env_new(minishell, "OLDPWD");
		if (!env_old_pwd)
			return ;
		ft_env_add(minishell, env_old_pwd);
	}
	env_old_pwd->content = ft_strdup_gc(old_pwd, &minishell->gc);
}

char	*save_pwd(void)
{
	char	*opwd;

	opwd = getcwd(NULL, 0);
	if (!opwd)
	{
		ft_error(NULL, errno, "pwd", NULL);
		return (NULL);
	}
	return (opwd);
}

char	*ft_no_path(t_env *head_env)
{
	t_env	*tmp;

	tmp = ft_env_find(head_env, "HOME");
	if (!tmp)
		return (NULL);
	return (tmp->content);
}
