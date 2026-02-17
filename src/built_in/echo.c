/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:33:53 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 18:02:35 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void ft_echo(char **args)
{
	int i;
	int is_flag;

	if (!args || !*args)
	{
		if (write(1, "\n", 1) == -1)
			ft_error(errno, "write",NULL);
		return;
	}
	//FIX: handle -nnnnnnn -nnn -nnnnn flags
	i = 1;
	if (args[i][0] == '-')
		is_flag = 1;
	else 
		is_flag = 0;
	if (is_flag)
	{
		int j = 0;

		while (args[i] && args[i][j])
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(errno, "write",NULL);
			if (args[i + 1])
				write(1, " ", 1);
			i++;
		}
	}
	else
	{
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(errno, "write",NULL);
			if (write(1, " ", 1) == -1)
				ft_error(errno, "write",NULL);
			i++;
		}
	}
	if (!is_flag)
		write(1, "\n", 1);
}
