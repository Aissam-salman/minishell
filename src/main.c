#include "../includes/minishell.h"
#include <signal.h>
#include <readline/history.h>

void	signal_callback_handler(int sig)
{
	if (sig == SIGINT)
	{
		ft_printf("\n");
		rl_on_new_line();
		rl_replace_line("",0);
		rl_redisplay();
	}
	//NOTE: do nothing, but in process child only you need to handle this, you can disable it for parent
	// else if (sig == SIGQUIT)
	// {
	// 	// set g_signal = sig;
	// 	ft_printf("SIGQUIT: Ctrl-\\");
	// 	ft_printf("\n");
	// 	// Nothing for parent 
	// 	// but in child core dump
	// }
}

//TODO: make func for setup signal

int	main()
{
	t_minishell minishell;
	struct sigaction	sa;

	ft_bzero(&sa, sizeof(sa));
	sa.sa_handler = &signal_callback_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	// C-C
	sigaction(SIGINT, &sa, NULL);
	// C-'\' 
	sigaction(SIGQUIT, &sa, NULL);

	// printf("%s",getenv("PATH"));
	ft_bzero(&minishell, sizeof(t_minishell));
	while (1)
	{
		minishell.line = readline("foo$> ");
		//NOTE: CTRL-D
		if (minishell.line == NULL)
		{
			ft_gc_free_all(&minishell.gc);
			exit(0);
		}
		else if (*minishell.line == EOF)
			ft_printf("\n");
		ft_gc_add_node(&minishell.gc, minishell.line);
		if (*minishell.line)
		{
			// history
			if (ft_strncmp(minishell.line, "exit", 4) == 0)
			{
				ft_gc_free_all(&minishell.gc);
				exit(0);
			}
			char *args[] = {minishell.line, NULL};
			ft_parse(&minishell);
			// execution
			exit(0);
			execve("/bin/echo", args, NULL);
		}
		// printf("You entered: %s\n", minishell.line);
	}
	ft_gc_free_all(&minishell.gc);
	return (0);
}
