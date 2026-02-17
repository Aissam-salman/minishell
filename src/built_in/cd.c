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
#include "errors.h"
#include "includes/ft_gc.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

static void	update_pwd(t_env **head_env, t_minishell *minishell)
{
	char	*buff;
	t_env	*env_pwd;

	// auto par la function
	buff = getcwd(NULL, 0);
	if (buff)
	{
		env_pwd = ft_env_find(*head_env, "PWD");
		if (!env_pwd)
		{
			free(buff);
			ft_error(minishell, errno, "PWD not found", NULL);
			return ;
		}
		env_pwd->content = ft_strdup_gc(buff, &minishell->gc);
		free(buff);
		return ;
	}
	ft_error(minishell, errno, "pwd", NULL);
}

static void	update_old_pwd(t_env **head_env, char *old_pwd,
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

static char	*save_pwd(void)
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

int	ft_cd_back(t_minishell *minishell, char *old_pwd)
{
	t_env	*tmp;

	tmp = ft_env_find(minishell->head_env, "OLDPWD");
	if (!tmp)
		return (GENERAL_ERROR);
	if (chdir(tmp->content) == -1)
	{
		free(old_pwd);
		ft_error(minishell, errno, "cd", NULL);
		return (errno);
	}
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	update_pwd(&minishell->head_env, minishell);
	free(old_pwd);
	ft_pwd();
	return (SUCCESS);
}

char	*ft_expand_home(t_minishell *minishell, char *path)
{
	t_env	*tmp;
	char	*new_path;

	if (!path)
		return (NULL);
	if (path[0] == '~')
	{
		tmp = ft_env_find(minishell->head_env, "HOME");
		if (!tmp)
			return (NULL);
		new_path = ft_strjoin_gc(tmp->content, "/", &minishell->gc);
		if (!new_path)
			return (NULL);
		new_path = ft_strjoin_gc(new_path, path + 1, &minishell->gc);
		if (!new_path)
			return (NULL);
		return (new_path);
	}
	return (NULL);
}

int	ft_cd(t_minishell *minishell, char *path)
{
	char	*old_pwd;
	struct stat	stat_file;

	old_pwd = save_pwd();
	if (!old_pwd)
		return (GENERAL_ERROR);
	if (!path || !*path || ft_strcmp(path, "~") == 0)
		path = ft_no_path(minishell->head_env);
	else if (ft_strcmp(path, "-") == 0)
		return (ft_cd_back(minishell, old_pwd));
	if (ft_strchr(path, '~'))
		path = ft_expand_home(minishell, path);
	if ((stat(path, &stat_file) == 0 && S_ISDIR(stat_file.st_mode)) && chdir(path) == -1)
		return (ft_error(minishell, errno, "cd", NULL));
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	free(old_pwd);
	update_pwd(&minishell->head_env, minishell);
	return (SUCCESS);
}
