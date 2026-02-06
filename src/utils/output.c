/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 13:40:37 by tibras            #+#    #+#             */
/*   Updated: 2026/02/06 18:05:17 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_elem_print(t_elements *head)
{
	while (head)
	{
		ft_printf("%s || %d\n", head->str, head->type);
		head = head->next;
	}
}


void	ft_print_state(char c, char *buffer, t_minishell *minishell)
{
	if (!buffer[0])
		ft_printf("EMPTY BUFFER ||", buffer);
	else
		ft_printf("BUFFER = {%s} || ", buffer);
	ft_printf("CHARACTER  = \"%c\" || STATE = ", c);
	if (minishell->state == NORMAL)
		ft_printf("NORMAL\n");
	if (minishell->state == IN_QUOTE)
		ft_printf("IN_QUOTE\n");
	if (minishell->state == IN_DQUOTE)
		ft_printf("IN_DQUOTE\n");
	if (minishell->state == WAITING)
		ft_printf("WAITING\n");
}