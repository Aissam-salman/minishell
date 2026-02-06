/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/05 16:37:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_exit(t_minishell *minishell, int error, char *str )
{
	if (str)
		ft_putstr_fd(str, 2);
	ft_gc_free_all(&minishell->gc);
	exit(error);
}