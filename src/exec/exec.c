/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 22:29:13 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parent_process(int *prev_pipe, int pipe_fd[2])
{
	signal(SIGINT, SIG_IGN);
	close(pipe_fd[1]);
	if (*prev_pipe != -1)
		close(*prev_pipe);
	*prev_pipe = pipe_fd[0];
}

void	ft_exec_only_built_in(t_minishell *minishell, t_cmd *cmd)
{
	int	saved_stdout;

	saved_stdout = dup(STDOUT_FILENO);
	if (cmd->outfd != STDOUT_FILENO)
		dup2(cmd->outfd, STDOUT_FILENO);
	run_built_in(cmd, minishell);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
}

void	ft_ex(t_minishell *minishell, int size_cmd, int pipe_fd[2], int *pids)
{
	t_cmd	*cmd;
	t_child	*child;

	child = ft_child_new(minishell, size_cmd);
	cmd = minishell->head_cmd;
	while (++child->index < size_cmd)
	{
		if (is_built_in(cmd) && size_cmd == 1)
			ft_exec_only_built_in(minishell, cmd);
		else if (pipe(pipe_fd) != -1)
		{
			pids[child->index] = fork();
			if (pids[child->index] < 0)
				ft_exit(minishell, errno, strerror(errno));
			if (pids[child->index] == 0)
				child_process(minishell, cmd, child, pipe_fd);
			parent_process(&child->prev_pipe, pipe_fd);
		}
		cmd = cmd->next;
	}
	if (child->prev_pipe != -1)
		close(child->prev_pipe);
}

void	ft_exec(t_minishell *minishell)
{
	int	*pids;
	int	pipe_fd[2];
	int	size_cmd;

	size_cmd = ft_cmd_size(minishell->head_cmd);
	if (!size_cmd)
		return ;
	pids = ft_gc_malloc(sizeof(int) * size_cmd, &minishell->gc);
	if (!pids)
		return ;
	ft_ex(minishell, size_cmd, pipe_fd, pids);
	if (is_built_in(minishell->head_cmd) && size_cmd == 1)
		return ;
	ft_wait_subprocess(minishell, size_cmd, pids);
}
