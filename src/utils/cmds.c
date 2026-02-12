/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:34:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 14:36:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_cmd	*ft_cmd_new(t_minishell *minishell)
{
	t_cmd	*new;

	new = ft_calloc_gc(1, sizeof(t_cmd), &minishell->gc);
	if (!new)
		return (NULL);
	new->next = NULL;
	new->infd = STDIN_FILENO;
	new->outfd = STDOUT_FILENO;
	return (new);
}

t_cmd	*ft_cmd_last(t_cmd *cmd_head)
{
	if (!cmd_head)
		return (NULL);
	while (cmd_head->next)
		cmd_head = cmd_head->next;
	return (cmd_head);
}

int   ft_cmd_size(t_cmd *cmd_head)
{
	int len;

	len = 0;
	while (cmd_head != NULL)
	{
		len++;
		cmd_head = cmd_head->next;
	}
	return (len);
}

int	ft_cmd_add(t_minishell *minishell, t_cmd *to_add)
{
	t_cmd	*last;

	if (!minishell || !to_add)
		return (1);
	if (!minishell->head_cmd)
	{
		minishell->head_cmd = to_add;
		return (0);
	}
	last = ft_cmd_last(minishell->head_cmd);
	last->next = to_add;
	return (0);
}

int	ft_token_word_count(t_token *current)
{
	int	count;

	// COMMENCE A 1 POUR CMD->ARGS[0] == PATH | CMD
	count = 1;
	while (current)
	{
		if (current->type == PIPE)
			return (count);
		else if (current->type == WORD || current->type == CMD)
			count++;
		current = current->next;
	}
	return (count);
}

void	ft_token_affect(t_cmd *cmd, t_token *token,
		int *i)
{
	// if (!minishell || !cmd || !token)
	// 	return ;
	// SI WORD = AJOUTE A ARGS
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
		ft_redirection_handler(cmd, token);
	}
	// SI OUTFILE => REMPLIR OUTFD
	// {
	// 	cmd->outfd = fopen(token->str,)
	// }
	// SI INFILE => REMPLIR INFD
	// else if (token->type == INFILE)
}

// CREER UNE LISTE DE COMMANDE A PARTIR DES TOKENS
void	ft_cmd_lst_create(t_minishell *minishell)
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
			ft_token_affect(cmd_new, tok_current, &i);

			// DOIT SAUTER LE NOEUD D'APRES
			tok_current = tok_current->next;
		}
		// SET LE DERNIER NOEUD A NULL
		cmd_new->args[i] = NULL;

		// SI PAS DE PATH
		if (!cmd_new->path)
			cmd_new->path = cmd_new->args[0];
		
		// ON AJOUTE LA STRUCT COMMAND A LA LISTE
		ft_cmd_add(minishell, cmd_new);

		// PASSER AU NOEUD SUIVANT TOKEN
		if (tok_current)
			tok_current = tok_current->next;
	}
	ft_cmd_add(minishell, NULL);
}
