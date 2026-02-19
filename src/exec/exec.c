/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 12:37:54 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_ins.h"
#include "minishell.h"
#include <string.h>

void	parent_process(int *prev_pipe, int pipe_fd[2])
{
	// disable signal ctrl+C in parent process
	signal(SIGINT, SIG_IGN);
	close(pipe_fd[1]);
	if (*prev_pipe != -1)
		close(*prev_pipe);
	*prev_pipe = pipe_fd[0];
}

void	ft_pipe_and_fork(t_minishell *minishell, int size_cmd, int pipe_fd[2],
		int *pids)
{
	int		i;
	int		prev_pipe;
	t_cmd	*cmd;
	t_child	*child;

	i = 0;
	prev_pipe = -1;
	cmd = minishell->head_cmd;
	child = ft_child_new(minishell);
	if (!child)
		return ;
	while (i < size_cmd)
	{
		child_set(child, i, prev_pipe, size_cmd);
		if (is_built_in(cmd) && size_cmd == 1)
		{
			int saved_stdout;

			saved_stdout = dup(STDOUT_FILENO);
			if (cmd->outfd != STDOUT_FILENO)
				dup2(cmd->outfd, STDOUT_FILENO);
			run_built_in(cmd, minishell);
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		else
		{
			if (pipe(pipe_fd) == -1)
				perror("Pipe");
			pids[i] = fork();
			if (pids[i] < 0)
				ft_exit(minishell, errno, strerror(errno));
			if (pids[i] == 0)
				child_process(minishell, cmd, child, pipe_fd);
			parent_process(&prev_pipe, pipe_fd);
		}
		cmd = cmd->next;
		i++;
	}
	if (prev_pipe != -1)
		close(prev_pipe);
}

void	ft_exec(t_minishell *minishell)
{
	int	*pids;
	int	pipe_fd[2];
	int	size_cmd;

	// WARN: if !minishell->head_cmd is NULL
	size_cmd = ft_cmd_size(minishell->head_cmd);
	if (!size_cmd)
		return ;
	pids = ft_gc_malloc(sizeof(int) * size_cmd, &minishell->gc);
	if (!pids)
		return ;
	ft_pipe_and_fork(minishell, size_cmd, pipe_fd, pids);
	if (is_built_in(minishell->head_cmd) && size_cmd == 1)
		return;
	ft_wait_subprocess(minishell, size_cmd, pids);
}
