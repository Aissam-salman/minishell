/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_exec_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 21:39:04 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 22:25:23 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	child_set(t_child *child, int i, int prev_pipe, int size_cmd)
{
	child->prev_pipe = prev_pipe;
	child->index = i;
	child->size_cmd = size_cmd;
	child->pipe_fd[0] = -1;
	child->pipe_fd[1] = -1;
}

t_child	*ft_child_new(t_minishell *minishell, int size_cmd)
{
	t_child	*child;

	child = ft_gc_malloc(sizeof(t_child), &minishell->gc);
	if (!child)
		return (NULL);
	child->index = -1;
	child->prev_pipe = -1;
	child->size_cmd = size_cmd;
	return (child);
}
