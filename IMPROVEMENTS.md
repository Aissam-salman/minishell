# Minishell Robustness & Improvement Plan

This document outlines the changes required to transform the current `minishell` into a robust, standard-compliant shell that passes rigorous testing.

## 1. Environment Variable Management (Critical)

**Current Status:**
The code uses `getenv("PATH")` and passes `minishell->envs` (which is `char **envp` from `main`) directly to `execve`. It lacks a way to _modify_ the environment (required for `export`, `unset`, and updating `shlvl`).

**Required Changes:**
- [ ] **Create a custom environment structure:** Instead of using `char **envp` directly, copy it into a linked list (e.g., `t_env`) at startup.
- [ ] **Remove `getenv` usage:** Re-implement `getenv` to search your linked list instead of using the forbidden/unreliable system `getenv`.
- [ ] **Implement `env_to_tab`:** Create a function to convert your `t_env` list back into a `char **` array before calling `execve`.
- [ ] **Persist changes:** `export` and `unset` must modify this list.

**Why:**
`export MY_VAR=123` currently won't work because you can't modify the `envp` provided by `main` easily, and `getenv` only looks at the initial environment, not your modified one.

**Test Case:**
```bash
export TEST=123
env | grep TEST  # Should output TEST=123
export TEST=456
echo $TEST       # Should output 456
unset TEST
echo $TEST       # Should output empty line
```

## 2. Exit Status Management ($?)

**Current Status:**
`minishell->exit_status` exists but isn't consistently updated or used. `exec.c` calls `wait(&pid)` but ignores the result.

**Required Changes:**
- [ ] **Capture `waitpid` status:** In `ft_exec`, use the macro `WEXITSTATUS(status)` and `WTERMSIG(status)` to extract the exit code from the child process.
- [ ] **Update `minishell->exit_status`:** Store this value.
- [ ] **Implement `$?` expansion:** Modify your expander to replace `$?` with the last stored exit status.
- [ ] **Handle signals:** If a child is killed by a signal (e.g., Ctrl-C), the exit status should reflect that (typically `128 + signal_number`).

**Why:**
Shells communicate success/failure via exit codes. `ls file_that_doesnt_exist; echo $?` should print `ls` error and then `1` or `2`.

**Test Case:**
```bash
/bin/false
echo $?        # Should output 1
/bin/true
echo $?        # Should output 0
sleep 5        # Hit Cmd+C (Ctrl+C) during sleep
echo $?        # Should output 130
```

## 3. Signal Handling (Interactive vs. Execution)

**Current Status:**
Basic `SIGINT` handler exists. `SIGQUIT` is commented out.

**Required Changes:**
- [ ] **Split modes:** 
    - **Interactive Mode (empty prompt):** `SIGINT` (Ctrl+C) -> New prompt. `SIGQUIT` (Ctrl+\) -> Ignored.
    - **Execution Mode (child running):** `SIGINT` -> Child terminates (default), Parent ignores or handles status. `SIGQUIT` -> Child terminates (core dump/default), Parent prints "Quit (core dumped)".
- [ ] **Heredoc Signals:** `SIGINT` inside a heredoc must stop the heredoc input cleanly.

**Why:**
Ctrl+C inside `cat` should kill `cat`, not the shell. Ctrl+\ should do nothing in the prompt but kill `cat`.

**Test Case:**
1. Open minishell. Press Ctrl-C. Prompt should refresh.
2. Run `cat`. Press Ctrl-C. `cat` closes, prompt returns.
3. Run `cat`. Press Ctrl-\. `cat` quits (message "Quit"), prompt returns.

## 4. Parser & Expander Robustness

**Current Status:**
The `parser.c` and `expander.c` are fragile. Quote handling (`quote_mode` in expander) looks incomplete. Valid commands like `echo "hello $USER"` or `echo '$USER'` need precise handling.

**Required Changes:**
- [ ] **Rewrite `ft_check_expands`:** It currently barely handles quotes. It needs to:
    - Expand variables inside double quotes `""`.
    - NOT expand variables inside single quotes `''`.
    - Handle mixed quotes: `echo "'$USER'"` -> `'$USER'` (literal), `echo '"$USER"'` -> `"fardeau"` (expanded).
- [ ] **Handle edge cases:** Empty variables should disappear completely (unless quoted).

**Test Case:**
```bash
export A="ls -l"
$A              # Should run ls -l
export EMPTY=""
ls $EMPTY -la   # Should run ls -la, not ls "" -la (which errors)
```

## 5. Built-ins Implementation

**Current Status:**
No built-ins found in `exec.c`. It tries to `execve` everything.

**Required Changes:**
- [ ] **Implement Built-ins:** `cd`, `pwd`, `echo`, `export`, `unset`, `env`, `exit`.
- [ ] **Routing:** In `ft_exec` (or before forking), check if the command is a built-in.
    - If it's `cd`, `export` (with args), `unset`, `exit`: **Run in Parent**.
    - If it's `echo`, `pwd`, `env`: **Run in Child** (usually, for piping support) OR Parent (optimization).

**Why:**
`cd` in a child process changes the child's PWD, not the shell's.

**Test Case:**
```bash
cd ..
pwd      # Should show parent directory
exit 42  # Should close shell with code 42
```

## 6. Execution & Piping Logic

**Current Status:**
`ft_exec` handles one command with one fork. `pipe` logic is commented out.

**Required Changes:**
- [ ] **Implement Pipe Loop:** You need a loop to handle `cmd1 | cmd2 | cmd3`.
    - Pipe creation `pipe(fd)`.
    - `dup2` to redirect `STDOUT` of cmd1 to pipe, and `STDIN` of cmd2 to pipe.
    - Close unused FDs (Strictly!).
- [ ] **Redirections:** Handle `<`, `>`, `<<` (heredoc), `>>` **before** execution.

**Test Case:**
```bash
ls | cat -e
ls > outfile
cat < outfile
grep "something" << EOF
line 1
something
line 3
EOF
```

---
**Summary of Action:**
Start with **Environment** and **Built-ins**. Without them, you cannot properly test the rest of the logic.

# Detailed File-by-File Improvements

## `src/exec.c`
- **`ft_exec`**:
    - Calls `fork()` once. Needs a loop for multiple commands (pipelines).
    - Hardcoded `exit(1)` in child. Should be based on `execve` error (127 for command not found, 126 for permission denied).
    - `perror("EXEC\n")` is vague. Use `ft_error` with `strerror`.
    - `wait(&pid)` ignores the exit status. Needs `waitpid(pid, &status, 0)` and update `minishell->exit_status`.
    - Missing signals handling in child (restore default `SIGINT`, `SIGQUIT`).
    - Uses `minishell->envs` (array of strings) directly. Needs `env_to_tab` from custom list.

## `src/parsing/lexer.c`
- **`ft_tokenize` / `ft_token_create`**:
    - Generally okay, but check for unclosed quotes error handling.
    - Quote state logic might need refinement for edge cases like `echo "hello'world"`.

## `src/parsing/parser.c` & `src/utils/parser_utils.c`
- **`ft_quotes_handle`**:
    - **Buffer Limit:** uses `BUFFER_SIZE` (likely 4096 or similar). A large variable expansion will crash or truncate. needs dynamic allocation.
    - **Exit Status:** Uses `GENERAL_ERROR` macro for `$?` expansion. MUST use `minishell->exit_status`.
    - **Forbidden:** Uses `getenv` inside `ft_expend`.
- **`checker_token`**:
    - Iterates and handles tokens but structure is a bit loose.
    - `handle_redirection` checks validation but doesn't seem to prepare execution fully.

## `src/parsing/support/check2.c`
- **`ft_check_cmd`**:
    - **Forbidden:** Uses `getenv("PATH")`.
    - **Logic:** `stat` check is good. `access` check is good.
    - **Leak:** `envp` from `ft_split_sep_gc` might be allocated; ensure GC handles it.

## `src/parsing/support/filter.c`
- **`is_need_expands`**:
    - **Syntax Error:** Mismatched parentheses in condition: `token->str[0] == '\"'))`.
- **`ft_strdup_without`**:
    - **Double Allocation Leak:** Calls `ft_gc_malloc`, then ignores result and overwrites pointer with `ft_substr_gc`. The first malloc is leaked (though GC might catch it eventually, it's waste).

## `src/parsing/support/expander.c`
- **`ft_check_expands`**:
    - **Forbidden:** Uses `getenv`.
    - **Incomplete:** Only handles simple cases. Doesn't handle `$?`.
    - **Quote Handling:** `quote_mode` logic is too simple (binary on/off). Needs to respect single vs double quotes difference.

## `src/helpers/cntrl.c`
- **`ft_heredoc`**:
    - **Not Implemented:** Just prints "HEREDOC". Needs:
        - Random filename generation (e.g., `/tmp/.heredoc_123`).
        - `readline` loop.
        - Expansion logic inside heredoc (unless delimiter is quoted).
        - Signal handling (SIGINT stops heredoc).
- **`ft_redirection_handler`**:
    - **Error Handling:** `perror` prints to stderr but function continues. If open fails (fd -1), execution MUST stop for this command.
    - **Permissions:** `O_CREAT | O_TRUNC | 0644` is correct for `>`.
    - **Global State:** Needs to update `minishell->exit_status` on failure (exit code 1).

## `src/utils/cmds.c`
- **`ft_cmd_lst_create`**:
    - Generates command list from tokens.
    - Logic `while (tok_current && tok_current->type != PIPE)` suggests it builds one command at a time.
    - **Missing:** Needs to handle `pipe` tokens correct to separate commands in the list.

## `src/main.c`
- **Loop:**
    - `readline("foo$> ")` is fine.
    - `minishell.line == EOF` check is weird. `readline` returns `NULL` on EOF (Ctrl-D).
    - `ft_exit` calls `exit(0)`. Should exit with last status.
    - **Init:** Needs environment duplication here.

## General
- **Memory:** Extensive usage of `gc`. Ensure `ft_gc_free_all` is called in every exit path (including child processes).
- **Norm:** Many functions arguably > 25 lines or have commented-out code.
- **Headers:** Check for implicit declarations.

