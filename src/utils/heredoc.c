/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:04:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/13 15:22:24 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_heredoc(t_cmd *cmd, t_token *token)
{
	char *line;


	(void)cmd;
	(void)token;
	ft_printf("HEREDOC\n");
	while (1)
	{
		line = readline("here_doc > ");
		if (!ft_strncmp(line, token->str, ft_strlen(token->str)))
		{
			ft_putstr_fd("HERE_DOC TROUVE\n", 1);
			break;
		}
		free(line);
		line = NULL;
	}

}

t_token	*ft_heredoc_find_last(t_token *token)
{
	t_token *last;

	last = NULL;
	while (token)
	{
		if (token->type == IN_DCHEVRON)
			last = token;
		token = token->next;
	}
	return (last);
}
