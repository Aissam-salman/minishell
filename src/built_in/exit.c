/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:44:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/18 09:45:49 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

long	ft_atoi_safe(const char *nptr, long *res)
{
	long long	nbr;
	int			sign;

	if (!nptr)
		return (-1);
	while (ft_isspace((unsigned char)*nptr))
		nptr++;
	sign = 1;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
			sign = -1;
		nptr++;
	}
	if (!ft_isdigit(*nptr))
		return (GENERAL_ERROR);
	nbr = 0;
	while (ft_isdigit((unsigned char)*nptr))
	{
		nbr = nbr * 10 + (*nptr - '0');
		nptr++;
	}
	*res = (long)nbr  * sign;
	// ft_printf("code atoi: %d\n", *res);
	return (SUCCESS);
}

// A MODIFIER : echo hello | exit 255 doesn´t print anything
// A MODIFIER : Peut prendre jusqu'a long long et renvoyer le result % 256
void	ft_buildin_exit(t_minishell *minishell, char **args)
{
	long	code;
	int i;

	i = 0;
	while (args[i])
		i++;
	write(1, "exit\n", 5);
	if (i > 2)
	{
		ft_error(minishell, 1, "exit: ", "too many arguments");
		return;
	}
	else if (ft_atoi_safe(args[1], &code) == GENERAL_ERROR)
	{
		ft_error(minishell, 2, args[1], ": numeric argument required");
		rl_clear_history();
		ft_gc_free_all(&minishell->gc);
		exit(2);
	}
	else if (i == 1)
		code = minishell->exit_status;
	ft_gc_free_all(&minishell->gc);
	rl_clear_history();
	exit(code % 256);
}
