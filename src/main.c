#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "minishell.h"

int	main(int argc, char const *argv[])
{
	t_minishell minishell;

	(void)argc;
	(void)argv;
	while (1)
	{
		minishell.line = readline("foo$> ");
		ft_gc_add_node(&minishell.gc, minishell.line);
		if (!minishell.line)
			ft_printf("\n");
		if (*minishell.line)
		{
            char *args[] = {minishell.line, NULL};
			ft_parse(&minishell);
			execve(minishell.line, args, NULL);
		}
		printf("You entered: %s\n", minishell.line);
	}
	ft_gc_free_all(&minishell.gc);
	return (0);
}
