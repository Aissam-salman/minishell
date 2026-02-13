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

#include "../../../includes/minishell.h"

static char	**ft_get_path(t_minishell *minishell)
{
	char	*env;
	char	**envp;

	env = getenv("PATH");
	if (!env)
		return (NULL);
	envp = ft_split_sep_gc(env, ':', &minishell->gc);
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
		cur_path = ft_strjoin_gc(tmp, token->str, &minishell->gc);
		if (stat(cur_path, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
		{
			if (cur_path && access(cur_path, X_OK) == 0)
			{
				token->path = cur_path;
				return (1);
			}
		}
		i++;
	}
	return (0);
}

void	ft_check_cmd(t_minishell *minishell, t_token *token)
{
	char		**envp;
	int			res;
	struct stat	stat_file;

	if (!token || !token->str || !*token->str)
		return;
	if (token->str[0] == '/' && !token->str[1])
		return;
	if (stat(token->str, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
	{
		if (access(token->str, X_OK) == 0)
		{
			token->path = token->str;
			return;
		}
	}
	envp = ft_get_path(minishell);
	if (!envp)
		return;
	res = ft_test_path(minishell, envp, token);
	if (res == 1)
		return;
}

int	ft_check_pipe(char *str)
{
	if (!str)
		return (0);
	if (str[0] == '|' && !str[1])
		return (1);
	return (0);
}

int	is_redirection(t_token *token)
{
	if (token->type == IN_CHEVRON || token->type == OUT_CHEVRON
		|| token->type == IN_DCHEVRON || token->type == OUT_DCHEVRON)
		return (1);
	return (0);
}
