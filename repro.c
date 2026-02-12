#include "includes/minishell.h"
#include <stdio.h>

int main()
{
    t_minishell minishell = {0};
    char *path = "/bin/:/usr/bin";
    char **res = ft_split_sep_gc(path, ':', &minishell.gc);
    
    int i = 0;
    while (res && res[i])
    {
        printf("Path %d: '%s'\n", i, res[i]);
        i++;
    }
    return 0;
}
