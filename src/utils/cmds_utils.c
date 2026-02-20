/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:27:48 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/20 10:30:37 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*ft_cmd_new(t_minishell *minishell)
{
	t_cmd	*new;

	new = ft_calloc_gc(1, sizeof(t_cmd), &minishell->gc);
	if (!new)
		return (NULL);
	new->args = NULL;
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

int	ft_cmd_size(t_cmd *cmd_head)
{
	int	len;

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
		return (GENERAL_ERROR);
	if (!minishell->head_cmd)
	{
		minishell->head_cmd = to_add;
		return (SUCCESS);
	}
	last = ft_cmd_last(minishell->head_cmd);
	last->next = to_add;
	return (SUCCESS);
}
