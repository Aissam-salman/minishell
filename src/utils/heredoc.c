/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:04:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/21 19:42:39 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

void ft_signal_heredoc_handler(int sig)
{
	(void)sig;
	g_signal_glob = SIGINT;
	write(1, "\n", 1);
	close(STDIN_FILENO);
}

int ft_signal_here_ctrld(int saved_stdin, t_token *token)
{
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(token->str, 2);
	ft_putstr_fd("')\n", 2);
	return (SUCCESS);
}

int ft_signal_here_ctrlc(int saved_stdin, char *line)
{
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	free(line);
	return (SUCCESS);
}

void	ft_heredoc(t_minishell *minishell, t_cmd *cmd, t_token *token, int mod)
{
	char	*line;
	int		pipefd[2];
	size_t	limiter_len;
	size_t	line_len;
	int 	saved_stdin;

	limiter_len = ft_strlen(token->str);
	if (pipe(pipefd))
	{
		ft_error(NULL, GENERAL_ERROR, "Heredoc error :",
			"Pipe creation failed");
		return ;
	}
	saved_stdin = dup(STDIN_FILENO);
	signal(SIGINT, ft_signal_heredoc_handler);
	while (1)
	{
		line = readline("> ");
		if (g_signal_glob == SIGINT && ft_signal_here_ctrlc(saved_stdin, line))
			break ;
		if (!line && ft_signal_here_ctrld(saved_stdin, token))
			break ;
		else
			ft_gc_add_node(&minishell->gc, line);
		line_len = ft_strlen(line);
		if (!ft_strncmp(line, token->str, limiter_len)
			&& line_len == limiter_len)
			break ;
		ft_putstr_fd(line, pipefd[1]);
		ft_putchar_fd('\n', pipefd[1]);
		line = NULL;
	}
	close(saved_stdin);
	//pipefd, minishell, mod, cmd
	close(pipefd[1]);
	if (g_signal_glob == SIGINT)
	{
		close(pipefd[0]);
		minishell->exit_status = 130;
	}
	// SI DERNIER HERE_DOC DE LA CHAINE
	if (mod == 1)
		ft_redirection_exec(pipefd[0], &cmd->infd);
	else
		close(pipefd[0]);
	setup_signal();
}

t_token	*ft_heredoc_find_last(t_token *token)
{
	t_token	*last;

	last = NULL;
	while (token && token->type != PIPE)
	{
		if (token->type == IN_DCHEVRON)
			last = token;
		token = token->next;
	}
	return (last);
}

void	ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	t_token	*last;

	last = ft_heredoc_find_last(token);
	if (token == last)
		ft_heredoc(minishell, cmd, token->next, 1);
	else
		ft_heredoc(minishell, cmd, token->next, 0);
}
