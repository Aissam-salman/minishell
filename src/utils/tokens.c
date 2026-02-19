/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 14:14:33 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 10:02:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// RECUPERE LE DERNIER TOKEN
t_token	*ft_token_last(t_token *token)
{
	if (!token)
		return (NULL);
	while (token->next)
		token = token->next;
	return (token);
}

// CREATION DU TOKEN A PARTIR DU BUFFER
t_token	*ft_token_create(t_minishell *minishell, char *buffer)
{
	t_token	*new_token;

	new_token = ft_calloc_gc(1, sizeof(t_token), &minishell->gc);
	if (!new_token)
		return (NULL);
	new_token->str = ft_strdup_gc(buffer, &minishell->gc);
	if (!new_token->str)
		return (NULL);
	new_token->type = DEFAULT;
	new_token->next = NULL;
	ft_bzero(buffer, ft_strlen(buffer));
	return (new_token);
}

// AJOUT DU TOKEN A LA LISTE CHAINEE *head_token
int	ft_token_add(t_minishell *minishell, t_token *to_add)
{
	t_token	*last;

	if (!minishell || !to_add)
		return (GENERAL_ERROR);
	if (!minishell->head_token)
	{
		minishell->head_token = to_add;
		return (SUCCESS);
	}
	last = ft_token_last(minishell->head_token);
	last->next = to_add;
	return (SUCCESS);
}

int	ft_find_token (t_types type, t_token *head)
{
	while (head)
	{
		if (head->type == type)
			return (1);
		head = head->next;
	}
	return (0);
}