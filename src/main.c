#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>

#include "../includes/minishell.h"

int	main()
{
	t_minishell minishell;

	// handle signal 
	// printf("%s",getenv("PATH"));
	ft_bzero(&minishell, sizeof(t_minishell));
	while (1)
	{
		minishell.line = readline("foo$> ");
		if (!minishell.line)
			ft_printf("\n");
		ft_gc_add_node(&minishell.gc, minishell.line);
		if (*minishell.line)
		{
			if (ft_strncmp(minishell.line, "exit", 4) == 0)
				ft_exit(&minishell, 0, "See ya!");

			if (minishell.line[0] != 0)
				add_history(minishell.line);
			ft_parse(&minishell);
			// char *args[] = {minishell.line, NULL};
			// POUR LIBERER LA LISTE DES TOKENS A CHAQUE EXECT
			minishell.head_token = NULL;
			// execve("/bin/echo", args, NULL);
		}
		printf("You entered: %s\n", minishell.line);
	}
	ft_exit(&minishell, 0, NULL);
}
