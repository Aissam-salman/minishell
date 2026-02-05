#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "minishell.h"

int	main(int argc, char const *argv[])
{
	t_minishell minishell;
	char	*line;

	(void)argc;
	(void)argv;
	while (1)
	{
		line = readline("foo$> ");
		ft_gc_add_node(&minishell.gc, (void *)line);
		if (!line)
			ft_printf("\n");
		if (*line)
		{
            char *args[] = {line, NULL};
			execve(line, args, NULL);
		}
		printf("You entered: %s\n", line);
		free(line);
	}
	return (0);
}
