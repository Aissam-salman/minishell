#include "../includes/minishell.h"
#include <readline/history.h>
#include <signal.h>

void	signal_callback_handler(int sig)
{
	if (sig == SIGINT)
	{
		ft_printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	// NOTE: do nothing, but in process child only you need to handle this,
		// you can disable it for parent
	// else if (sig == SIGQUIT)
	// {
	// 	// set g_signal = sig;
	// 	ft_printf("SIGQUIT: Ctrl-\\");
	// 	ft_printf("\n");
	// 	// Nothing for parent
	// 	// but in child core dump
	// }
}

void	setup_signal(void)
{
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = &signal_callback_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	// C-C
	sigaction(SIGINT, &sa, NULL);
	// C-'\'
	sigaction(SIGQUIT, &sa, NULL);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void)argc;
	(void)argv;
	minishell.envs = envp;
	ft_bzero(&minishell, sizeof(t_minishell));
	setup_signal();
	while (1)
	{
		minishell.line = readline("foo$> ");
		// NOTE: CTRL-D
		// if (minishell.line == NULL)
		// {
		// 	ft_gc_free_all(&minishell.gc);
		// 	exit(0);
		// }
		if (*minishell.line)
		{
			if (*minishell.line == EOF)
				ft_printf("\n");
			ft_gc_add_node(&minishell.gc, minishell.line);
			if (ft_strncmp(minishell.line, "exit", 4) == 0)
				ft_exit(&minishell, 0, "See ya!");
			if (minishell.line[0] != 0)
				add_history(minishell.line);
			ft_tokenize(&minishell);
			// ft_tokens_print(minishell.head_token);
			// if (ft_parse(minishell, minishell.head_token) == ERROR)
			// {
			//		minishell.exit_status = 2;
			//		minishell.head_token = NULL
						// || ft_clear_tokens(&minishell.head_token)
			//		continue ;
			// }
			checker_token(&minishell);
			ft_cmd_lst_create(&minishell);
			ft_exec(&minishell);
			// POUR REMETTRE A ZERO LA LISTE DES TOKENS A CHAQUE EXEC (Free avec garbage collector)
			minishell.head_token = NULL;
			minishell.head_cmd = NULL;
		}
	}

	ft_exit(&minishell, 0, NULL);
}
