/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:33:53 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/13 19:42:32 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void ft_echo(char *txt, int have_flag)
{
	// -n  == 1
	if (have_flag)
	{
		if (write(1, txt, ft_strlen(txt)) == -1)
			ft_error(errno, "write",NULL);
	}
	// == 0
	else
	{
		if (write(1, txt, ft_strlen(txt)) == -1)
			ft_error(errno, "write",NULL);
		if (write(1, "\n", 1) == -1)
			ft_error(errno, "write",NULL);

	}
	//TODO: handle \n from readline maybe
}
