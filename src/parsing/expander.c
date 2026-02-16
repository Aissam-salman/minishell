/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:04:48 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/11 12:05:52 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// TODO: refacto
char	*ft_check_expands(t_minishell *minishell, char *str)
{
	int		start;
	int		end;
	char	*var_env;
	char	*test_env;
	int		quote_mode;

	start = 0;
	if (str[0] == '$' && !str[1])
		return (ft_strdup_gc("$", &minishell->gc));
	while (str[start] && str[start] != '$')
		start++;
	if (str[start] == '$')
		start++;
	while (str[start] == '\'' || str[start] == '\"')
	{
		quote_mode = 1;
		start++;
	}
	end = start;
	while (str[end] && ft_isalnum(str[end]))
		end++;
	end++;
	var_env = ft_substr_gc(str, start, ft_strlen(str) - start + 1,
			&minishell->gc);
	if (!var_env)
		return (ft_strdup_gc("", &minishell->gc));
	if (quote_mode == 1)
		return (var_env);
	test_env = getenv(var_env);
	if (!test_env)
		return (ft_strdup_gc("", &minishell->gc));
	return (test_env);
}
