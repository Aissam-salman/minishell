/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 19:19:36 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// ERROR + MSG SUR SORTIE ERREUR
int	ft_error(int error, char *str)
{
	// PENSER  A PRENDRE EN COMPTRE STRERRRNO PLUS SIMPLE POUR LA GESTION
	// ft_putstr_fd(str, STDERR_FILENO);
	// if (str)
	// 	ft_putstr_fd(str, STDERR_FILENO);
	if (errno != 0)
	{
		perror("Minishell : ");
		return (errno);
	}
	else
	{
		ft_putstr_fd("Minishell : ", 2);
		ft_putstr_fd(str, 2);
		return (error);
	}
}

// FONCTION DE FIN DE PROGRAMME, EXIT AVEC VALEUR CODE ERROR
void	ft_exit(t_minishell *minishell, int error, char *str)
{
	// FREE / LIBERE TOUT
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();

	// IMPRIME ERREUR
	if (str)
		ft_putstr_fd(str, STDERR_FILENO);
	exit(error);
}
