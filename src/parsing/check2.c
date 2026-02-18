/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:00:44 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/12 17:42:53 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

static char	**ft_get_path(t_minishell *minishell)
{
	t_env	*env;
	char	**envp;

	env = ft_env_find(minishell->head_env, "PATH");
	if (!env)
		return (NULL);
	envp = ft_split_sep_gc(env->content, ':', &minishell->gc);
	if (!envp)
		return (NULL);
	return (envp);
}

static int	ft_test_path(t_minishell *minishell, char **envp, t_token *token)
{
	char		*tmp;
	char		*cur_path;
	int			i;
	struct stat	stat_file;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strjoin_gc(envp[i], "/", &minishell->gc);
		if (!tmp)
			return (GENERAL_ERROR);
		cur_path = ft_strjoin_gc(tmp, token->str, &minishell->gc);
		if (!cur_path)
			return (GENERAL_ERROR);
		if (stat(cur_path, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
		{
			if (access(cur_path, X_OK) == 0)
			{
				token->path = cur_path;
				return (SUCCESS);
			}
		}
		i++;
	}
	return (GENERAL_ERROR);
}

int	ft_cmd_find_path(t_minishell *minishell, t_token *token)
{
	char		**envp;
	struct stat	stat_file;

	if (!token || !token->str || !*token->str)
		return (GENERAL_ERROR);
	if (token->str[0] == '/' && !token->str[1])
		return (GENERAL_ERROR);
	if (stat(token->str, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
	{
		if (access(token->str, X_OK) == 0)
		{
			token->path = token->str;
			// FIX: return 126
			return (SUCCESS);
		}
		else
			return (GENERAL_ERROR);
	}
	envp = ft_get_path(minishell);
	if (!envp)
		return (GENERAL_ERROR);
	return (ft_test_path(minishell, envp, token));
}

int	ft_check_pipe(char *str)
{
	return (str && str[0] == '|' && !str[1]);
}

int	is_redirection(t_token *token)
{
	if (token->type == IN_CHEVRON || token->type == OUT_CHEVRON
		|| token->type == IN_DCHEVRON || token->type == OUT_DCHEVRON)
		return (1);
	return (0);
}
