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
#include <string.h>

void	ft_redirection_exec(int new_fd, int *old_fd)
{
	if (*old_fd > 2)
		close(*old_fd);
	*old_fd = new_fd;
}

int	ft_open(char *path, t_types mod)
{
	if (mod == IN_CHEVRON)
		return (open(path, O_RDONLY));
	if (mod == OUT_CHEVRON)
		return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (mod == OUT_DCHEVRON)
		return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

int	ft_redirection_handler(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	int	fd;

	// A MODIFIER : Les valeurs de retour, SET en fonction du cas
	fd = -1;
	if (token->next == NULL)
		return (GENERAL_ERROR);
	if (token->type != IN_DCHEVRON)
		fd = ft_open(token->next->str, token->type);
	else if (token->type == IN_DCHEVRON)
		return (ft_heredoc_handle(minishell, cmd, token), 0);
	if (fd == -1)
	{
		// perror(token->next->str);
		// return (GENERAL_ERROR);
		return (ft_error(minishell, GENERAL_ERROR, strerror(errno), NULL));
	}
	if (fd > 2 && token->type == IN_CHEVRON)
		ft_redirection_exec(fd, &cmd->infd);
	else if (fd > 2 && (token->type == OUT_CHEVRON
			|| token->type == OUT_DCHEVRON))
		ft_redirection_exec(fd, &cmd->outfd);
	return (0);
}
