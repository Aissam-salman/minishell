#include "../includes/minishell.h"
#include "errors.h"

void	ft_minishell_init(t_minishell *minishell)
{
	minishell->head_token = NULL;
	minishell->head_cmd = NULL;
	// minishell->line = NULL;
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void)argv;
	if (argc == 1)
	{
		ft_bzero(&minishell, sizeof(t_minishell));
		minishell.envs = envp;
		ft_env_setup(&minishell, envp);
		// ft_printf("RESULT = %s\n", ft_env_find(minishell.head_env, "PATH")->content);
		// ft_env(minishell.head_env, STDOUT_FILENO);
		while (1)
		{
			setup_signal();
			ft_minishell_init(&minishell);
			minishell.line = readline("foo$> ");
			// NOTE: CTRL-D
			if (!minishell.line)
				ft_buildin_exit(&minishell, 0);
			if (*minishell.line)
			{
				minishell.state = NORMAL;
				ft_gc_add_node(&minishell.gc, minishell.line);
				if (minishell.line[0] != 0)
					add_history(minishell.line);
				if (ft_tokenize(&minishell))
					continue;
				checker_token(&minishell);
				// ft_tokens_print(minishell.head_token);
				if (ft_cmd_lst_create(&minishell) != SUCCESS)
					continue;
				// ft_cmd_print(minishell.head_cmd);
				ft_exec(&minishell);
				// POUR REMETTRE A ZERO LA LISTE DES TOKENS A CHAQUE EXEC (Free avec garbage collector)
			}
		}

	}
	// ft_gc_free_all(&minishell.gc);
	// ft_exit(&minishell, 0, NULL);
}
