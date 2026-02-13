/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cntrl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:49:08 by tibras            #+#    #+#             */
/*   Updated: 2026/02/13 17:41:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

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


void	ft_redirection_handler(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	int fd;

	fd = -1;
	if (token->next == NULL)
		return ;
	if (token->type != IN_DCHEVRON)
		fd = ft_open(token->next->str, token->type);
	else if (token->type == IN_DCHEVRON)
		ft_heredoc_handle(minishell, cmd, token);
	if (fd == -1)
		perror(token->path);
	if (fd > 2 && token->type == IN_CHEVRON)
		ft_redirection_exec(fd, &cmd->infd);
	else if (fd > 2 && (token->type == OUT_CHEVRON || token->type == OUT_DCHEVRON))
		ft_redirection_exec(fd, &cmd->outfd);
}
