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

void ft_echo(char **args, int have_flag)
{
	//FIX: handle -nnnnnnn -nnn -nnnnn flags
	int i;

	if (!args || !*args)
	{
		if (write(1, "\n", 1) == -1)
			ft_error(errno, "write",NULL);
		return;
	}
	// -n  == 1
	if (!args)
		return;
	if (have_flag)
	{
		i = 2;
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(errno, "write",NULL);
			i++;
		}
	}
	else
{
		i = 1;
		while (args[i])
		{
			if (write(1, args[i], ft_strlen(args[i])) == -1)
				ft_error(errno, "write",NULL);
			if (write(1, "\n", 1) == -1)
				ft_error(errno, "write",NULL);
			i++;
		}

	}
	//TODO: handle \n from readline maybe
}
