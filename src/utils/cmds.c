/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:34:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/20 10:28:27 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_token_word_count(t_token *current)
{
	int	count;

	// COMMENCE A 1 POUR CMD->ARGS[0] == PATH | CMD
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

int	ft_token_affect(t_minishell *minishell, t_cmd *cmd, t_token **token_ptr,
		int *i)
{
	t_token	*next;
	t_token	*token;

	token = *token_ptr;
	// SI WORD = AJOUTE A ARGS
	next = NULL;
	if (token->next)
		next = token->next;
	if (token->type == WORD || token->type == FLAG)
		cmd->args[(*i)++] = token->str;

	// SI CMD => REMPLIR PATH ET ARGV[0]
	else if (token->type == CMD)
	{
		cmd->path = token->path;
		cmd->args[0] = token->str;
	}
	else if (token->type == OUT_CHEVRON || token->type == OUT_DCHEVRON
		|| token->type == IN_CHEVRON)
	{
		if (!next || !next->str || !next->str[0])
			return (ft_error(minishell, ERR_SYNTAX,
					"Syntax error near unexpected token 'newline'", NULL));
		ft_quotes_handle(minishell, next);
		// A MODIFIER : VALEUR DE RETOUR
		ft_redirection_handler(minishell, cmd, token);
		if (minishell->exit_status != ERR_OPEN && minishell->exit_status != 0)
			return (GENERAL_ERROR);
		if (token->next)
			*token_ptr = token->next;
	}
	// GESTION DES HERE_DOC
	else if (token->type == IN_DCHEVRON)
	{
		if (!next || !next->str)
			return (ft_error(minishell, ERR_SYNTAX,
					"Syntax error near unexpected token 'newline'", NULL));
		else
		{
			if (token->next)
				token->next->type = WORD;
			ft_heredoc_handle(minishell, cmd, token);
			*token_ptr = next;
		}
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

	// COMMENCE A PARCOURIR LA CHAINE DE TOKENS
	tok_current = minishell->head_token;
	while (tok_current)
	{
		i = 1;
		// INITIALISE LE NOUVEAU NOEUD
		cmd_new = ft_cmd_new(minishell);
		count = ft_token_word_count(tok_current);
		// ALLOUE LE NOMBRE DE WORDS
		cmd_new->args = ft_calloc_gc(count + 1, sizeof(char *), &minishell->gc);
		while (tok_current && tok_current->type != PIPE)
		{
			// AFFEECT LES DIFFERENTES PARTIES DE CMD A CHAQUE TOKEN
			if (ft_token_affect(minishell, cmd_new, &tok_current, &i))
				return (GENERAL_ERROR);

			// DOIT SAUTER LE NOEUD D'APRES
			// if (tok_current) already moved in ft_token_affect if redirs used
			if (tok_current)
				tok_current = tok_current->next;
		}
		// SET LE DERNIER NOEUD A NULL
		cmd_new->args[i] = NULL;
		// SI PAS DE PATH
		if (!cmd_new->path)
			cmd_new->path = cmd_new->args[0];
		if (!cmd_new->args[0] || !cmd_new->args[0][0])
		{
			if (tok_current)
				tok_current = tok_current->next;
			continue;

		}
		// ON AJOUTE LA STRUCT COMMAND A LA LISTE
		ft_cmd_add(minishell, cmd_new);
		// WARN: no use the return value
		// PASSER AU NOEUD SUIVANT TOKEN
		if (tok_current)
			tok_current = tok_current->next;
	}
	ft_cmd_add(minishell, NULL);
	return (SUCCESS);
}
