/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   extra_bin.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:46:32 by tibras            #+#    #+#             */
/*   Updated: 2026/02/17 11:52:14 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_extra_bin(t_minishell *minishell, int mod)
{
	if (mod == 0)
		minishell->exit_status = 0;
	if (mod == 1)	
		minishell->exit_status = 1;
}