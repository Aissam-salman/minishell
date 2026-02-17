# REPORT 1 — Why `sleep` and Pipes Don't Work Properly

## Executive Summary

After a full review of the execution pipeline (`exec.c`, `child_exec.c`, `handler.c`, `wait.c`, `is_built_in.c`, `check2.c`, `cmds.c`), **7 bugs** were identified — 4 critical, 2 moderate, and 1 minor. Together, they explain why `sleep` appears to not execute, why multi-stage pipelines break, why heredoc + pipe combos crash, and why unknown commands cause a fatal exit.

---

## Bug #1 — CRITICAL: Single external commands are never waited on (CHECK)

### File: `src/execution/exec.c` — `ft_exec()`, line ~84

```c
ft_pipe_and_fork(minishell, size_cmd, pipe_fd, pids);
if (!is_built_in(minishell->head_cmd) && size_cmd > 1)  // <-- BUG
    ft_wait_subprocess(minishell, size_cmd, pids);
```

### Problem

The condition `size_cmd > 1` means `ft_wait_subprocess` is **only** called for pipelines (2+ commands). For a **single external command** like `sleep 3`:

1. The child is `fork()`ed and calls `execv("sleep", ...)` — it **does** run.
2. But the parent **never calls `waitpid()`**, so it immediately returns to the `readline` prompt.
3. The user sees the prompt reappear instantly, as if `sleep` was never executed.
4. The child finishes 3 seconds later in the background and becomes a **zombie process**.

### Impact

- **Every single external command** (`sleep`, `ls`, `cat`, `grep`, etc.) appears to "not work" because the shell doesn't wait for it.
- Zombie processes accumulate with each command.

### Fix

```c
ft_pipe_and_fork(minishell, size_cmd, pipe_fd, pids);
if (!is_built_in(minishell->head_cmd) || size_cmd > 1)
    ft_wait_subprocess(minishell, size_cmd, pids);
```

Change `&&` to `||`. The shell should wait whenever:
- The command is **not** a single builtin, **OR**
- There is more than one command (pipeline).

More precisely, the built-in-only shortcut already runs synchronously in the parent (no fork), so the real condition should be: **wait whenever you forked children**.

---

## Bug #2 — CRITICAL: `handler_mid_cmd` redirects pipe to STDIN instead of STDOUT (CHECK)

### File: `src/execution/handler.c` — `handler_mid_cmd()`, line ~50

```c
void handler_mid_cmd(int prev_pipe, int pipe_fd)
{
    dup2(prev_pipe, STDIN_FILENO);
    close(prev_pipe);
    dup2(pipe_fd, STDIN_FILENO);  // <-- BUG: should be STDOUT_FILENO
}
```

### Problem

For a middle command in a pipeline (e.g., `cmd2` in `cmd1 | cmd2 | cmd3`):
- `prev_pipe` (read-end from the previous pipe) is correctly redirected to **STDIN** ✅
- `pipe_fd` (write-end of the next pipe) should be redirected to **STDOUT**, but it's redirected to **STDIN** ❌

This means the middle command:
- Reads from the **write-end** of the next pipe instead of writing to it.
- Loses its connection to the previous command's output.
- Cannot send output to the next command.

### Impact

**Any pipeline with 3+ commands is completely broken.** For example:

```
cat file | grep foo | wc -l
```

`grep` will read from the wrong fd and its output will never reach `wc`.

### Fix

```c
void handler_mid_cmd(int prev_pipe, int pipe_fd)
{
    dup2(prev_pipe, STDIN_FILENO);
    close(prev_pipe);
    dup2(pipe_fd, STDOUT_FILENO);  // FIXED: output goes to next pipe
}
```

---

## Bug #3 — MODERATE: `handler_last_cmd` leaks `prev_pipe` file descriptor

### File: `src/execution/handler.c` — `handler_last_cmd()`, line ~40

```c
void handler_last_cmd(int prev_pipe, int outfd)
{
    dup2(prev_pipe, STDIN_FILENO);
    // Missing: close(prev_pipe);  <-- BUG
    if (outfd != 1)
    {
        dup2(outfd, STDOUT_FILENO);
        close(outfd);
    }
}
```

### Problem

After `dup2(prev_pipe, STDIN_FILENO)`, the original `prev_pipe` fd is still open. It's now a duplicate reference to the pipe's read-end. This means:
- The pipe's read-end has **two** open file descriptors in this child.
- If the writing end is closed, EOF propagation may be delayed because the extra fd still holds a reference to the pipe.
- It's a file descriptor leak (though `execv` will inherit it).

### Fix

```c
void handler_last_cmd(int prev_pipe, int outfd)
{
    dup2(prev_pipe, STDIN_FILENO);
    close(prev_pipe);  // FIXED: close the original fd after dup2
    if (outfd != 1)
    {
        dup2(outfd, STDOUT_FILENO);
        close(outfd);
    }
}
```

---

## Bug #4 — MODERATE: Parent leaks last `prev_pipe` after the fork loop

### File: `src/execution/exec.c` — `ft_pipe_and_fork()`, end of while loop

```c
while (i < size_cmd)
{
    // ... fork, parent_process updates prev_pipe ...
    i++;
}
// prev_pipe is still open here! <-- BUG
```

### Problem

After the loop ends, `prev_pipe` holds the **read-end** of the last pipe created. The parent never closes it. This means:
- The pipe's read-end stays open in the parent for the rest of the shell's lifetime (until the next command iteration or exit).
- For the last child, the write-end of that pipe was already closed, but the parent still holds the read-end.
- This is a file descriptor leak that accumulates with every pipeline execution.

### Fix

Add after the while loop:

```c
while (i < size_cmd)
{
    // ... existing code ...
    i++;
}
if (prev_pipe != -1)
    close(prev_pipe);
```

---

## Bug #5 — MINOR: Built-in piped commands exit with status 1

### File: `src/execution/child_exec.c` — `close_pipe_and_exec()`, line ~44

```c
if (is_built_in(cmd) == 1)
{
    run_built_in_piped(cmd, minishell);
    exit(1);  // <-- BUG: should be exit(0) on success
}
```

### Problem

When a built-in runs in a pipeline (e.g., `echo hello | cat`), the child always exits with status **1** (error), even on success. The parent's `ft_wait_subprocess` will see a non-zero exit status, which would set `$?` incorrectly.

### Fix

```c
if (is_built_in(cmd) == 1)
{
    run_built_in_piped(cmd, minishell);
    exit(0);  // FIXED: success exit
}
```

---

## Bug #6 — CRITICAL: `echo` with no arguments crashes (SEGV / NULL dereference)

### File: `src/built_in/is_built_in.c` — `run_built_in()` and `run_built_in_piped()`

```c
// In run_built_in():
else if (ft_strcmp(str, "echo") == 0)
    ft_echo(cmd->args, ft_strcmp(cmd->args[1], "-n") == 0);

// In run_built_in_piped():
if (ft_strcmp(str, "echo") == 0)
    ft_echo(cmd->args, ft_strcmp(cmd->args[1], "-n") == 0);
```

### Problem

When `echo` is invoked **without any arguments** (e.g. in the pipeline `cat << EOF | wc -l | echo | sleep 3`), `cmd->args` is `{"echo", NULL}`. The expression `ft_strcmp(cmd->args[1], "-n")` dereferences `cmd->args[1]` which is `NULL` → **SEGV**.

This is exactly the crash observed:

```
AddressSanitizer:DEADLYSIGNAL
==565511==ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000
==565511==The signal is caused by a READ memory access.
==565511==Hint: address points to the zero page.
```

The child process running `echo` in the pipe crashes before it can produce any output or exit.

### How to Reproduce

```bash
foo$> echo
# crashes
foo$> cat << EOF | wc -l | echo | sleep 3
# crashes in the echo child
```

### Fix

Guard `cmd->args[1]` before passing it to `ft_strcmp`:

```c
// In BOTH run_built_in() and run_built_in_piped():
else if (ft_strcmp(str, "echo") == 0)
    ft_echo(cmd->args, cmd->args[1] && ft_strcmp(cmd->args[1], "-n") == 0);
```

The short-circuit `&&` ensures `ft_strcmp` is only called when `args[1]` is non-NULL.

---

## Bug #7 — CRITICAL: Unknown commands are never validated — shell calls `execv` on invalid paths and dies

### Files: `src/parsing/support/check2.c` — `ft_check_cmd()`, `src/utils/cmds.c` — `ft_cmd_lst_create()`, `src/execution/child_exec.c` — `close_pipe_and_exec()`

```c
// check2.c — ft_check_cmd:
// If the command is not found in PATH, the function simply returns
// without setting token->path. token->path stays NULL.

// cmds.c — ft_cmd_lst_create, line ~176:
if (!cmd_new->path)
    cmd_new->path = cmd_new->args[0];  // sets path to raw string e.g. ":"

// child_exec.c — close_pipe_and_exec:
if (execv(cmd->path, cmd->args) == -1)
    ft_exit(minishell, errno, "EXECV");  // kills the child with ft_exit
```

### Problem

When a user types a command that doesn't exist (e.g., `:`, `foo`, `nonexistent`):

1. **`ft_check_cmd`** iterates over all PATH directories. None contains the command, so it returns without setting `token->path` — it stays `NULL`.
2. **`ft_cmd_lst_create`** sees `cmd->path == NULL` and falls back to `cmd->path = cmd->args[0]` (the raw string `":"`).
3. The shell **forks anyway** and the child calls `execv(":", ...)` which fails immediately (`ENOENT` or `EACCES`).
4. **`ft_exit`** is called, which runs `ft_gc_free_all`, `rl_clear_history`, prints "EXECV" to stderr, and calls `exit(errno)` — this kills the **child** process.

While this doesn't kill the parent shell (it's in a child), it produces a confusing error message ("EXECV" instead of "command not found") and still forks uselessly.

However, **in the single-command case** (before Bug #1 is fixed), the parent doesn't wait, so the user just sees "EXECV" appear asynchronously. After Bug #1 is fixed, the parent will wait for this child, but the exit status and error message are still wrong.

### Impact

- Confusing error: user sees `EXECV` instead of `minishell: :: command not found`.
- In bash, `:` is actually a valid built-in (no-op), but even for truly invalid commands, the behavior should be a clean error, not a raw errno dump.
- The `ft_exit` call in the child also calls `rl_clear_history()` which is unnecessary in a child process (readline state is not shared after fork).

### How to Reproduce

```bash
foo$> :
EXECV
# Should print: minishell: :: command not found

foo$> nonexistent_command
EXECV
# Should print: minishell: nonexistent_command: command not found
```

### Fix

Validate the command **before forking**. If `cmd->path` is still NULL after parsing and the command is not a built-in, print "command not found" and skip execution:

**Option A — Check in `ft_cmd_lst_create` (after the fallback):**
```c
if (!cmd_new->path)
    cmd_new->path = cmd_new->args[0];
// No change above, but mark that it was not resolved:
// The real check happens at exec time.
```

**Option B (recommended) — Check in `close_pipe_and_exec` before `execv`:**
```c
void close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2])
{
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    if (is_built_in(cmd) == 1)
    {
        run_built_in_piped(cmd, minishell);
        exit(0);
    }
    if (!cmd->path || access(cmd->path, X_OK) != 0)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(cmd->args[0], 2);
        ft_putstr_fd(": command not found\n", 2);
        exit(127);  // 127 = standard "command not found" exit code
    }
    if (execv(cmd->path, cmd->args) == -1)
        ft_exit(minishell, errno, "EXECV");
}
```

Exit code **127** is the POSIX standard for "command not found", and **126** should be used for "permission denied".

---

## Additional Observation: Unnecessary pipe for single commands

In `ft_pipe_and_fork`, a `pipe()` is always called even for a single command (`size_cmd == 1`). This is harmless because the child closes both ends in `close_pipe_and_exec()`, and `handler_first_cmd` skips the `dup2` when `size_cmd == 1`. However, it wastes a system call and two file descriptors. A cleaner approach would be to only call `pipe()` when `size_cmd > 1` or when `i < size_cmd - 1`.

---

## Summary Table

| # | Severity | File | Function | Description |
|---|----------|------|----------|-------------|
| 1 | **CRITICAL** | `exec.c` | `ft_exec` | Single commands never waited on (`&&` should be `\|\|`) |
| 2 | **CRITICAL** | `handler.c` | `handler_mid_cmd` | `dup2(pipe_fd, STDIN_FILENO)` → should be `STDOUT_FILENO` |
| 3 | MODERATE | `handler.c` | `handler_last_cmd` | `prev_pipe` not closed after `dup2` |
| 4 | MODERATE | `exec.c` | `ft_pipe_and_fork` | `prev_pipe` never closed after loop |
| 5 | MINOR | `child_exec.c` | `close_pipe_and_exec` | `exit(1)` should be `exit(0)` for builtins |
| 6 | **CRITICAL** | `is_built_in.c` | `run_built_in` / `run_built_in_piped` | `ft_strcmp(cmd->args[1], "-n")` crashes when `echo` has no args (NULL deref) |
| 7 | **CRITICAL** | `check2.c` / `cmds.c` / `child_exec.c` | `ft_check_cmd` / `close_pipe_and_exec` | Unknown commands are never validated; `execv` called on invalid path, prints raw "EXECV" instead of "command not found" |

---

## How to Reproduce

```bash
# Bug 1: sleep appears to do nothing
foo$> sleep 3
# Prompt returns instantly instead of waiting 3 seconds

# Bug 2: 3-stage pipeline broken
foo$> echo hello | cat | wc -l
# Produces no output or hangs

# Bug 6: echo with no args crashes (SEGV)
foo$> echo
# SEGV: NULL pointer dereference on cmd->args[1]

foo$> cat << EOF | wc -l | echo | sleep 3
# Child running 'echo' crashes with AddressSanitizer: SEGV on address 0x0

# Bug 7: unknown command prints "EXECV" instead of "command not found"
foo$> :
EXECV
# Should print: minishell: :: command not found
```

---

*Report generated on 2026-02-16 after full source review of `src/execution/`.*
