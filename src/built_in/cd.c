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

#include "errors.h"
#include "minishell.h"
#include "utils.h"
#include <asm-generic/errno-base.h>

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

char *ft_define_path(t_minishell *minishell, char **args)
{
	char *path;

	if (!args[1]  || ft_strcmp(args[1], "~") == 0)
		path = ft_no_path(minishell->head_env);
	else if (ft_strchr(args[1], '~') && args[1][1])
		path = ft_expand_home(minishell, args[1]);
	else
		path = args[1];
	return (path);
}

int ft_try_cd(char *path, t_minishell *minishell)
{
	struct stat	stat_file;

	if (stat(path, &stat_file) != 0)
		return (ft_error(minishell, GENERAL_ERROR, "cd: ", "No such file or directory"));
	if (!S_ISDIR(stat_file.st_mode))
		return (ft_error(minishell, ENOTDIR, "cd: ", ft_strjoin_gc(path, ": Not a directory", &minishell->gc)));
	if (chdir(path) == -1)
		return (ft_error(minishell, errno, "cd", path));
	return (SUCCESS);
}

int		ft_cd(t_minishell *minishell, char **args)
{
	char	*old_pwd;
	char *path;
	int res;

	if (args[1] && args[2])
		return (ft_error(minishell, GENERAL_ERROR, "cd: ", "too many arguments"));
	if (ft_strcmp(args[1], "-") == 0)
	{
		old_pwd = save_pwd();
		return (ft_cd_back(minishell, old_pwd));
	}
	else
		path = ft_define_path(minishell, args);
	res = ft_try_cd(path, minishell);
	if (res != SUCCESS)
		return (res);
	old_pwd = save_pwd();
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	free(old_pwd);
	update_pwd(&minishell->head_env, minishell);
	return (SUCCESS);
}
