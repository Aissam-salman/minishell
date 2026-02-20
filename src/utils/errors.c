/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/19 11:34:42 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <asm-generic/errno-base.h>

// IMPRIME UN MESSAGE D'ERREUR SUR STDERR
static void	ft_errmsg(char *str1, char *str2)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (str1)
		ft_putstr_fd(str1, STDERR_FILENO);
	if (str2)
		ft_putstr_fd(str2, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
}

// ERROR + MSG SUR SORTIE ERREUR
int	ft_error(t_minishell *minishell, int error, char *str1, char *str2)
{
	if (minishell)
		minishell->exit_status = error;
	if (str1 || str2)
		ft_errmsg(str1, str2);
	else if (errno != 0)
	{
		perror("minishell");
		if (errno == EACCES)
			return (126);
	}
	return (error);
}

// FONCTION DE FIN DE PROGRAMME, EXIT AVEC VALEUR CODE ERROR
void	ft_exit(t_minishell *minishell, int error, char *str)
{
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	minishell->exit_status = error;
	if (str)
		ft_errmsg(str, NULL);
	exit(error);
}
