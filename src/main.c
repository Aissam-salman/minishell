#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>

#include "../includes/minishell.h"

int	main(int argc, char const *argv[])
{
	t_minishell minishell;

	(void)argc;
	(void)argv;
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
			{
				ft_gc_free_all(&minishell.gc);
				exit(0);
			}
            char *args[] = {minishell.line, NULL};
			ft_parse(&minishell);
			execve(minishell.line, args, NULL);
		}
		printf("You entered: %s\n", minishell.line);
	}
	ft_gc_free_all(&minishell.gc);
	return (0);
}
