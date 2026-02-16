/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 11:57:07 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 14:04:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/minishell.h"

int ft_check_flags(char *str)
{
	int i;

	i = 0;
	if (str[i] != '-' || !str[i])
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

// A MODIFIER : POUR POUVOIR GERE "unvalid syntax near token <<"
int ft_check_redirection(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		if (str[i] != str[0] || i > 2)
		{
			return (1);
		}
		i++;
	}
    // if (!str)
	//     return (0);
    // if (str[0] == ' ')
	//     return (0);
    // else if ((str[0] == '<' && !str[1]) || (str[0] == '>' && !str[1]))
	//     return (1);
    // else if ((str[0] == '<' && str[1] == '<' && !str[2]) ||
	// 	    (str[0] == '>' && str[1] == '>' && !str[2]))
	//     return (1);
    // return (0);
	return (0);
}

int ft_check_heredoc_end(char *str)
{
	//NOTE: maybe handle " " and ""
	if (!str)
		return (0);
	return (1);
}

int ft_check_file_of_redirection(t_token *token)
{
	if (token->type == IN_CHEVRON)
	{
		if (access(token->next->str, R_OK) == -1)
			return (0);
	}
	else if (token->type == IN_DCHEVRON)
	{
		if (ft_check_heredoc_end(token->next->str) == 0)
			return (0);
	}
	return (1);
}

int ft_check_file(t_token *token)
{
	if (access(token->str, R_OK | W_OK) == -1)
		return (0);
	return (1);
}
