/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/11 19:46:23 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
		close(outfd);
	}
}

void handler_mid_cmd(int prev_pipe, int pipe_fd)
{
	dup2(prev_pipe, STDIN_FILENO);
	close(prev_pipe);
	dup2(pipe_fd, STDIN_FILENO);
}

void close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2])
{

	close(pipe_fd[0]);
	close(pipe_fd[1]);
	if (execv(cmd->path, cmd->args) == -1)
		ft_exit(minishell, errno, "EXECV");
}

typedef struct s_child
{
	int index;
	int prev_pipe;
	int size_cmd;
} t_child;

void child_set(t_child *child, int i, int prev_pipe, int size_cmd)
{
	child->prev_pipe = prev_pipe;
	child->index = i;
	child->size_cmd = size_cmd;
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

void parent_process(int *prev_pipe, int pipe_fd[2])
{
	//disable signal ctrl+C in parent process
	signal(SIGINT, SIG_IGN);
	close(pipe_fd[1]);
	if (*prev_pipe != -1)
		close(*prev_pipe);
	*prev_pipe = pipe_fd[0];
}

void ft_pipe_and_fork(t_minishell *minishell,int size_cmd, int pipe_fd[2], int *pids)
{
	int i;
	int prev_pipe;
	t_cmd *cmd;
	t_child *child;

	i = 0;
	prev_pipe = -1;
	cmd = minishell->head_cmd;
	child = ft_child_new(minishell);
	if (!child)
		return;
	while (i < size_cmd)
	{
		child_set(child, i, prev_pipe, size_cmd);
		if (pipe(pipe_fd) == -1)
			perror("Pipe");
		pids[i] = fork();
		if (pids[i] < 0)
			ft_exit(minishell, errno, "FORK");
		if (pids[i] == 0)
			child_process(minishell, cmd, child,  pipe_fd);
		parent_process(&prev_pipe, pipe_fd);
		cmd = cmd->next;
		i++;
	}
}

void handler_status(int status, t_cmd *cmd)
{
	int sig;

	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
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
}

void ft_wait_subprocess(t_minishell *minishell, int size_cmd, int *pids)
{
	int status;
	t_cmd *cmd;

	cmd = minishell->head_cmd;
	int i = 0;
	while (i < size_cmd)
	{
		waitpid(pids[i], &status, 0);
		handler_status(status, cmd);
		cmd = cmd->next;
		i++;
	}
}

void pipefd_set(int pipe_fd[2])
{
	pipe_fd[0] = -1;
	pipe_fd[1] = -1;
}

void	ft_exec(t_minishell *minishell)
{
	int *pids;
	int pipe_fd[2];
	int size_cmd;

	pipefd_set(pipe_fd);
	//WARN: if !minishell->head_cmd is NULL
	size_cmd = ft_cmd_size(minishell->head_cmd);
	if (!size_cmd)
		return;
	pids = ft_gc_malloc(sizeof(int) * size_cmd, &minishell->gc);
	if (!pids)
		return;
	//NOTE: if size == 1 and build in qui touche a l'env separer le bordel
	// handle if only export KEY=VALUE  ft_export, 
	ft_pipe_and_fork(minishell, size_cmd, pipe_fd, pids);
	ft_wait_subprocess(minishell, size_cmd, pids);
}

