/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/21 19:29:23 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_expend_separator(t_minishell *minishell, char *usable_str, char *buffer)
{
	if (ft_buffer_add(usable_str, '$') != 0)
		return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
				NULL));
	if (ft_strlcat(usable_str, buffer, BUFFER_SIZE) > BUFFER_SIZE)
		return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
				NULL));
	return (SUCCESS);
}

int	ft_expend_error_val(t_minishell *minishell, char *usable_str, int *start)
{
	char	*err_value;

	err_value = ft_itoa_gc(minishell->cached_status, &minishell->gc);
	if (!err_value)
		return (ft_error(minishell, MALLOC_FAIL, "Error malloc expands", NULL));
	if (ft_strlcat(usable_str, err_value, BUFFER_SIZE) > BUFFER_SIZE)
		return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
				NULL));
	(*start)++;
	return (SUCCESS);
}

int	ft_expend(char *str, int *start, char *usable_str, t_minishell *minishell)
{
	char	buffer[BUFFER_SIZE];
	t_env	*path_env;

	ft_bzero(buffer, BUFFER_SIZE);
	if (ft_ischarset(str[*start + 1], SEPARATORS) || !str[*start + 1])
		return ((*start)++, ft_expend_separator(minishell, usable_str, buffer));
	(*start)++;
	if (str[*start] && str[*start] == '?')
		return (ft_expend_error_val(minishell, usable_str, start));
	while (str[*start] && !ft_ischarset(str[*start], SEPARATORS))
	{
		if (ft_buffer_add(buffer, str[*start]) != 0)
			return (BUFFER_FAIL);
		(*start)++;
	}
	path_env = ft_env_find(minishell->head_env, buffer);
	if (path_env && path_env->content)
	{
		if (ft_strlcat(usable_str, path_env->content,
				BUFFER_SIZE) > BUFFER_SIZE)
			return (ft_error(minishell, BUFFER_FAIL, "Insufficient buffer size",
					NULL));
	}
	return (SUCCESS);
}

void	ft_quotes_utils(t_token *token, t_minishell *minishell,
		char *usable_str)
{
	int	i;

	i = 0;
	while (token->str[i])
	{
		ft_state_detect(token->str[i], minishell);
		if (token->str[i] == '$' && minishell->state != IN_QUOTE)
		{
			if (ft_expend(token->str, &i, usable_str, minishell))
				return ;
			continue ;
		}
		else if ((token->str[i] != '\'' || minishell->state == IN_DQUOTE)
			&& (token->str[i] != '\"' || minishell->state == IN_QUOTE))
		{
			if (ft_buffer_add(usable_str, token->str[i]))
			{
				ft_error(NULL, 0, "Buffer too small", NULL);
				return ;
			}
		}
		i++;
	}
}

// FONCTION PRINCIPALE POUR GERE LES QUOTES
void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	char	usable_str[BUFFER_SIZE];

	minishell->state = NORMAL;
	ft_bzero(usable_str, BUFFER_SIZE);
	ft_quotes_utils(token, minishell, usable_str);
	token->str = ft_strdup_gc(usable_str, &minishell->gc);
	if (!token->str)
		ft_error(minishell, MALLOC_FAIL, "Error malloc end of quotes handling",
			NULL);
}
