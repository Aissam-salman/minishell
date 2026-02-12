/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/11 19:46:23 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

t_cmd *ft_pipe_and_fork(t_cmd *cmd,int size_cmd, int pipe_fd[2], int *pids)
{
	int i;
	int prev_pipe;

	i = 0;
	prev_pipe = -1;
	while (i < size_cmd)
	{
		if (pipe(pipe_fd) == -1)
			perror("Pipe");
		pids[i] = fork();
		if (pids[i] < 0)
		{
			perror("fork fail");
			exit(1);
		}
		//CHILD
		if (pids[i] == 0)
		{
			// first cmd
			if (i == 0)
			{
				if (cmd->infd != 0)
				{
					dup2(cmd->infd, STDIN_FILENO);
					close(cmd->infd);
				}
				if (size_cmd > 1)
					dup2(pipe_fd[1], STDOUT_FILENO);
			}
			else if (i == size_cmd - 1)
			{
				dup2(prev_pipe, STDIN_FILENO);
				if (cmd->outfd != 1)
				{
					dup2(cmd->outfd, STDOUT_FILENO);
					close(cmd->outfd);
				}
			}
			else
			{
				dup2(prev_pipe, STDIN_FILENO);
				close(prev_pipe);
				dup2(pipe_fd[1], STDIN_FILENO);
			}
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			execv(cmd->path, cmd->args);
			perror("Execv");
			exit(1);
		}
		close(pipe_fd[1]);
		if (prev_pipe != -1)
			close(prev_pipe);
		prev_pipe = pipe_fd[0];
		cmd = cmd->next;
		i++;
	}
	return (cmd);
}

void	ft_exec(t_minishell *minishell)
{
	t_cmd *cmd;
	int pipe_fd[2];
	int *pids;
	int size_cmd;
	int status;

	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
	cmd = minishell->head_cmd;
	size_cmd = ft_cmd_size(minishell->head_cmd);
	if (!size_cmd)
		return;
	pids = ft_gc_malloc(sizeof(int) * size_cmd, &minishell->gc);
	if (!pids)
		return;
	cmd = ft_pipe_and_fork(cmd, size_cmd, pipe_fd, pids);
	int i = 0;
	while (i < size_cmd)
	{
		waitpid(pids[i], &status, 0);
		i++;
	}
	// int pid;
	// int respipe;
	// int pipefd[2];

	// respipe = pipe(pipefd);
	// if (respipe == -1)
	// 	return;
	// pid = fork();
	// if (pid == 0)
	// {
	//
	// 	execve(minishell->head_cmd->path, minishell->head_cmd->args,minishell->envs );
	// 	perror("EXEC\n");
	// 	exit(1);
	// }
	// wait(&pid);
	// else
	// {
	// 	waitpid(-1);
	// }
}

