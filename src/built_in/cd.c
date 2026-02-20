/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:49:15 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/20 18:32:32 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	ft_pwd(minishell);
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

char	*ft_define_path(t_minishell *minishell, char **args)
{
	char	*path;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		path = ft_no_path(minishell->head_env);
	else if (ft_strchr(args[1], '~') && args[1][1])
		path = ft_expand_home(minishell, args[1]);
	else
		path = args[1];
	return (path);
}

static int	ft_try_cd(char *path, t_minishell *minishell)
{
	struct stat	stat_file;

	if (stat(path, &stat_file) != 0 )
		return (ft_error(minishell, GENERAL_ERROR, strerror(errno), NULL));
	if (!S_ISDIR(stat_file.st_mode))
		return (ft_error(minishell, ENOTDIR, "cd: ", ft_strjoin_gc(path,
					": Not a directory", &minishell->gc)));
	if (chdir(path) == -1)
		return (ft_error(minishell, errno, strerror(errno), NULL));
	return (SUCCESS);
}

int	ft_cd(t_minishell *minishell, char **args)
{
	char	*old_pwd;
	char	*path;
	int		res;

	if (args[2])
		return (ft_error(minishell, GENERAL_ERROR, "cd: ",
				"too many arguments"));
	if (args[1] && ft_strcmp(args[1], "-") == 0)
	{
		old_pwd = save_pwd();
		return (ft_cd_back(minishell, old_pwd));
	}
	else
		path = ft_define_path(minishell, args);
	if (!path)
		return (ft_error(minishell, GENERAL_ERROR, "cd: ", "HOME not set"));
	old_pwd = save_pwd();
	res = ft_try_cd(path, minishell);
	if (res != SUCCESS)
		return (res);
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	free(old_pwd);
	update_pwd(&minishell->head_env, minishell);
	return (SUCCESS);
}

// ON LANCE MINISHELL
// ON CREE CURRENT ET OLD A LA POSITION ACTUELLE (dans env_setup)
// SI args[2]
	// TROP D'ARGUMENTS
// SI [cd] ou [cd ~]
	// OLD = CURRENT
	// CURRENT = HOME
// SINON SI [cd -]
	// OLD = CURRENT
	// CURRENT = OLD
// SINON SI [cd *str]
	// OLD = CURRENT
	// CURRENT = str
// ft_try_cd(CURRENT->str, minishell)
