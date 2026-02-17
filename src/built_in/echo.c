/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:33:53 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/17 12:16:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int ft_is_flag(char *str)
{
	int j = 0;
	if (str[j] && str[j] == '-')
		j++;
	else
		return (0);
	while (str[j] && str[j] == 'n') 
		j++;
	if (!str[j])
		return (1);
	return (0);
}

void ft_echo(char **args)
{
	int i;
	int is_flag;

	if (!args || !*args || !args[1])
	{
		if (write(1, "\n", 1) == -1)
			ft_error(NULL, errno, "write",NULL);
		return;
	}

	if (args[1][0] == '-' && !args[1][1])
	{
		if (write(1, "-", 1) == -1)
			ft_error(NULL, errno, "write",NULL);
		if (write(1, "\n", 1) == -1)
			ft_error(NULL, errno, "write",NULL);
		return;
	}
	i = 1;
	if (args[i][0] == '-' && args[i][i] == 'n' )
		is_flag = 1;
	else 
		is_flag = 0;
	if (is_flag)
	{
		while (args[i])
		{
			if (!ft_is_flag(args[i]))
				break;
			i++;
		}
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(NULL, errno, "write",NULL);
			if (args[i] && args[i + 1])
			{
				if (write(1, " ", 1) == -1)
					ft_error(NULL, errno, "write",NULL);
			}
			i++;
		}
	}
	else
	{
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(NULL, errno, "write",NULL);
			if (args[i] && args[i + 1])
			{
				if (write(1, " ", 1) == -1)
					ft_error(NULL, errno, "write",NULL);
			}
			i++;
		}
	}
	if (!is_flag)
		write(1, "\n", 1);
}
