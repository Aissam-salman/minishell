# REPORT 2 — Full Code Audit: Readability, Dead Code, Simplifications & Bash Compliance

*Generated on 2026-02-16 after full review of every `.c` and `.h` file in `src/`, `includes/`.*
*Sections are ordered by **execution flow**: headers → initialization → lexing → parsing → command construction → execution → built-ins → post-execution → utilities.*

---

## Table of Contents

### Headers & Definitions
1. [includes/minishell.h](#1-includesminishellh)
2. [includes/errors.h](#2-includeserrorsh)
3. [includes/testing.h](#3-includestestingh)

### Initialization
4. [src/main.c](#4-srcmainc)
5. [src/utils/env_setup.c](#5-srcutilsenv_setupc)
6. [src/signal/signal_core.c](#6-srcsignalsignal_corec)

### Lexing (Tokenization)
7. [src/parsing/lexer.c](#7-srcparsinglexerc)
8. [src/utils/tokens.c](#8-srcutilstokensc)

### Parsing & Validation
9. [src/parsing/parser.c](#9-srcparsingparserc)
10. [src/parsing/support/check.c](#10-srcparsingsupportcheckc)
11. [src/parsing/support/check2.c](#11-srcparsingsupportcheck2c)
12. [src/utils/parser_utils.c](#12-srcutilsparser_utilsc)
13. [src/parsing/support/expander.c](#13-srcparsingsupportexpanderc)
14. [src/parsing/support/filter.c](#14-srcparsingsupportfilterc)

### Command Construction
15. [src/utils/cmds.c](#15-srcutilscmdsc)
16. [src/utils/heredoc.c](#16-srcutilsheredocc)

### Execution
17. [src/helpers/cntrl.c](#17-srchelperscntrlc)
18. [src/execution/exec.c](#18-srcexecutionexecc)
19. [src/execution/child_exec.c](#19-srcexecutionchild_execc)
20. [src/execution/handler.c](#20-srcexecutionhandlerc)

### Built-in Commands
21. [src/built_in/is_built_in.c](#21-srcbuilt_inis_built_inc)
22. [src/built_in/echo.c](#22-srcbuilt_inechoc)
23. [src/built_in/cd.c](#23-srcbuilt_incdc)
24. [src/built_in/pwd.c](#24-srcbuilt_inpwdc)
25. [src/built_in/export.c](#25-srcbuilt_inexportc)
26. [src/built_in/unset.c](#26-srcbuilt_inunsetc)
27. [src/built_in/env.c](#27-srcbuilt_inenvc)
28. [src/built_in/exit.c](#28-srcbuilt_inexitc)

### Post-Execution & Utilities
29. [src/execution/wait.c](#29-srcexecutionwaitc)
30. [src/errors/errors.c](#30-srcerrorserrorsc)
31. [src/utils/output.c](#31-srcutilsoutputc)

### Reference
32. [Return Value Reference Table](#32-return-value-reference-table)

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- HEADERS & DEFINITIONS                              -->
<!-- ═══════════════════════════════════════════════════ -->

## 1. `includes/minishell.h`

### Inconsistent include formatting
Some `#include` directives use the 42 norm indentation (`# include`) while others don't (`#include`). Pick one style and stick to it. [x]

### Duplicate prototype declarations
The following functions are declared **twice** in the header:
- `handler_signal_child()` — once under `EXECUTION/EXEC.C` and once under `EXECUTION/HANDLER.C`.
- `handler_first_cmd()` — same.
- `handler_last_cmd()` — same.
- `close_pipe_and_exec()` — once under `EXEC.C` and once under `CHILD_EXEC.C`.
- `ft_state_detect()` / `ft_buffer_add()` / `ft_tokenize()` — declared twice under `PARSING/LEXER.C`.

**Action:** Remove all duplicate declarations. Each function should appear exactly once. [x]

### Stale/wrong prototype
```c
void ft_create_cmd_lst(t_minishell *minishell);
```
This prototype has signature `void` but the actual function `ft_cmd_lst_create` returns `int`. Also the name doesn't match. **Remove this line.** [x]

### Broken line in prototype
```c
int
_tokenize(t_minishell *minishell);
```
This is a linebreak typo — it should be `ft_tokenize`. **Fix or remove** (the correct prototype already exists below). [x]

### Unused struct member: `t_cmd.pipefd[2]`
The `pipefd[2]` field inside `t_cmd` is **never read or written** anywhere in the codebase. The pipe fds are passed as local arrays in `exec.c`. **Remove it.** [x]

### Unused field: `exit_status`
`minishell->exit_status` is declared but **never assigned or read** anywhere. This should be the `$?` value. It needs to be wired up in `ft_wait_subprocess` and in built-in return paths. [x]

### Unused enum value: `LAST_HEREDOC`
Never referenced anywhere. **Remove it.** [x]

### `SEPARATORS` macro
```c
# define SEPARATORS " $\'\"\t"
```
Only used in one place (`parser_utils.c`). This includes `$`, `'`, `"` — which are not separators in the shell sense. Consider renaming to `EXPAND_DELIMITERS` or similar to clarify intent.

### `BUFFER_SIZE 5000`
A fixed 5000-byte buffer is fragile. Consider dynamic allocation (already done via gc in some places). At minimum, document that this limits max input length.

### Tips
- Group prototypes by **actual file**, not by guessed file. Remove all section comments that are wrong.
- Add a brief one-line comment above each prototype group to explain what it does.
- Consider splitting this header into multiple headers (`parsing.h`, `execution.h`, `builtins.h`) once the project grows.

---

## 2. `includes/errors.h`

### Non-standard error code values
```c
SYNTAX_ERROR = 3,    // bash uses exit code 2 for syntax errors
PARSING_FAIL = 50,   // internal, OK
BUFFER_FAIL = 98,    // internal, OK
```
Bash returns **2** for syntax errors (e.g., `bash -c '>' ; echo $?` → `2`). Your `SYNTAX_ERROR = 3` will give wrong `$?`. [x]

### Missing error code: `MISUSE = 2`
Bash defines exit code **2** as "Misuse of shell builtins" (which covers syntax errors). Add it.

### Tips
- Make internal-only codes negative or very high (e.g., `PARSING_FAIL = -1`) so they can never leak as an actual exit status.

---

## 3. `includes/testing.h`

### Entirely stale file
Every prototype in this file is either **wrong** (signatures don't match actual functions) or **duplicate** of what's in `minishell.h`:
- `int ft_check_cmd(...)` → actual function returns `void`.
- `char *ft_check_expends(...)` → typo, actual name is `ft_check_expands`.
- All other prototypes are already in `minishell.h`.

This file is not included by any `.c` file in the Makefile's `SRCS`.

**Action:** Delete `testing.h` entirely or move it to a `tests/` directory if it's used by unit tests. [x]

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- INITIALIZATION                                     -->
<!-- ═══════════════════════════════════════════════════ -->

## 4. `src/main.c`

### Dead code: `if (*minishell.line == EOF)`
```c
if (*minishell.line == EOF)
    ft_printf("\n");
```
`readline` returns `NULL` on EOF (CTRL-D), which is already handled above. If `readline` returns a non-NULL string, `*minishell.line` is a normal `char` — it will never equal `EOF` (which is typically `-1`, and `char` is `0–127` or `0–255`). **Remove this block.** [x]

### `ft_minishell_init` is redundant with `ft_bzero`
`ft_bzero` already zeroes the entire struct at startup. Inside the loop, `ft_minishell_init` only resets `head_token` and `head_cmd`. This is fine, but the function name is misleading — it's a "reset per iteration", not a full init. Rename to `ft_minishell_reset`. /OK

### Missing `checker_token` error handling
After `checker_token(&minishell)`, the code never checks if any token has `code_error != 0`. Execution proceeds unconditionally. **Add a check:** [x]
```c
checker_token(&minishell);
if (ft_has_token_error(minishell.head_token))
    continue;
```

### Unreachable code after `while (1)`
```c
ft_gc_free_all(&minishell.gc);
ft_exit(&minishell, 0, NULL);
```
These lines are **never reached** because the `while (1)` loop has no `break`. The only exits are via `ft_buildin_exit`. **Remove or make the loop breakable.** [x]

### Tips
- The `(void)argc; (void)argv;` is fine for now, but bash rejects extra arguments in interactive mode. Consider validating `argc == 1`. [x]

---

## 5. `src/utils/env_setup.c`

### `ft_env_delone`: uses `==` to compare strings instead of `ft_strcmp`
```c
else if (head->name == target_name)
```
This compares **pointers**, not string content. If `target_name` is a different allocation with the same text, this will never match. Use `ft_strcmp`. [x]

### `ft_env_delone`: crash when list has one element
```c
head = *head_env;
nxt = head->next;          // could be NULL
while (head)
{
    // ...
    head = head->next;
    if (head->next)         // CRASH if head is NULL
        nxt = head->next;
```
If `head->next` is NULL at the end of the list, `head` becomes NULL, then `head->next` is a **NULL dereference**. [x]

### `ft_env_delone`: first-element check uses wrong comparison
```c
if (ft_strcmp(head->name, target_name) == 0 &&
    ft_strcmp(head->name, (*head_env)->name))  // ???
```
The second condition `ft_strcmp(head->name, (*head_env)->name)` checks if the current node is NOT the head. But in the first iteration `head == *head_env`, so this evaluates to `ft_strcmp(X, X)` which is **0 (false)**. This means the first element can **never** be deleted by this branch. The logic is inverted. [x]


### `ft_env_find`: uses `ft_strncmp` with `ft_strlen(to_find)` — prefix matching bug
```c
if (!ft_strncmp(head_env->name, to_find, ft_strlen(to_find)))
```
If you search for `"HOME"`, this also matches `"HOMEWORLD"` because it only compares the first 4 characters. Use `ft_strcmp` instead. [x]

### Tips
- Rewrite `ft_env_delone` from scratch — it has multiple interacting bugs.[x]

---

## 6. `src/signal/signal_core.c`

### Clean file, no major issues

### Minor: perror message copy-paste
```c
if (signal(SIGINT, SIG_DFL) == SIG_ERR)
    perror("signal error default SIGQUIT");  // Says SIGQUIT but handles SIGINT
```
This is in `handler.c`, not here, but the pattern shows up. In this file, all is correct. [x]

### Tips
- `SA_RESTART` flag means `readline` will restart after signal — this is correct behavior for SIGINT. [x]

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- LEXING (TOKENIZATION)                              -->
<!-- ═══════════════════════════════════════════════════ -->

## 7. `src/parsing/lexer.c`

### Redundant includes
```c
#include <stdio.h>
#include <stdlib.h>
```
Both are already included via `minishell.h`. **Remove.** [x]

### `ft_buffer_add`: re-inventing `ft_strlen` every call
```c
i = 0;
while (buffer[i])
    i++;
```
This is literally `ft_strlen(buffer)`. Replace with: [x]
```c
int len = ft_strlen(buffer);
```

### `ft_state_interpret`: three consecutive `if` blocks instead of `if/else if`
The function has:
```c
if (spaces && NORMAL) { ... }
if (operators && NORMAL) { ... }
if (state != WAITING) { ... }
```
The first `if` sets `state = WAITING`, then the third `if` (`state != WAITING`) may still fire in the same call — but now state *is* WAITING so it won't. This works by accident. Convert to proper `if/else if/else` for clarity and safety.

### `ft_token_lst_create`: `line[i] = '\0'` after loop is useless
```c
i = 0;
while (i < line_len)
    ...
    i++;
line[i] = '\0';  // i == line_len, but line[line_len] is already '\0'
```
`line` is a null-terminated string — `line[line_len]` is already `'\0'`. **Remove this line.** [x]

### `ft_type_affect`: can be simplified with a lookup approach
The if/else chain checks `str[0]` then `str[1]`. This is fine for 3 operators, but ensure that invalid combos like `<>` or `|>` are caught. Currently they are not. [ ] ???  |> exit 0

### Tips
- The lexer is the core of correctness. Consider adding a dedicated flush function instead of repeating the `ft_token_add(minishell, ft_token_create(minishell, buffer))` pattern 5 times.

---

## 8. `src/utils/tokens.c`

### Clean file, well-structured

### Minor: return values inconsistency
`ft_token_add` returns `0` on success and `1` on error. The rest of the codebase uses `SUCCESS` (0) and `GENERAL_ERROR` (1). Use the enums for consistency. [x]

### `ft_token_create`: buffer cleared inside creator
```c
ft_bzero(buffer, ft_strlen(buffer));
```
Side-effecting the caller's buffer inside a "create" function is surprising. Consider clearing the buffer in the caller instead, or document this behavior clearly. [x]

### Tips
- `ft_token_last` is a utility that could be shared with `ft_cmd_last` and `ft_env_last` via a generic pattern, since all three linked lists follow the same structure. [x]

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- PARSING & VALIDATION                               -->
<!-- ═══════════════════════════════════════════════════ -->

## 9. `src/parsing/parser.c`

### Dead code: commented-out `handle_expands`
```c
// void handle_expands(t_token *token, t_minishell *minishell)
// { ... }
```
**Remove entirely.** Quote/expand handling is now in `parser_utils.c`. [x]

### Dead code: commented-out lines inside `handle_word`
```c
// ft_filter_quote(token, minishell);
// if (ft_check_file(token) == 1)
//     token->type = R_FILE;
```
**Remove.** [x]

### `handle_word` promotes to FLAG after CMD 
```c
if (*cmd_find == 0)
{
    ft_check_cmd(minishell, token);
    token->type = CMD;
    *cmd_find = 1;
}
if (ft_check_flags(token->str) == 1)
    token->type = FLAG;
```
If the **first word** is a flag-like string (e.g., `-l`), it gets set to `CMD` then immediately overwritten to `FLAG`. The second `if` should be `else if` so that the command itself is never re-typed. [x]

### Magic number `301`
```c
token->code_error = 301;
```
No corresponding enum value. Use a named constant. [x]

### Tips
- `checker_token` should return an `int` (error code) so `main.c` can check it directly instead of scanning all tokens for `code_error`. [x]

---

## 10. `src/parsing/support/check.c`

### **COMPILATION ERROR**: `ft_check_redirection` is broken
```c
int ft_check_redirection(char *str)
{
    int i
    // ... entire body is commented out
}
```
Missing semicolon after `int i`, and the entire function body is commented out. This **will not compile**. Either restore the function body or provide a new implementation. [x]

### `ft_check_flags`: logic bug on first line
```c
if (str[i] != '-' || !str[i])
    return (0);
```
`i` is 0, so `!str[i]` checks if the string is empty. But the `||` means: if the first char is NOT `-` **OR** the string is empty. Since accessing `str[0]` when string is empty is the null terminator (safe but confusing), the check works — but it should be:
```c
if (!str[0] || str[0] != '-')
    return (0);
```
Check for empty **first**, then check the character. Clearer and safer. [x]

### `ft_check_heredoc_end`: does nothing useful
```c
int ft_check_heredoc_end(char *str)
{
    if (!str)
        return (0);
    return (1);
}
```
This function only checks if `str` is NULL. It's called from `ft_check_file_of_redirection`. This is essentially a NULL check — either inline it or give it a meaningful name. [x] remove

### `ft_check_file`: **never called**
```c
int ft_check_file(t_token *token)
```
All references to `ft_check_file` in the codebase are **commented out**. **Remove this function.** [x]

### `ft_check_file_of_redirection`: incomplete logic
Only handles `IN_CHEVRON` and `IN_DCHEVRON`. Does not handle `OUT_CHEVRON` or `OUT_DCHEVRON`. For output redirections, it silently returns 1 (OK) without any check. [x] remove unused

### Tips
- This file needs the most work. Fix the compilation error first.
- The `ft_check_redirection` function should validate operator strings — this is the core of syntax error detection.

---

## 11. `src/parsing/support/check2.c`

### `ft_get_path` uses `getenv("PATH")` instead of the internal env list
```c
env = getenv("PATH");
```
Your shell has its own env list (`minishell->head_env`). If the user does `export PATH=/new/path`, the internal list is updated but `getenv()` still returns the **original** system PATH. Use `ft_env_find(minishell->head_env, "PATH")` instead. [x]

### `ft_test_path`: redundant NULL check
```c
if (cur_path && access(cur_path, X_OK) == 0)
```
`cur_path` is the return value of `ft_strjoin_gc`. If `ft_strjoin_gc` returns NULL, you have bigger problems (malloc fail). The `access` call will just fail on NULL anyway. Not harmful, but unnecessary. [x]

### `ft_check_cmd`: unused variable `res`
```c
res = ft_test_path(minishell, envp, token);
if (res == 1)
    return;
```
Can be simplified to:
```c
if (ft_test_path(minishell, envp, token))
    return;
```
Or remove `res` entirely. [x]

### `ft_check_pipe`: trivially simplifiable
```c
int ft_check_pipe(char *str)
{
    if (!str)
        return (0);
    if (str[0] == '|' && !str[1])
        return (1);
    return (0);
}
```
Could be: [x]
```c
int ft_check_pipe(char *str)
{
    return (str && str[0] == '|' && !str[1]);
}
```

### Tips
- `ft_check_cmd` should signal "not found" somehow (return value, error flag) instead of silently leaving `token->path` as NULL. This is the root cause of Bug #7 from REPORT1.

---

## 12. `src/utils/parser_utils.c`

### `ft_expend`: crash when env variable not found
```c
path_env = ft_env_find(minishell->head_env, buffer);
if (path_env->content)  // CRASH: path_env can be NULL
```
`ft_env_find` returns NULL if the variable doesn't exist. Dereferencing NULL → **SEGV**. [x]

**Fix:** 
```c
path_env = ft_env_find(minishell->head_env, buffer);
if (path_env && path_env->content)
```

### `ft_expend`: `$?` uses hardcoded `GENERAL_ERROR` instead of actual exit status
```c
err_value = ft_itoa_gc(GENERAL_ERROR, &minishell->gc); [x]
```
This always returns `"1"`. Should use `minishell->exit_status` (which itself needs to be wired up — see minishell.h section).

### `ft_quotes_handle`: complex nested conditions
The block at lines 84–99 has three `else if` branches for deciding when to add a character. This can be simplified with a helper function `should_add_char(c, state)`.

### Function name: `ft_expend` → should be `ft_expand`
Typo in the function name.

### Tips
- This is the most complex parsing function. Add unit tests for edge cases: `$NONEXISTENT`, `$?`, `"$HOME"`, `'$HOME'`, `$`, `$$`.

---

## 13. `src/parsing/support/expander.c`

### **Entirely dead code**: `ft_check_expands` is never called
All calls to `ft_check_expands` in the codebase are **commented out** (in `parser.c`'s `handle_expands`). The expansion logic has been moved to `parser_utils.c` (`ft_quotes_handle` / `ft_expend`). [x]

**Action:** Delete this entire file and remove it from the Makefile.

### Bugs in the dead code (for reference)
- `quote_mode` is used uninitialized if no quotes are found.
- `end++` after the `while` loop skips one extra character for no reason.

### Tips
- Keep only one expansion path. Right now `parser_utils.c` does the real work.

---

## 14. `src/parsing/support/filter.c`

### **Entirely dead code**: both functions are never called
- `is_need_expands()` — never called from any active code.
- `ft_filter_quote()` — all call sites are commented out in `parser.c`. 

**Action:** Delete this entire file and remove it from the Makefile. [x]

### `ft_strdup_without`: double allocation waste
```c
clear_word = ft_gc_malloc(ft_strlen(str) - 2 + 1, &minishell->gc);  // allocation 1
clear_word = ft_substr_gc(str, 1, ft_strlen(str) - 2, &minishell->gc);  // allocation 2 (overwrites pointer)
```
The first `ft_gc_malloc` is immediately leaked because `clear_word` is reassigned. **Remove the first allocation.** [x] i remove it

### Tips
- If you re-enable quote filtering in the future, fix the double allocation.

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- COMMAND CONSTRUCTION                               -->
<!-- ═══════════════════════════════════════════════════ -->

## 15. `src/utils/cmds.c`

### Massive commented-out code blocks
At least 15 lines of commented-out code throughout the file. **Remove all of them.** [x]

### `ft_cmd_add(minishell, NULL)` at end of `ft_cmd_lst_create`
```c
ft_cmd_add(minishell, NULL);
```
This call returns `GENERAL_ERROR` because `to_add` is NULL (the function's guard: `if (!minishell || !to_add) return GENERAL_ERROR`). The return value is silently ignored. This line does **nothing**. **Remove it.**  [ ] ?? 

### `ft_token_affect`: unreachable code after `return (SUCCESS)`
```c
return (SUCCESS);

// SI OUTFILE => REMPLIR OUTFD
// { ... }
// SI INFILE => REMPLIR INFD
// else if (token->type == INFILE)
```
Everything after `return` is dead. **Remove.** [ ] ??

### `ft_token_word_count`: counts from 1 without clarity
```c
count = 1;  // "COMMENCE A 1 POUR CMD->ARGS[0]"
```
This is correct (args[0] is the command name) but fragile. If the loop also counts `CMD` tokens, you're double-counting. Currently `CMD` type is set *after* this function runs, so it works — but it's a time bomb. [ ] ??

### Tips
- `ft_token_affect` is too long (60+ lines). Split into helper functions: `ft_affect_word`, `ft_affect_cmd`, `ft_affect_redir`, `ft_affect_heredoc`.
- Use `else if` chains consistently instead of independent `if` blocks. [ ]

---

## 16. `src/utils/heredoc.c`

### `ft_heredoc`: CTRL-D (NULL from readline) causes infinite loop
```c
line = readline("> ");
if (!line)
    continue;  // CTRL-D → NULL → loops forever
```
Bash exits the heredoc on CTRL-D with a warning. **Fix:** [x]
```c
if (!line)
    break;
```

### `ft_heredoc_find_last`: has debug print left in
```c
ft_printf("TEST HEREDOC : %s\n", token->next->str);
```
**Remove** — this prints to stdout and corrupts command output. [ ]

### Tips
- Heredoc should be handled **before** execution (during parsing), not during cmd list creation. In your current flow, the heredoc `readline` call blocks inside `ft_cmd_lst_create`, which means signals during heredoc input are not properly handled. [ ]

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- EXECUTION                                          -->
<!-- ═══════════════════════════════════════════════════ -->

## 17. `src/helpers/cntrl.c`

### `ft_redirection_handler`: heredoc branch can never succeed
```c
if (token->type != IN_DCHEVRON)
    fd = ft_open(token->next->str, token->type);
else if (token->type == IN_DCHEVRON)
    ft_heredoc_handle(minishell, cmd, token);
if (fd == -1)                    // fd is still -1 after heredoc!
{
    perror(token->path);         // token->path is likely NULL for redirections
    return (GENERAL_ERROR);
}
```
After the heredoc branch, `fd` is still `-1` (its initial value), so the `if (fd == -1)` check fires and returns an error **even though the heredoc succeeded**. [ ] ??

But `ft_token_affect` in `cmds.c` handles `IN_DCHEVRON` separately and never calls `ft_redirection_handler` for heredocs. So this branch is **dead code** inside `ft_redirection_handler`. **Remove the heredoc branch** from this function. [ ] ??

### `perror(token->path)` — wrong field
For redirections, `token->path` is NULL (path is only set for commands). Should be `token->next->str` (the filename). [x]

### Tips
- `ft_open` is clean and simple. Keep it.

---

## 18. `src/execution/exec.c`

### `pipefd_set`: unnecessary function
```c
static void pipefd_set(int pipe_fd[2])
{
    pipe_fd[0] = -1;
    pipe_fd[1] = -1;
}
```
This can be done inline: `int pipe_fd[2] = {-1, -1};`. **Remove the function.** [x]

### `ft_pipe_and_fork`: pipe created even for single external commands
When `size_cmd == 1`, a pipe is created but both ends are closed in the child. Wasteful. Add `if (size_cmd > 1 || i < size_cmd - 1)` guard around `pipe()`.

### `ft_pipe_and_fork`: leaks `prev_pipe` after loop
After the while loop, `prev_pipe` holds the read-end of the last pipe. It's never closed in the parent. **Add:** [x]
```c
if (prev_pipe != -1)
    close(prev_pipe);
```

### Tips
- The `signal(SIGINT, SIG_IGN)` in `parent_process` is correct — parent should ignore SIGINT while child runs. But `setup_signal` should be re-called after `ft_wait_subprocess` to restore readline handling. Currently this happens at the top of the next loop iteration, which works. [x]

---

## 19. `src/execution/child_exec.c`

### `close_pipe_and_exec`: piped built-in exits with `exit(1)`
```c
run_built_in_piped(cmd, minishell);
exit(1);
```
Should be `exit(0)` on success. (Already noted in REPORT1.) [x]

### `ft_child_new`: uses `sizeof(struct s_child)` instead of `sizeof(t_child)`
Both work, but `sizeof(t_child)` is more idiomatic and consistent with the rest of the codebase. [x]

### Tips
- `child` struct is allocated via gc but only one instance exists (reused across iterations via `child_set`). This is fine, but the gc allocation is slightly wasteful for a single small struct — a stack variable would suffice.

---

## 20. `src/execution/handler.c`

### `handler_signal_child`: perror message says SIGQUIT for SIGINT [x]

```c
if (signal(SIGINT, SIG_DFL) == SIG_ERR)
    perror("signal error default SIGQUIT");  // Should say SIGINT ```

### `handler_last_cmd`: only closes `prev_pipe` inside the `outfd != 1` branch
```c
if (outfd != 1)
{
    dup2(outfd, STDOUT_FILENO);
    close(prev_pipe);
    close(outfd);
}
```
If `outfd == 1` (no redirection), `prev_pipe` is **never closed**. Move `close(prev_pipe)` **before** the if block. [x]

### Tips
- All handler functions are small and focused — good. Just fix the fd leak.

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- BUILT-IN COMMANDS                                  -->
<!-- ═══════════════════════════════════════════════════ -->

## 21. `src/built_in/is_built_in.c`

### `is_built_in`: checks `cmd->args[0]` before `cmd->args`
```c
if (!cmd->args[0] || !cmd->args)
```
Dereferences `cmd->args` (via `cmd->args[0]`) **before** checking if `cmd->args` is NULL. Should be: [x]
```c
if (!cmd->args || !cmd->args[0])
```

### `run_built_in` and `run_built_in_piped`: massive code duplication
These two functions are 90% identical. Extract the common dispatch into a single function, with a parameter to control the differences (e.g., whether `exit`/`export`/`unset` should run). [x]

### `run_built_in`: inconsistent alignment
Some `ft_strcmp` lines are indented differently. Align all conditions. [x]

### `run_built_in_piped`: missing `exit`/`export`/`unset`
These built-ins are omitted from the piped version. In bash:
- `exit` in a pipe subshell only exits that subshell ✅ (correct to omit from parent-side, but should work in child) [x]
- `export`/`unset` in a pipe have no effect on the parent ✅ (but should still execute without error in the child) [x]

### Tips
- Create a function pointer table indexed by built-in name to avoid the `ft_strcmp` chain.

---

## 22. `src/built_in/echo.c`

### Incorrect `echo` behavior — newline after every argument
```c
while (args[i])
{
    write(1, args[i], ft_strlen(args[i]));
    write(1, "\n", 1);  // WRONG: should be space between args, newline at end
    i++;
}
```
Bash `echo hello world` prints `hello world\n`, not `hello\nworld\n`.

**Fix:** [x]
```c
i = 1;
while (args[i])
{
    write(1, args[i], ft_strlen(args[i]));
    if (args[i + 1])
        write(1, " ", 1);
    i++;
}
if (!have_flag)
    write(1, "\n", 1);
```

### `echo -n` mode also has no spaces between arguments
Same issue for the `-n` branch.

### `echo` with no args: bash prints a single newline
Currently, if `args[1]` is NULL, nothing is printed. Bash does: `echo` → `\n`. **Add a newline if no args and no `-n` flag.** [x]

### Tips
- `-n` flag detection should handle multiple `-n` args: `echo -n -n hello` is valid in bash.

---

## 23. `src/built_in/cd.c`

### `save_pwd`: returns `malloc`'d memory — memory leak risk
`getcwd(NULL, 0)` calls `malloc`. The result is `free()`d in most paths but could be missed on error paths in `ft_cd`.

### `ft_cd`: `cd -` always prints pwd
```c
if (ft_strcmp(path, "-") == 0)
{
    // ... chdir ...
    ft_pwd();   // prints current directory
    return;
}
```
In bash, `cd -` prints the **old** directory (the one you're going TO), not the current one. Functionally this is close since by the time `ft_pwd` runs, you've already chdir'd. But the `ft_pwd` call should only happen for `cd -`, which it does — this is correct. [x]

### `update_old_pwd`: doesn't add to env if OLDPWD doesn't exist
```c
env_old_pwd = ft_env_find(*head_env, "OLDPWD");
if (!env_old_pwd)
    env_old_pwd = ft_env_new(minishell, "OLDPWD");
```
The new node is created but **never added to the list**. The `ft_env_add` call is missing. So if OLDPWD didn't exist before, it's created, modified, and then lost. [x]

### Tips
- Factor out the `free(old_pwd)` calls — there are 3 different paths that each `free` it. [x]

---

## 24. `src/built_in/pwd.c`

### Uses `printf` instead of `ft_printf` or `write`
```c
printf("%s\n", buff);
```
Inconsistent with the rest of the codebase. Use `ft_putstr_fd(buff, STDOUT_FILENO)` + `ft_putchar_fd('\n', STDOUT_FILENO)`. [x]

### Tips
- Clean and simple otherwise.

---

## 25. `src/built_in/export.c`

### `ft_env_format_check`: debug print left in
```c
ft_printf("format: %s\n", str);
```
**Remove** — this prints to stdout and corrupts output.

### `ft_env_format_check`: incomplete validation
Only checks first character. In bash, identifiers can contain `_` and must not start with a digit. The check `!ft_isalpha(str[0]) || str[0] == '-'` is redundant (`'-'` is not alpha anyway). [x]

**Correct validation:**
```c
if (!ft_isalpha(str[0]) && str[0] != '_')
    return (GENERAL_ERROR);
```

### `ft_export` with no args: should print with `declare -x` prefix
Bash output of `export` is:
```
declare -x HOME="/home/user"
declare -x PATH="/usr/bin:..."
```
Your implementation calls `ft_env_print` which prints `HOME=/home/user`. **Use a different format.** [x]

### `ft_export`: doesn't check if variable already exists
```c
new = ft_env_new(minishell, str);
ft_env_add(minishell, new);
```
If the variable already exists, this creates a **duplicate**. Should find and update the existing entry first. [x]

### Tips
- Export without `=` (e.g., `export FOO`) should mark the variable as exported but with no value. Currently it creates a node with `content = NULL`, which is close but `env` should not print it. [x]

---

## 26. `src/built_in/unset.c`

### Trivial wrapper, delegates to `ft_env_delone`
The function itself is fine, but `ft_env_delone` (in `env_setup.c`) is broken — see that section.

### Missing: multiple argument support
`unset VAR1 VAR2` should unset both. Currently only `cmd->args[1]` is used, ignoring any further arguments. [x]

### Tips
- Loop over all arguments: `int i = 1; while (cmd->args[i]) ft_unset(&head_env, cmd->args[i++]);`

---

## 27. `src/built_in/env.c`

### Trivial wrapper function
```c
void ft_env(t_env *head_env, int outfd)
{
    ft_env_print(head_env, outfd);
}
```
This is a one-line wrapper. Consider inlining it or keeping it if you plan to add `env` argument handling later.

### Missing: `env` should only print variables that have a value
In bash, `env` only prints `NAME=VALUE` pairs. If an env variable was `export`ed without a value, it should NOT appear in `env` output (but should appear in `export` output). [x]

### Tips
- `env` with arguments (e.g., `env ls`) should print an error or run the command in a modified environment. For minishell, you can ignore this.

---

## 28. `src/built_in/exit.c`

### Non-standard include path
```c
#include "includes/ft_conversion.h"
```
This works but is inconsistent with the rest of the codebase which uses `../../includes/minishell.h` (and `minishell.h` includes `libft.h` which includes `ft_conversion.h`). **Remove this line** — `ft_atoi` is already available via `minishell.h`.

### `ft_buildin_exit`: no validation of argument
Bash handles these cases:
- `exit` → exit with status of last command
- `exit 42` → exit with code 42
- `exit abc` → `bash: exit: abc: numeric argument required` → exit 2
- `exit 1 2` → `bash: exit: too many arguments` → does NOT exit

Currently your code does `ft_atoi(code_exit)` which silently returns 0 for non-numeric strings. **Add validation.**

### `ft_buildin_exit`: duplicates cleanup from `ft_exit`
```c
ft_gc_free_all(&minishell->gc);
rl_clear_history();
```
This is the same cleanup as `ft_exit`. Consider calling `ft_exit` instead.

### Tips
- Exit status should be `code % 256` (bash wraps it). `ft_atoi` doesn't handle this.

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- POST-EXECUTION & UTILITIES                         -->
<!-- ═══════════════════════════════════════════════════ -->

## 29. `src/execution/wait.c`

### `handler_status`: logic bug with `WIFEXITED` inside `WIFSIGNALED`
```c
if (WIFSIGNALED(status))
{
    // ...
    if (WIFEXITED(status) > 0)  // This is ALWAYS false here
        ft_printf("Error: %s\n", strerror(status));
}
```
`WIFSIGNALED` and `WIFEXITED` are **mutually exclusive**. A process either exited normally OR was killed by a signal, never both. This `WIFEXITED` check inside the `WIFSIGNALED` block will **never** be true. **Remove it.**

### `handler_status`: `strerror(status)` is wrong
`strerror` takes an `errno` value, not a `waitpid` status. Use `WTERMSIG(status)` to get the signal number, then `strsignal(sig)`.

### `handler_status`: accessing `cmd->args[1]` without NULL check
```c
ft_printf(" %s %s\n", cmd->args[0], cmd->args[1]);
```
If the command has no arguments (e.g., `sleep` killed by signal), `args[1]` is NULL → undefined behavior with `ft_printf`.

### `ft_wait_subprocess`: variable declaration inside block
```c
int i = 0;
```
42 norm requires declarations at the top of the function. Move it up.

### `ft_wait_subprocess`: doesn't save exit status
The exit status of the **last** command should be saved to `minishell->exit_status` for `$?` support.

### Tips
- For proper `$?` support: `if (WIFEXITED(status)) minishell->exit_status = WEXITSTATUS(status); else if (WIFSIGNALED(status)) minishell->exit_status = 128 + WTERMSIG(status);`

---

## 30. `src/errors/errors.c`

### `ft_error`: commented-out code noise
4 lines of commented-out code at the top. **Remove.**

### `ft_exit`: always prints newline even without error
```c
if (str)
{
    ft_putstr_fd(str, STDERR_FILENO);
    ft_putchar_fd('\n', STDERR_FILENO);
}
```
If `str` is NULL, the function calls `exit()` silently, which is correct. But previously (before your recent edit) it always printed a newline. Make sure the version you use is the clean one.

### Tips
- Consider making `ft_error` return the error code consistently so callers can do `return (ft_error(...))` in one line (already done in most places — good).

---

## 31. `src/utils/output.c`

### `ft_cmd_print`: bug — iterates `current` but prints `head`
```c
while (current)
{
    // Uses head->path, head->args, head->outfd, head->infd
    current = current->next;
}
```
It iterates via `current` but always prints the **first** command (`head`). Should use `current` everywhere.

### `ft_cmd_print`, `ft_tokens_print`, `ft_type_print`, `ft_state_print`: **debug-only functions, never called in production**
All call sites are commented out. These are useful for development but should be:
1. Wrapped in `#ifdef DEBUG` guards, or
2. Moved to a separate `debug.c` file.

### `ft_type_print`: doesn't handle all enum values
Missing: `CMD`, `FLAG`, `LAST_HEREDOC`, `DEFAULT` (partially handled), `NBR_TYPES`.

### Tips
- `ft_env_print` IS used (by `ft_env` and `ft_export`) — keep it.
- Consider removing all other print functions from the Makefile for production builds.

---

<!-- ═══════════════════════════════════════════════════ -->
<!-- REFERENCE                                          -->
<!-- ═══════════════════════════════════════════════════ -->

## 32. Return Value Reference Table

These are the exit codes that bash uses and that your minishell should match for `$?`:

| Exit Code | Meaning | When to use |
|-----------|---------|-------------|
| **0** | Success | Command completed successfully |
| **1** | General error | Catchall for general errors (e.g., `export ''`, division by zero) |
| **2** | Misuse of shell builtin / Syntax error | `bash: syntax error near unexpected token`, invalid `exit` argument |
| **126** | Permission denied | Command found but not executable (`chmod -x script.sh; ./script.sh`) |
| **127** | Command not found | Command doesn't exist in PATH (`nonexistent_command`) |
| **128** | Invalid exit argument | `exit` with out-of-range value (rarely used directly) |
| **128 + N** | Fatal signal N | Process killed by signal N (e.g., `128 + 2 = 130` for SIGINT, `128 + 3 = 131` for SIGQUIT, `128 + 9 = 137` for SIGKILL) |
| **130** | SIGINT (Ctrl-C) | `128 + 2` — process interrupted |
| **131** | SIGQUIT (Ctrl-\\) | `128 + 3` — process quit with core dump |
| **255** | Exit code out of range | `exit -1` → wraps to 255 (`-1 % 256 = 255` unsigned) |

### Internal error codes (your `errors.h`) vs bash exit codes:

| Your Code | Your Name | Correct bash equivalent | Action needed |
|-----------|-----------|------------------------|---------------|
| `0` | `SUCCESS` | `0` | ✅ Correct |
| `1` | `GENERAL_ERROR` | `1` | ✅ Correct |
| `2` | `NO_SUCH_FILE_O_DIR` | Depends: `127` if command, `1` if file arg | ⚠️ Context-dependent |
| `3` | `SYNTAX_ERROR` | **`2`** | ❌ **Change to 2** |
| `50` | `PARSING_FAIL` | N/A (internal) | ⚠️ Must never leak as exit code |
| `98` | `BUFFER_FAIL` | N/A (internal) | ⚠️ Must never leak as exit code |
| `99` | `MALLOC_FAIL` | N/A (internal) | ⚠️ Must never leak as exit code |
| `126` | `PERMISSION_DENIED` | `126` | ✅ Correct |
| `127` | `CMD_NOT_FOUND` | `127` | ✅ Correct |
| `128` | `SIGNAL` | `128` | ✅ Correct (base value) |

### Per-builtin expected return values:

| Built-in | Success | Failure cases |
|----------|---------|---------------|
| `echo` | `0` | `1` if write error |
| `cd` | `0` | `1` if directory not found, `1` if too many args |
| `pwd` | `0` | `1` if getcwd fails |
| `export` | `0` | `1` if invalid identifier |
| `unset` | `0` | Always `0` (even if var doesn't exist) |
| `env` | `0` | `0` (no error cases in minishell) |
| `exit` | N/A (exits) | `2` if non-numeric arg (but still exits), does NOT exit if too many args (returns `1`) |

---

## Summary of Dead / Unused Code to Remove

| File | What to remove | Reason |
|------|---------------|--------|
| `includes/testing.h` | Entire file | Stale, wrong prototypes, not included anywhere |
| `src/parsing/support/expander.c` | Entire file | `ft_check_expands` never called, replaced by `parser_utils.c` |
| `src/parsing/support/filter.c` | Entire file | `ft_filter_quote` and `is_need_expands` never called |
| `src/parsing/support/check.c` | `ft_check_file()` | Never called (all call sites commented out) |
| `src/utils/cmds.c` | `ft_cmd_add(minishell, NULL)` call | Does nothing (returns error, ignored) |
| `includes/minishell.h` | `ft_create_cmd_lst` prototype | Wrong name, wrong return type |
| `includes/minishell.h` | `int\n_tokenize(...)` broken prototype | Linebreak typo |
| `includes/minishell.h` | All duplicate prototypes | Listed in section 1 |
| `t_cmd.pipefd[2]` | Struct field | Never used |
| `t_types.LAST_HEREDOC` | Enum value | Never used |
| `src/main.c` | `if (*minishell.line == EOF)` block | Dead condition |
| `src/main.c` | Code after `while(1)` | Unreachable |
| All files | Commented-out code blocks | ~50 lines total |

---

*End of report. No files were modified.*
