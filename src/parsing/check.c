/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 11:57:07 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 17:01:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int ft_check_flags(char *str)
{
	int i;

	i = 0;
	if ( !str[i] || str[i] != '-')
		return (0);
	else
		i++;
	if (str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isalnum(str[i]))
			return(0);
		i++;
	}
	return (1);
}

// REGARDE SI LA STRING EST UN OPERATEUR VALIDE (<, <<, >, >>)
int	ft_check_redirection(char *str)
{
	if (!str)
		return (0);
	if ((str[0] == '<' && !str[1])
		|| (str[0] == '<' && str[1] == '<' && !str[2])
		|| (str[0] == '>' && !str[1])
		|| (str[0] == '>' && str[1] == '>' && !str[2]))
		return (1);
	return (0);
}
