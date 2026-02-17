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
	//FIX: handle -nnnnnnn -nnn -nnnnn flags
	i = 1;
	if (args[i][0] == '-' && args[i][i] == 'n' )
		is_flag = 1;
	else 
		is_flag = 0;
	if (is_flag)
	{
		while (args[i])
		{
			// tant que -nnnn avancer j = 0 == '-' 
			// si next -nnn ok      j = 1++ == 'n' si diff break
			int j = 0;
			if (args[i] && args[i][j] == '-')
				j++;
			else
				break;
			while (args[i] && args[i][j]) 
			{
				if (args[i][j] != 'n')
					break;
				j++;
			}
			if (args[i][j] != 'n')
					break;
			i++;
		}
		i++;
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(NULL, errno, "write",NULL);
			if (args[i + 1])
				write(1, " ", 1);
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
			if (write(1, " ", 1) == -1)
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
