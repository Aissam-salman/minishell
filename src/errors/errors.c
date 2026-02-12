/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/12 14:34:46 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// ERROR + MSG SUR SORTIE ERREUR
int	ft_error(int error, char *str)
{
	// PENSER  A PRENDRE EN COMPTRE STRERRRNO PLUS SIMPLE POUR LA GESTION
	ft_putstr_fd(str, STDERR_FILENO);
	return (error);
}

// FONCTION DE FIN DE PROGRAMME, EXIT AVEC VALEUR CODE ERROR
void	ft_exit(t_minishell *minishell, int error, char *str)
{
	if (str)
		ft_putstr_fd(str, STDERR_FILENO);
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	exit(error);
}
