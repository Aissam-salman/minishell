/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/11 12:15:32 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void checker_token(t_minishell *minishell)
{
	t_token *token;
	int		cmd_find = 0;

	token = minishell->head_token;
	while (token)
	{
		if (is_redirection(token))
		{
			if (ft_check_redirection(token->str) == 0)
				token->code_error = SYNTAX_ERROR;
			if (token->next)
			{
				if (ft_check_file_of_redirection(token) == 0)
					token->code_error = 299;
				else
					token->next->type = R_FILE;
			}
		}
		else if ((token->type == WORD && (ft_strchr(token->str, '$') &&  token->str[0] == '\"')) || (ft_strchr(token->str, '$') && token->str[0] != '\''))
		{
			ft_filter_quote(token, minishell);
			token->str = ft_check_expands(minishell, token->str);
		}
		else if (token->type == WORD)
		{
			ft_filter_quote(token, minishell);
			if (ft_check_cmd(minishell, token) == 1 && cmd_find == 0)
			{
				token->type = CMD;
				cmd_find = 1;
			}
			if (ft_check_flags(token->str) == 1)
				token->type = FLAG;
			if (ft_check_file(token) == 1)
				token->type = R_FILE;
		}
		else if (token->type == PIPE)
		{
			if (!ft_check_pipe(token->str))
					token->code_error = 301;
			cmd_find = 0;
		}
		token = token->next;
	}
}
