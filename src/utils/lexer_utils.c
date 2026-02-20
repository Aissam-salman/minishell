/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:50:24 by tibras            #+#    #+#             */
/*   Updated: 2026/02/20 10:52:24 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_buffer_add(char *buffer, char c)
{
	int	len;

	len = ft_strlen(buffer);
	if (len < BUFFER_SIZE)
	{
		buffer[len] = c;
		buffer[len + 1] = '\0';
		return (0);
	}
	else
		return (ft_error(NULL, BUFFER_FAIL, ERRS_BUF_SMALL, NULL));
}

