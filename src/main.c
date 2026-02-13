#include "../includes/minishell.h"
#include <signal.h>
#include <readline/history.h>
#include <signal.h>
#include <stdio.h>

void	signal_callback_handler(int sig)
{
	//C-C
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
	//NOTE: ignore signal by default for parent process
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		perror("signal error ignore SIGQUIT");
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void)argc;
	(void)argv;
	ft_bzero(&minishell, sizeof(t_minishell));
	minishell.envs = envp;
	ft_env_setup(&minishell, envp);
	// ft_printf("RESULT = %s\n", ft_env_find(minishell.head_env, "PATH")->content);
	// ft_env(minishell.head_env, STDOUT_FILENO);
	setup_signal();
	while (1)
	{
		setup_signal();
		minishell.line = readline("foo$> ");
		// NOTE: CTRL-D
		if (minishell.line == NULL)
		{
			ft_gc_free_all(&minishell.gc);
			exit(0);
		}
		if (*minishell.line)
		{
			if (*minishell.line == EOF)
				ft_printf("\n");
			ft_gc_add_node(&minishell.gc, minishell.line);
			if (ft_strncmp(minishell.line, "exit", 4) == 0)
				ft_exit(&minishell, 0, "See ya!");
			if (minishell.line[0] != 0)
				add_history(minishell.line);
			if (ft_tokenize(&minishell))
				continue;

			// ft_tokens_print(minishell.head_token);
			// if (ft_parse(minishell, minishell.head_token) == ERROR)
			// {
			//		minishell.exit_status = 2;
			//		minishell.head_token = NULL
						// || ft_clear_tokens(&minishell.head_token)
			//		continue ;
			// }
			checker_token(&minishell);
			// ft_tokens_print(minishell.head_token);
			if (ft_cmd_lst_create(&minishell) != SUCCESS)
				continue;
			// ft_cmd_print(minishell.head_cmd);
			ft_exec(&minishell);
			// POUR REMETTRE A ZERO LA LISTE DES TOKENS A CHAQUE EXEC (Free avec garbage collector)
			minishell.head_token = NULL;
			minishell.head_cmd = NULL;
		}
	}
	ft_gc_free_all(&minishell.gc);
	ft_exit(&minishell, 0, NULL);
}
