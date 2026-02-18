/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cntrl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:49:08 by tibras            #+#    #+#             */
/*   Updated: 2026/02/18 12:08:03 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void	ft_redirection_exec(int new_fd, int *old_fd)
{
	if (*old_fd > 2)
		close(*old_fd);
	*old_fd = new_fd;
}



int	ft_open(char *path, t_types mod)
{
	int fd;

	fd = -1;
	if (mod == IN_CHEVRON)
		fd = open(path, O_RDONLY);
	else if (mod == OUT_CHEVRON)
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (mod == OUT_DCHEVRON)
		fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	return (fd);
}

int	ft_redirection_handler(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	int	fd;

	// A MODIFIER : Les valeurs de retour, SET en fonction du cas
	if (token->next == NULL)
		return (GENERAL_ERROR);
	if (token->type == IN_DCHEVRON)
		return (ft_heredoc_handle(minishell, cmd, token), 0);
	fd = ft_open(token->next->str, token->type);
	if (fd == -1)
	{
		cmd->error_file = 1;
		perror(token->next->str);
		minishell->exit_status = GENERAL_ERROR;
		return (GENERAL_ERROR);
	}
	if (fd > 2 && token->type == IN_CHEVRON)
		ft_redirection_exec(fd, &cmd->infd);
	else if (fd > 2 && (token->type == OUT_CHEVRON
			|| token->type == OUT_DCHEVRON))
		ft_redirection_exec(fd, &cmd->outfd);
	return (SUCCESS);
}
