/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:34:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/20 11:36:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_token_word_count(t_token *current)
{
	int	count;

	count = 1;
	while (current)
	{
		if (current->type == PIPE)
			return (count);
		else if (current->type != GARBAGE)
			count++;
		current = current->next;
	}
	return (count);
}

int	ft_token_expands_1(t_minishell *minishell, t_token *next)
{
	if (!next || !next->str || !next->str[0])
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NL, NULL));
	ft_quotes_handle(minishell, next);
	return (SUCCESS);
}
int	ft_token_expands_2(t_minishell *minishell, t_token *token,
		t_token **token_ptr, t_cmd *cmd)
{
	ft_redirection_handler(minishell, cmd, token);
	if (minishell->exit_status != ERR_OPEN && minishell->exit_status != 0)
		return (GENERAL_ERROR);
	if (token->next)
		*token_ptr = token->next;
	return (SUCCESS);
}
int	ft_token_indchevron(t_minishell *minishell, t_token **token_ptr, t_cmd *cmd)
{
	t_token		*token;
	t_token		*next;

	token = *token_ptr;
	next = token->next;
	if (!next || !next->str)
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NL, NULL));
	if (token->next)
		token->next->type = WORD;
	ft_heredoc_handle(minishell, cmd, token);
	*token_ptr = next;
	return (SUCCESS);
}

int	ft_token_affect(t_minishell *minishell, t_cmd *cmd, t_token **token_ptr,
		int *i)
{
	t_token	*next;
	t_token	*token;
	int		res;

	token = *token_ptr;
	next = NULL;
	if (token->next)
		next = token->next;
	if (token->type == WORD || token->type == FLAG)
		cmd->args[(*i)++] = token->str;
	else if (token->type == CMD)
	{
		cmd->path = token->path;
		cmd->args[0] = token->str;
	}
	else if (token->type == IN_DCHEVRON)
	{
		// **token_ptr, *token_ptr, next = token->next;
		res = ft_token_indchevron(minishell, token_ptr, cmd);
		if (res != SUCCESS)
			return (res);
	}
	else
	{
		res = ft_token_expands_1(minishell, next);
		if (res != SUCCESS)
			return (res);
		res = ft_token_expands_2(minishell, token, token_ptr, cmd);
		if (res != SUCCESS)
			return (res);
	}
	return (SUCCESS);
}

// CREER UNE LISTE DE COMMANDE A PARTIR DES TOKENS
int	ft_cmd_lst_create(t_minishell *minishell)
{
	t_token	*tok_current;
	t_cmd	*cmd_new;
	int		i;
	int		count;

	tok_current = minishell->head_token;
	while (tok_current)
	{
		i = 1;
		count = ft_token_word_count(tok_current);
		cmd_new = ft_cmd_new(minishell);
		if (!cmd_new)
			return (ft_error(minishell, MALLOC_FAIL, ERRS_MALLOC_CMD, NULL));
		cmd_new->args = ft_calloc_gc(count + 1, sizeof(char *), &minishell->gc);
		if (!cmd_new->args)
			return (ft_error(minishell, MALLOC_FAIL, ERRS_MALLOC_CMD, NULL));
		while (tok_current && tok_current->type != PIPE)
		{
			if (ft_token_affect(minishell, cmd_new, &tok_current, &i))
				return (GENERAL_ERROR);
			if (tok_current)
				tok_current = tok_current->next;
		}
		cmd_new->args[i] = NULL;
		if (!cmd_new->path)
			cmd_new->path = cmd_new->args[0];
		if (!cmd_new->args[0] || !cmd_new->args[0][0])
		{
			if (tok_current)
				tok_current = tok_current->next;
			continue ;
		}
		ft_cmd_add(minishell, cmd_new);
		if (tok_current)
			tok_current = tok_current->next;
	}
	ft_cmd_add(minishell, NULL);
	return (SUCCESS);
}
