/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:44:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/13 20:54:21 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void ft_buildin_exit(t_minishell *minishell)
{
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	write(1, "exit\n", 5);
	exit(0);
}
