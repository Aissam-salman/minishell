/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:49:15 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/17 12:16:55 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void update_pwd(t_env **head_env, t_minishell *minishell)
{
	char *buff;
	t_env *env_pwd;

	// auto par la function
	buff = getcwd(NULL, 0);
	if (buff)
	{
		env_pwd = ft_env_find(*head_env, "PWD");
		if (!env_pwd)
		{
			free(buff);
			ft_error(minishell, errno,"PWD not found",NULL);
			return;
		}
		env_pwd->content = ft_strdup_gc(buff, &minishell->gc);
		free(buff);
		return;
	}
	ft_error(minishell, errno,"pwd",NULL);
}

static void update_old_pwd(t_env **head_env, char *old_pwd, t_minishell *minishell)
{
	t_env *env_old_pwd;

	env_old_pwd = ft_env_find(*head_env, "OLDPWD");
	if (!env_old_pwd)
		env_old_pwd = ft_env_new(minishell, "OLDPWD");
	env_old_pwd->content = ft_strdup_gc(old_pwd, &minishell->gc);
}

static char  *save_pwd()
{
	char *opwd;

	opwd = getcwd(NULL, 0);
	if (!opwd)
	{
		ft_error(NULL, errno,"pwd",NULL);
		return (NULL);
	}
	return (opwd);
}

void ft_cd(t_minishell *minishell, char *path)
{
	char *old_pwd;
	t_env *tmp;

	old_pwd = save_pwd();
	if (!old_pwd)
		return;
	//stocker l'ancien path et le mettre dans OLD_PWD si chdir reussi
	// env OLD_PWD a mettre a jours 
	if (!path || !*path || ft_strcmp(path, "~") == 0)
	{
		tmp =ft_env_find(minishell->head_env, "HOME"); 
		if (!tmp)
			return;
		path = tmp->content;
	}
	// si old_pwd go to pwd, et pwd 
	// sinon juste pwd
	if (ft_strcmp(path, "-") == 0)
	{
		tmp = ft_env_find(minishell->head_env, "OLDPWD");
		if (tmp)
		{
			if (chdir(tmp->content) == -1)
			{
				if (old_pwd)
					free(old_pwd);
				ft_error(minishell, errno,"cd", NULL);
				return ;
			}
			update_old_pwd(&minishell->head_env, old_pwd, minishell);
			update_pwd(&minishell->head_env, minishell);
		}
		free(old_pwd);
		ft_pwd();
		return;
	}
	if (chdir(path) == -1)
	{
		if (old_pwd)
			free(old_pwd);
		ft_error(minishell, errno,"cd", NULL);
		return ;
	}
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	free(old_pwd);
	update_pwd(&minishell->head_env, minishell);
}

