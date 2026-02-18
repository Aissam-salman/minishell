/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 14:09:27 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/18 10:23:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_callback_handler(int sig)
{
	// C-C
	if (sig == SIGINT)
	{
		ft_printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	setup_signal(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = &signal_callback_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	// C-C
	if (sigaction(SIGINT, &sa, NULL) == -1)
		perror("signal error SIGINT");
	// C-'\'
	// NOTE: ignore signal by default for parent process
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		perror("signal error ignore SIGQUIT");
}
