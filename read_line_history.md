# Guide: GNU Readline & History for Minishell

This guide explains how to effectively use the `readline` and `history` libraries in your Minishell project. These libraries provide the "real shell" experience: command line editing and accessing previous commands with arrow keys.

## 1. Setup & Compilation

### Headers
You need two specific headers. `readline.h` handles the input, and `history.h` handles the storage of previous commands.

```c
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
```

### Makefile linking
The code won't compile unless you link against the library. Add `-lreadline` to your flags.

```makefile
LIBS = -lreadline
# ...
$(NAME): $(OBJS)
    $(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)
```

---

## 2. Core Functions & Usage

### 2.1 `readline`
**Purpose**: Reads a line from the terminal, allowing the user to move the cursor (left/right) and edit text before pressing Enter.
**Prototype**: `char *readline(const char *prompt);`

*   **Tip**: It allocates memory for the returned string. **You must free this string** after use.
*   **Return**: Returns the string typed by the user, or `NULL` if `Ctrl+D` (EOF) is pressed.

### 2.2 `add_history`
**Purpose**: Saves a string into the internal history buffer. This allows the user to press **Up/Down Arrow** keys later to recall it.
**Prototype**: `void add_history(const char *string);`

*   **Tip**: Only call this if you actually want the command saved. Typically, you avoid saving empty lines.

### 2.3 `rl_clear_history`
**Purpose**: Frees the memory used effectively by the history list.
**Prototype**: `void rl_clear_history(void);`

*   **Usage**: Call this right before your program `exit()`s to ensure Valgrind is happy and clean.

---

## 3. Signal Handling Trio (The "Smart" Way)

When you press `Ctrl+C` in a real shell, the current line is aborted, and a fresh prompt appears on a new line. To mimic this behavior in Minishell, you need these three functions inside your signal handler:

1.  **`rl_on_new_line()`**: Tells readline functions that the cursor has moved to a new line (essential after you print `\n`).
2.  **`rl_replace_line("", 0)`**: Clears the current text in the internal buffer (so the "ghost" of the cancelled command doesn't remain).
3.  **`rl_redisplay()`**: Forces readline to print the prompt and the (now empty) line again.

**Example Signal Handler:**
```c
void handle_sigint(int sig)
{
    (void)sig;
    printf("\n"); // Move to next line
    rl_on_new_line(); // Tell readline we are on a new line
    rl_replace_line("", 0); // Clear the previous text
    rl_redisplay(); // Show the new prompt
}
```

---

## 4. Smart Efficiency Tips

### Tip 1: Don't Spam History
Standard shells don't usually save empty commands (just pressing Enter).

**Bad:**
```c
line = readline("minishell> ");
add_history(line); // Adds empty lines to history
```

**Good:**
```c
line = readline("minishell> ");
if (line && *line) // Check if line exists and is not empty
    add_history(line);
```

### Tip 2: Avoid Consecutive Duplicates (Optional)
If a user types `ls` ten times, you might not want history filled with 10 `ls` entries. Check if the current line matches the previous one before adding.

### Tip 3: Valgrind & "Still Reachable"
Readline is known to have some "Still Reachable" memory at the end of execution. This is normal behavior for this library on many systems.
*   **Must Do**: Make sure you free the string returned by `readline` every loop.
*   **Must Do**: Call `rl_clear_history()` at exit.
*   **Ignore**: Some internal readline buffers that persist until the process dies.

---

## 5. Complete Example Pattern

Here is a robust structure for your main loop:

```c
int main(void)
{
    char *input;

    // Setup signals here (e.g., signal(SIGINT, handle_sigint))

    while (1)
    {
        input = readline("minishell$ ");
        
        // Handle Ctrl+D (EOF)
        if (!input)
        {
            printf("exit\n");
            break;
        }

        // Only process string if it has content
        if (input[0] != '\0')
        {
            add_history(input); // Add to history
            
            // ... Parse and Execute command ...
        }

        free(input); // IMPORTANT: Free the input every iteration
    }

    rl_clear_history(); // Clean up history memory
    return (0);
}
```
