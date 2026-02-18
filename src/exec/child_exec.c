/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:31:47 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/18 08:54:04 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <string.h>

void	child_set(t_child *child, int i, int prev_pipe, int size_cmd)
{
	child->prev_pipe = prev_pipe;
	child->index = i;
	child->size_cmd = size_cmd;
}

t_child	*ft_child_new(t_minishell *minishell)
{
	t_child	*child;

	child = ft_gc_malloc(sizeof(t_child), &minishell->gc);
	if (!child)
		return (NULL);
	child->index = 0;
	child->prev_pipe = 0;
	child->size_cmd = 0;
	return (child);
}

void	close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2])
{
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (is_built_in(cmd) == 1)
	{
		run_built_in(cmd, minishell);
		ft_gc_free_all(&minishell->gc);
		rl_clear_history();
		_exit(0);
	}
	if (!cmd->path || access(cmd->path, X_OK) != 0)
	{
		ft_error(minishell, CMD_NOT_FOUND, cmd->args[0], ": command not found");
		ft_gc_free_all(&minishell->gc);
		rl_clear_history();
		_exit(CMD_NOT_FOUND);
	}
	if (execv(cmd->path, cmd->args) == -1)
		ft_exit(minishell, errno, strerror(errno));
}

void	child_process(t_minishell *minishell, t_cmd *cmd, t_child *child,
		int pipe_fd[2])
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
