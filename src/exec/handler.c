/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:43:32 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 13:10:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void handler_signal_child()
{
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		perror("signal error default SIGQUIT");
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
		perror("signal error default SIGQUIT");
}

void	handler_first_cmd(int infd, int size_cmd, int pipe_fd)
{
	if (infd != 0)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	if (size_cmd > 1)
		dup2(pipe_fd, STDOUT_FILENO);
}

void handler_last_cmd(int prev_pipe, int outfd)
{
	dup2(prev_pipe, STDIN_FILENO);
	if (outfd != 1)
	{
		dup2(outfd, STDOUT_FILENO);
		close(prev_pipe);
		close(outfd);
	}
}

void handler_mid_cmd(int prev_pipe, int pipe_fd)
{
	dup2(prev_pipe, STDIN_FILENO);
	close(prev_pipe);
	dup2(pipe_fd, STDOUT_FILENO);
}
