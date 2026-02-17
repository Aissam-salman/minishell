/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/17 12:56:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <unistd.h>

// ERROR + MSG SUR SORTIE ERREUR
int	ft_error(t_minishell *minishell, int error, char *str1, char *str2)
{
	// PENSER  A PRENDRE EN COMPTRE STRERRRNO PLUS SIMPLE POUR LA GESTION
	if (minishell)
		minishell->exit_status = error;
	if (str1 || str2)
	{
		ft_putstr_fd("bash : ", 2);
		if (str1)
			ft_putstr_fd(str1, STDERR_FILENO);
		if (str2)
			ft_putstr_fd(str2, STDERR_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		return (error);
	}
	else if (errno != 0)
	{
		perror("bash : ");
		return (errno);
	}
	return (error);
}

// FONCTION DE FIN DE PROGRAMME, EXIT AVEC VALEUR CODE ERROR
void	ft_exit(t_minishell *minishell, int error, char *str)
{
	// FREE / LIBERE TOUT
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	minishell->exit_status = error;
	// IMPRIME ERREUR
	if (str)
	{
		ft_putstr_fd(str, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	exit(error);
}
