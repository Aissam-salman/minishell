/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:34:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/09 18:30:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

t_cmd	*ft_cmd_new(t_minishell *minishell)
{
	t_cmd *new;

	new = ft_calloc_gc(1, sizeof(t_cmd), &minishell->gc);
	if (!new)
		return (NULL);
	new->next = NULL;
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

int	ft_cmd_add(t_minishell *minishell, t_cmd *to_add)
{
	t_cmd *last;

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
	int count;

	count = 0;
	while (current)
	{
		if (current->type == PIPE)
			return (count);
		else if (current->type == WORD)
			count++;
		current = current->next;	
	}
	return (count);
}

// CREER UNE LISTE A PARTIR DES TOKENS
void ft_cmd_lst_create(t_minishell *minishell)
{
	t_token *current;
	t_cmd	*new;
	int i;
	int count;

	current = minishell->head_token;
	while (current)
	{
		i = 0;
		new = ft_cmd_new(minishell);
		count = ft_token_word_count(current);
		new->args = ft_calloc_gc(count + 1, sizeof(char *), &minishell->gc);
		while (current && current->type != PIPE)
		{
			if (current->type == WORD)
			{
				new->args[i] = current->str;
				i++;
			}
			current = current->next;
		}
		new->args[i] = NULL;
		ft_cmd_add(minishell, new);
		ft_cmd_print(new);
		if (current)
			current = current->next;
	}
}

