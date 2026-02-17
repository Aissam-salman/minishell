/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:04:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/17 12:17:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// A MODIFIER : RAJOUTER LA GESTION DES SIGNAUX
void	ft_heredoc(t_minishell *minishell, t_cmd *cmd, t_token *token, int mod)
{
	char *line;
	int pipefd[2];
	size_t	limiter_len;
	size_t	line_len;

	limiter_len = ft_strlen(token->str);
	// ON SECURISE L'OUVERTUR DU PIPE
	if (pipe(pipefd))
	{
		ft_error(NULL, GENERAL_ERROR, "Heredoc error :", "Pipe creation failed");
		return;
	}

	// BOUCLE DE READ
	while (1)
	{
		// RECUPERE LA LINE
		line = readline("> ");
		// ctrl-D
		if (!line)
			break;
		else
			ft_gc_add_node(&minishell->gc, line);
		line_len = ft_strlen(line);

		// SI HERE_DOC TROUVÉ
		if (!ft_strncmp(line, token->str, limiter_len) && line_len == limiter_len)
			break;

		// IMPRIME LA LINE DANS PIPE
		ft_putstr_fd(line, pipefd[1]);
		ft_putchar_fd('\n', pipefd[1]);
		line = NULL;
	}
	close(pipefd[1]);
	// SI DERNIER HERE_DOC DE LA CHAINE
	if (mod == 1)
		ft_redirection_exec(pipefd[0], &cmd->infd);
	else
		close(pipefd[0]);
}

t_token	*ft_heredoc_find_last(t_token *token)
{
	t_token *last;

	last = NULL;
	while (token && token->type != PIPE)
	{
		if (token->type == IN_DCHEVRON)
		{
			// ft_printf("TEST HEREDOC : %s\n", token->next->str);
			last = token;
		}
		token = token->next;
	}
	// if (last)
	// 	ft_printf("%s\n", last->next->str);
	return (last);
}

void	ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	t_token *last;

	// TROUVER LE DERNIER HEREDOC
	last = ft_heredoc_find_last(token);
	// while (token && token->next->type != PIPE)
	// {
	if (token == last)
		ft_heredoc(minishell, cmd, token->next, 1);
	else
		ft_heredoc(minishell, cmd, token->next, 0);
}
