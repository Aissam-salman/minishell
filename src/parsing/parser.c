/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 18:06:37 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "errors.h"

int	handle_redirection(t_token *token)
{
	if (ft_check_redirection(token->str) == 0)
		return (ft_error(ERR_SYNTAX, ERRS_SYNT_NEAR, token->str));
	if (!token->next)
		return (ft_error(ERR_SYNTAX, ERRS_SYNT_NEAR, "`newline'"));
	if (is_redirection(token->next) || token->next->type == PIPE)
		return (ft_error(ERR_SYNTAX, ERRS_SYNT_NEAR, token->next->str));
	return (SUCCESS);
}

void	handle_word(t_token *token, t_minishell *minishell, int *cmd_find)
{
	if (*cmd_find == 0)
	{
		ft_check_cmd(minishell, token);
		token->type = CMD;
		*cmd_find = 1;
	}
	else if (ft_check_flags(token->str) == 1)
		token->type = FLAG;
}

int	handle_pipe(t_token *token, int *cmd_find)
{
	if (!ft_check_pipe(token->str))
		return (ERR_HANDLE_PIPE);
	*cmd_find = 0;
	return (SUCCESS);
}

int	checker_token(t_minishell *minishell)
{
	t_token	*token;
	int		cmd_find;

	cmd_find = 0;
	token = minishell->head_token;
	while (token)
	{
		ft_quotes_handle( minishell, token);
		if (is_redirection(token))
		{
			if (handle_redirection(token))
				return (ERR_SYNTAX);
			if (token->next)
				token = token->next;
		}
		else if (token->type == WORD)
			handle_word(token, minishell, &cmd_find);
		else if (token->type == PIPE)
			if (handle_pipe(token, &cmd_find))
				return (ERR_SYNTAX);
		token = token->next;
	}
	return (SUCCESS);
}
