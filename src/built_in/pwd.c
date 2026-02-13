/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:02:48 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/13 20:01:27 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void ft_pwd()
{
	char *buff;

	// Alloc auto par la function
	buff = getcwd(NULL, 0);
	if (buff)
	{
		printf("%s", buff);
		free(buff);
		return;
	}
	ft_error(errno,"pwd",NULL);
}
