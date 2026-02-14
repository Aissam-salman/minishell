/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:31:47 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/14 14:39:17 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void child_set(t_child *child, int i, int prev_pipe, int size_cmd)
{
	child->prev_pipe = prev_pipe;
	child->index = i;
	child->size_cmd = size_cmd;
}

t_child *ft_child_new(t_minishell *minishell)
{
	t_child *child;

	child = ft_gc_malloc(sizeof(struct s_child), &minishell->gc);
	if (!child)
		return (NULL);
	child->index = 0;
	child->prev_pipe = 0;
	child->size_cmd = 0;
	return (child);
}

void close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2])
{

	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (is_built_in(cmd) == 1)
	{
		run_built_in_piped(cmd, minishell);
		exit(1);
	}
	else
		if (execv(cmd->path, cmd->args) == -1)
			ft_exit(minishell, errno, "EXECV");
}

void child_process(t_minishell *minishell, t_cmd *cmd, t_child *child, int pipe_fd[2])
{
	handler_signal_child();
	if (child->index == 0)
		handler_first_cmd(cmd->infd, child->size_cmd, pipe_fd[1]);
	else if (child->index == child->size_cmd - 1)
		handler_last_cmd(child->prev_pipe, cmd->outfd);
	else
		handler_mid_cmd(child->prev_pipe, pipe_fd[1]);
	close_pipe_and_exec(cmd, minishell, pipe_fd);
}
