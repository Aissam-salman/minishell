#include "../includes/minishell.h"

void	ft_minishell_reset(t_minishell *minishell)
{
	minishell->head_token = NULL;
	minishell->head_cmd = NULL;
	minishell->state = NORMAL;
	minishell->cached_status = minishell->exit_status ;
	minishell->exit_status = 0;
	// minishell->line = NULL;
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	(void)argv;
	(void)argc;
	ft_bzero(&minishell, sizeof(t_minishell));
	ft_env_setup(&minishell, envp);
	while (1)
	{
		setup_signal();
		ft_minishell_reset(&minishell);
		minishell.line = readline("foo$> ");
		if (!minishell.line)
				ft_buildin_exit(&minishell, 0);
		if (!*minishell.line)
		{
			free(minishell.line);
			continue ;
		}
		ft_gc_add_node(&minishell.gc, minishell.line);
		add_history(minishell.line);
		ft_tokenize(&minishell);
		if (minishell.exit_status != 0)
			continue;
		checker_token(&minishell);
		if (minishell.exit_status != 0)
			continue;
		ft_cmd_lst_create(&minishell);
		if (minishell.exit_status != 0)
			continue;
		ft_exec(&minishell);
	}
	ft_exit(&minishell, minishell.exit_status, NULL);
}
