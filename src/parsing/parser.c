/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/12 17:31:51 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	handle_redirection(t_token *token)
{
	if (ft_check_redirection(token->str) == 0)
		token->code_error = SYNTAX_ERROR;
	if (token->next)
	{
		if (ft_check_file_of_redirection(token) == 0)
			token->code_error = 299;
		// else
		// 	token->next->type = R_FILE;
	}
}

// void	handle_expands(t_token *token, t_minishell *minishell)
// {
// 	ft_filter_quote(token, minishell);
// 	token->str = ft_check_expands(minishell, token->str);
// }

void	handle_word(t_token *token, t_minishell *minishell, int cmd_find)
{
	// ft_filter_quote(token, minishell);
	if (ft_check_cmd(minishell, token) == 1 && cmd_find == 0)
	{
		token->type = CMD;
		cmd_find = 1;
	}
	if (ft_check_flags(token->str) == 1)
		token->type = FLAG;
	// if (ft_check_file(token) == 1)
	// {
	// 	ft_printf("AHAHHAHAHA %s\n", token->str);
	// 	token->type = R_FILE;
	// }
}

void	handle_pipe(t_token *token, int *cmd_find)
{
	if (!ft_check_pipe(token->str))
		token->code_error = 301;
	*cmd_find = 0;
}

void	checker_token(t_minishell *minishell)
{
	t_token	*token;
	int		cmd_find;

	cmd_find = 0;
	token = minishell->head_token;
	while (token)
	{
		ft_quotes_handle( minishell, token);
		if (is_redirection(token))
			handle_redirection(token);
		else if (token->type == WORD)
			handle_word(token, minishell, cmd_find);
		else if (token->type == PIPE)
			handle_pipe(token, &cmd_find);
		// ft_printf("TOKEN STR = %s\n", token->str);
		token = token->next;
	}
}
