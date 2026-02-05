#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	main(int argc, char const *argv[])
{
	char	*line;

	while (1)
	{
		line = readline("foo$> ");
		if (!line)
			printf("\n");
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
