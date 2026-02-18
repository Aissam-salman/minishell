# Heredoc Implementation Explanation

This document explains the logic behind the `ft_heredoc` function implementation for Minishell.

## Overview

The Heredoc (`<<`) redirection allows the user to input multiple lines of text interactively until a specific delimiter is typed. This accumulated input is then passed as standard input to the command.

## Key Implementation Concepts

### 1. Using a Pipe (`pipe()`)
We use a pipe to simulate a file. Instead of creating a temporary file on disk (which is slower and requires cleanup), we keep the data in kernel memory.
*   **`pipefd[1]` (Write End)**: Used by the parent process to write the user's input.
*   **`pipefd[0]` (Read End)**: Will be assigned to the command's input (`stdin`).

### 2. The Reading Loop (`readline`)
The loop continues until the delimiter is found or the user sends an EOF signal (Ctrl+D).

*   **Handling Ctrl+D**: `readline` returns `NULL` on EOF. We must check `if (!line)` *before* checking the length or content to avoid segmentation faults.
*   **Checking Delimiter**: We compare the input line against the delimiter. If it matches exactly, we break the loop.
*   **Writing to Pipe**: We write the line to `pipefd[1]` and must manually add a newline `\n`, as `readline` strips it.

### 3. Resource Management
*   **Memory**: The `line` returned by `readline` must be freed (or added to your garbage collector) in every iteration.
*   **Closing The Write End**: After the loop finishes, **you must close `pipefd[1]`**. This sends an EOF signal to the reader. If you forget this, the command reading from the pipe (like `cat` or `grep`) will hang forever waiting for more data.

### 4. Handling Multiple Heredocs
Bash allows multiple heredocs (e.g., `<< A << B`), but only the **last one** is actually used as input for the command.
*   **Argument `mod == 1`**: This is the last heredoc. We save `pipefd[0]` into `cmd->fd_in`.
*   **Argument `mod == 0`**: These are ignored heredocs. We read the input to clear it from the terminal buffer, but we immediately close `pipefd[0]` because the data won't be used.

---

## Corrected Code Snippet

Here is the robust version of the function incorporating these concepts:

```c
#include "minishell.h"

void    ft_heredoc(t_minishell *minishell, t_cmd *cmd, t_token *token, int mod)
{
    char    *line;
    int     pipefd[2];
    char    *delimiter;

    // 1. Validate token existence
    if (!token->next)
        return ;
    delimiter = token->next->str;

    // 2. Create the pipe
    if (pipe(pipefd) == -1)
    {
        ft_error(GENERAL_ERROR, "Heredoc error", "Pipe failed");
        return ;
    }

    // 3. Read Loop
    while (1)
    {
        line = readline("here_doc > ");
        
        // Add to GC to ensure it gets freed eventually
        if (line)
            ft_gc_add_node(minishell, line);
        
        // Handle Ctrl+D (EOF)
        if (!line)
        {
            // Optional: Print warning here
            break ;
        }

        // Check for Delimiter Match (using ft_strcmp for exact match)
        // Ensure you have an ft_strcmp or use ft_strncmp with max length logic
        if (ft_strcmp(line, delimiter) == 0)
            break ;

        // Write to pipe
        ft_putstr_fd(line, pipefd[1]);
        ft_putchar_fd('\n', pipefd[1]);
    }

    // 4. CRITICAL: Close write end so reader knows input is finished
    close(pipefd[1]);

    // 5. Handle the result
    if (mod == 1)
    {
        // This is the chosen input.
        // Close previous input fd if it exists to prevent leaks
        if (cmd->fd_in > 2)
            close(cmd->fd_in);
        cmd->fd_in = pipefd[0];
    }
    else
    {
        // This heredoc is superseded by a later one.
        // Close the read end, discarding the data.
        close(pipefd[0]);
    }
}
```
