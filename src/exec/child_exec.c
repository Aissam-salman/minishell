/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_exec.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:31:47 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 11:44:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"
#include "utils.h"
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
	struct stat	path_stat;
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (cmd->outfd == -1 || cmd->infd == -1 || cmd->error_file == 1)
		exit(GENERAL_ERROR);

	if (is_built_in(cmd))
	{
		run_built_in(cmd, minishell);
		ft_gc_free_all(&minishell->gc);
		rl_clear_history();
		exit(SUCCESS);
	}
	
	if (!cmd->path)
		ft_exit(minishell, CMD_NOT_FOUND, " command not found");
	if (access(cmd->path, X_OK) == -1)
	{
		if (errno == EACCES)
		{
			if (ft_strchr(cmd->path, '/'))
				ft_exit(minishell, PERMISSION_DENIED, " Permission denied");
			else
				ft_exit(minishell, CMD_NOT_FOUND, " command not found");
		}
		else if (!ft_strchr(cmd->path, '/'))
			ft_exit(minishell, CMD_NOT_FOUND, " command not found");
		else
			ft_exit(minishell, CMD_NOT_FOUND, " No such file or directory");
	}
	if (stat(cmd->path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
	{
		if (ft_strchr(cmd->path, '/'))
			ft_exit(minishell, 126, " Is a directory");
		else
			ft_exit(minishell, 127, " command not found");
	}
	if (execv(cmd->path, cmd->args) == -1)
	{
		ft_error(minishell, errno, strerror(errno), NULL);
		exit(errno);
	}
}

void	child_process(t_minishell *minishell, t_cmd *cmd, t_child *child,
		int pipe_fd[2])
{
	handler_signal_child();
	if (child->index == 0)
		handler_first_cmd(cmd->infd, cmd->outfd, child->size_cmd, pipe_fd[1]);
	else if (child->index == child->size_cmd - 1)
		handler_last_cmd(cmd->infd, child->prev_pipe, cmd->outfd);
	else
		handler_mid_cmd(cmd->infd, child->prev_pipe, cmd->outfd, pipe_fd[1]);
	close_pipe_and_exec(cmd, minishell, pipe_fd);
}
