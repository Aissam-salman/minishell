/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:44:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/14 14:45:45 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include "includes/ft_conversion.h"

void ft_buildin_exit(t_minishell *minishell, char *code_exit)
{
	int code;

	code = 0;
	if (code_exit)
		code = ft_atoi(code_exit);
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	write(1, "exit\n", 5);
	exit(code);
}
