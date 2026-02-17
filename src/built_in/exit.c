/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:44:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/16 17:38:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	ft_buildin_exit(t_minishell *minishell, char *code_exit)
{
	int	code;

	code = 0;
	if (code_exit)
		code = ft_atoi(code_exit);
	else
		code = minishell->exit_status;
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	write(1, "exit\n", 5);
	exit(code);
}
