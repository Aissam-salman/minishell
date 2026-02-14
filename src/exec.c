/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/14 18:29:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "includes/ft_output.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void ft_pipe_and_fork(t_cmd *cmd,int size_cmd, int pipe_fd[2], int *pids)
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
			if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
				perror("signal error default SIGQUIT");
			if (signal(SIGINT, SIG_DFL) == SIG_ERR)
				perror("signal error default SIGQUIT");
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
		//disable signal ctrl+C
		signal(SIGINT, SIG_IGN);
		close(pipe_fd[1]);
		if (prev_pipe != -1)
			close(prev_pipe);
		prev_pipe = pipe_fd[0];
		cmd = cmd->next;
		i++;
	}
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
	ft_pipe_and_fork(cmd, size_cmd, pipe_fd, pids);
	int i = 0;
	while (i < size_cmd)
	{
		waitpid(pids[i], &status, 0);
		if (WIFSIGNALED(status))
		{
			int sig = WTERMSIG(status);
			if (sig == SIGQUIT)
			{
				ft_printf("Quit");

				if (WCOREDUMP(status))
				{
					ft_printf("\t\t\t\t%s","(core dumped)");
				}
				ft_printf(" %s %s\n", cmd->args[0], cmd->args[1]);
			}
			else if (sig == SIGINT)
			{
				ft_printf("\n");
			}
			if (WIFEXITED(status) > 0)
				ft_printf("Error: %s\n", strerror(status));
		}
		cmd = cmd->next;
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

