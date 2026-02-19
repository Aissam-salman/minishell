# REPORT 4 — Execution Order Analysis & Remaining Bugs

*Generated on 2026-02-19 after full tester run (144/146).*
*Goal: Document every execution order mistake, explain what bash really does, and give precise fix tips.*

---

## Table of Contents

- [Current Score](#current-score)
- [Part A — Your Execution Pipeline (What Happens Now)](#part-a--your-execution-pipeline-what-happens-now)
- [Part B — Bash's Execution Pipeline (What Should Happen)](#part-b--bashs-execution-pipeline-what-should-happen)
- [Part C — Failing Tests](#part-c--failing-tests)
  - [BUG 1 — Test 83: Single built-in ignores redirections](#bug-1--test-83-echo-hi---outfilesoutfile01-bye)
  - [BUG 2 — Test 134: `$EMPTY` should be a no-op](#bug-2--test-134-empty-should-be-a-no-op)
- [Part D — Warning (Passing but Wrong Error Message)](#part-d--warning-passing-but-wrong-error-message)
  - [WARN 1 — Test 73: Missing "Broken pipe" error](#warn-1--test-73-missing-broken-pipe-error)
- [Part E — Full Execution Order Comparison Table](#part-e--full-execution-order-comparison-table)
- [Part F — Order-of-Operations Deep Dive](#part-f--order-of-operations-deep-dive)
  - [1. Variable expansion vs. command resolution timing](#1-variable-expansion-vs-command-resolution-timing)
  - [2. When to check if a command is empty after expansion](#2-when-to-check-if-a-command-is-empty-after-expansion)
  - [3. Redirections for single built-in commands](#3-redirections-for-single-built-in-commands)
  - [4. Error message selection: slash-based dispatch](#4-error-message-selection-slash-based-dispatch)
  - [5. Directory detection before "Permission denied"](#5-directory-detection-before-permission-denied)
- [Part G — Recap: What To Fix, In Which File](#part-g--recap-what-to-fix-in-which-file)

---

## Current Score

```
144 / 146  (1 ⚠️ warning)
```

| Status | Count | Tests |
|--------|-------|-------|
| ❌ Fail | 2 | 83, 134 |
| ⚠️ Warn | 1 | 73 (passes, wrong error message) |
| ✅ Pass | 143 | Everything else |

---

## Part A — Your Execution Pipeline (What Happens Now)

Here is the **exact order** your code follows for every command line:

```
main.c                       ← main loop
│
├─ 1. readline()             ← read user input
├─ 2. ft_tokenize()          ← lexer.c: split into tokens, assign types (WORD, PIPE, etc.)
│     ├─ ft_token_lst_create()   ← char-by-char state machine, creates raw token strings
│     └─ ft_type_affect()        ← assigns PIPE / IN_CHEVRON / OUT_CHEVRON / WORD types
│
├─ 3. checker_token()        ← parser.c: syntax check + resolve commands
│     └─ for each WORD token (first word after pipe or start):
│           ├─ ft_quotes_handle()      ← expand $VAR and remove quotes  ⬅️ EXPANSION HERE
│           ├─ ft_cmd_find_path()      ← check2.c: resolve path via PATH or direct path
│           └─ token->type = CMD       ← mark as command
│
├─ 4. ft_cmd_lst_create()    ← cmds.c: build t_cmd linked list from tokens
│     └─ for each token:
│           ├─ CMD   → cmd->path = token->path, cmd->args[0] = token->str
│           ├─ WORD  → cmd->args[i++] = token->str
│           ├─ FLAG  → cmd->args[i++] = token->str
│           └─ REDIR → ft_redirection_handler() opens files, sets cmd->infd/outfd
│     └─ FALLBACK: if !cmd->path → cmd->path = cmd->args[0]  ⬅️ PROBLEM AREA
│
├─ 5. ft_exec()              ← exec.c: execute commands
│     ├─ if single built-in → run_built_in() in parent  ⬅️ NO REDIRECTION APPLIED
│     └─ else for each cmd:
│           ├─ pipe()
│           ├─ fork()
│           ├─ child: child_process()
│           │     ├─ handler_*_cmd()          ← dup2 pipes + redirections
│           │     └─ close_pipe_and_exec()
│           │           ├─ check built-in → run in child, exit(0)
│           │           ├─ check !cmd->path → exit(127)
│           │           ├─ check access(X_OK) → exit(126 or 127)
│           │           └─ execv()
│           └─ parent: close pipe write-end, save read-end
│
└─ 6. ft_wait_subprocess()   ← wait.c: waitpid for all children
```

---

## Part B — Bash's Execution Pipeline (What Should Happen)

```
1. READ     → readline
2. TOKENIZE → split into words, operators
3. PARSE    → build AST / command table, check syntax
4. EXPAND   → $VAR expansion, quote removal, word splitting, globbing
5. REDIRECT → open all files, set up fd redirections  ⬅️ BEFORE any execution
6. EXECUTE  →
     ├─ if single built-in: apply redirections (save/restore STDOUT), run in parent
     ├─ if pipeline: fork children, each child applies its own redirections + execve
     └─ if command is empty after expansion: DO NOTHING (exit 0)
7. WAIT     → collect exit statuses
```

**Key differences with your code:**

| Step | Bash | Your code | Problem? |
|------|------|-----------|----------|
| Expansion timing | After parsing, before execution | During `checker_token()` (step 3) | Mostly OK but causes issues with empty-after-expansion |
| Empty command check | After expansion, before exec | Never checked | ❌ **BUG 2** |
| Single built-in redirections | Save STDOUT → dup2 → run → restore | Runs without any dup2 | ❌ **BUG 1** |
| Directory detection | `stat()` + `S_ISDIR` → "Is a directory" | Only `access()` checks | Fixed recently, but keep in mind |

---

## Part C — Failing Tests

---

### BUG 1 — Test 83: `echo hi >         ./outfiles/outfile01 bye`

**What the tester expects:**
```
stdout:  (empty)
outfile01: hi bye
exit code: 0
```

**What minishell does:**
```
stdout: hi bye
outfile01: (empty — file not even created or empty)
exit code: 0
```

#### Root Cause

In `exec.c`, line 44-45, when the command is a **single built-in**:

```c
if (is_built_in(cmd) && size_cmd == 1)
    run_built_in(cmd, minishell);   // ← runs echo directly on STDOUT
```

The `outfd` from the redirection is stored in `cmd->outfd`, but **nobody calls `dup2(cmd->outfd, STDOUT_FILENO)`** before running the built-in. The child path (`child_process`) does handle this via `handler_first_cmd()`, but the single-built-in parent path skips it entirely.

#### When It Happens In Your Code

```
ft_exec()
  → is_built_in(cmd) && size_cmd == 1   ← TRUE
  → run_built_in(cmd, minishell)         ← echo writes to REAL stdout (fd 1)
  → return                               ← cmd->outfd (the opened file) is NEVER used
```

#### When It Should Happen (Bash's Way)

```
ft_exec()
  → is_built_in(cmd) && size_cmd == 1   ← TRUE
  → save original STDOUT: saved_stdout = dup(STDOUT_FILENO)
  → if cmd->outfd != STDOUT_FILENO:
       dup2(cmd->outfd, STDOUT_FILENO)
       close(cmd->outfd)
  → if cmd->infd != STDIN_FILENO:
       dup2(cmd->infd, STDIN_FILENO)
       close(cmd->infd)
  → run_built_in(cmd, minishell)         ← echo now writes to the file
  → restore STDOUT: dup2(saved_stdout, STDOUT_FILENO)
  → close(saved_stdout)
```

#### How To Fix — `exec.c`

In `ft_pipe_and_fork` (or better: before `run_built_in` in the single-builtin path), add redirection handling:

```c
if (is_built_in(cmd) && size_cmd == 1)
{
    int saved_stdout = -1;
    int saved_stdin = -1;

    if (cmd->outfd != STDOUT_FILENO)
    {
        saved_stdout = dup(STDOUT_FILENO);
        dup2(cmd->outfd, STDOUT_FILENO);
        close(cmd->outfd);
    }
    if (cmd->infd != STDIN_FILENO)
    {
        saved_stdin = dup(STDIN_FILENO);
        dup2(cmd->infd, STDIN_FILENO);
        close(cmd->infd);
    }
    run_built_in(cmd, minishell);
    if (saved_stdout != -1)
    {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }
    if (saved_stdin != -1)
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
}
```

> Also: if `cmd->error_file == 1` or `cmd->outfd == -1` or `cmd->infd == -1`, you should **not** run the built-in at all (just set exit_status = 1).

**Files to modify:** `src/exec/exec.c` (the single-builtin branch in `ft_pipe_and_fork`)

---

### BUG 2 — Test 134: `$EMPTY` should be a no-op

**What the tester expects:**
```
stdout:  (empty)
stderr:  (empty)
exit code: 0
```

**What minishell does:**
```
stdout:  (empty)
stderr: "bash : : command not found"
exit code: 127
```

#### Root Cause

When `$EMPTY` is an unset (or empty) variable, after expansion `token->str` becomes `""` (empty string). But your code still treats it as a command:

In `parser.c`, `handle_word()`:
```c
void handle_word(t_token *token, t_minishell *minishell, int *cmd_find)
{
    if (*cmd_find == 0)
    {
        ft_cmd_find_path(minishell, token);   // ← tries to find path for ""
        token->type = CMD;                     // ← marks empty string as CMD
        *cmd_find = 1;
    }
}
```

Then in `cmds.c`, `ft_cmd_lst_create()`:
```c
if (!cmd_new->path)
    cmd_new->path = cmd_new->args[0];   // ← sets path to "" (empty string)
```

Then in `child_exec.c`, `close_pipe_and_exec()`:
```c
if (!cmd->path)   // ← FALSE because path is "" not NULL
    ft_exit(minishell, CMD_NOT_FOUND, ...);
```

So it falls through to `execv("", args)` which fails.

#### When It Happens In Your Code

```
checker_token()
  → ft_quotes_handle()   ← expands $EMPTY to ""
  → handle_word()
     → ft_cmd_find_path(token)  ← token->str is "" → returns GENERAL_ERROR
     → token->type = CMD        ← still marks as CMD!

ft_cmd_lst_create()
  → cmd->args[0] = ""           ← empty string
  → cmd->path = "" (fallback)   ← empty string, NOT null

ft_exec() → fork → child
  → close_pipe_and_exec()
     → !cmd->path is FALSE (it's "" not NULL)
     → access("", X_OK) → -1 (ENOENT)
     → error: "command not found", exit 127
```

#### When It Should Happen (Bash's Way)

In bash, when **all words of a simple command expand to nothing**, the command is **silently skipped** with exit status 0. This should be checked **after expansion** but **before execution**.

```
After expansion:
  → if cmd->args[0] is NULL or cmd->args[0][0] == '\0':
       → DO NOTHING
       → exit_status = 0
       → skip execution entirely
```

#### How To Fix

**Option A — In `ft_cmd_lst_create()` (cmds.c):**

After building the command, before adding it to the list, check if the command is empty:

```c
// After the while loop that processes tokens
cmd_new->args[i] = NULL;
if (!cmd_new->path)
    cmd_new->path = cmd_new->args[0];

// NEW: skip empty commands (from expanded empty variables)
if (!cmd_new->args[0] || !cmd_new->args[0][0])
{
    // Don't add this cmd — it's a no-op
    // But still process redirections (they were already opened)
    if (tok_current)
        tok_current = tok_current->next;
    continue;
}

ft_cmd_add(minishell, cmd_new);
```

**Option B — In `ft_exec()` or `close_pipe_and_exec()`:**

Right before trying to execute, bail out silently:

```c
// In close_pipe_and_exec, after the built-in check:
if (!cmd->args[0] || !cmd->args[0][0])
    exit(SUCCESS);
```

> Option A is cleaner (bash does it before forking). Option B is simpler to implement but still forks unnecessarily.

**Files to modify:** `src/utils/cmds.c` (Option A) or `src/exec/child_exec.c` (Option B)

---

## Part D — Warning (Passing but Wrong Error Message)

### WARN 1 — Test 73: Missing "Broken pipe" error

**Test:** `cat <"./test_files/infile" | echo hi`

| | Your minishell | Bash |
|---|---|---|
| Error message | *(none)* | `Broken pipe` |

When `echo hi` finishes and closes the pipe read-end, `cat` gets a `SIGPIPE`. Bash prints "Broken pipe" but your minishell doesn't. This is minor (the test still passes on output + exit code) but worth knowing.

**Why:** Your child signal handler uses `SIG_DFL` for SIGPIPE handling, so `cat` gets killed silently. Bash catches it and prints the message.

**Low priority** — not a failing test.

---

## Part E — Full Execution Order Comparison Table

| Step | Bash Order | Your Order | Status |
|------|-----------|------------|--------|
| 1. Read input | `readline()` | `readline()` | ✅ Same |
| 2. Tokenize | Split words + operators | `ft_token_lst_create()` + `ft_type_affect()` | ✅ Same |
| 3. Syntax check | Check unclosed quotes, bad pipes, bad redirections | `checker_token()` | ✅ Same |
| 4. Expand variables | `$VAR` → value, remove quotes | `ft_quotes_handle()` inside `checker_token()` | ⚠️ Works but **too early** for CMD tokens — expansion should happen before path resolution, which it does, but the empty-result case is not handled |
| 5. Resolve command path | Search PATH or use direct path | `ft_cmd_find_path()` | ✅ OK (fixed recently for `/` check) |
| 6. Build command structs | Create argv, open redir files | `ft_cmd_lst_create()` → `ft_token_affect()` | ✅ OK |
| 7. **Check empty command** | If args[0] is empty → skip, exit 0 | **Not done** | ❌ **BUG 2** |
| 8. Apply redirections (single built-in) | `dup2` before running | **Not done** | ❌ **BUG 1** |
| 9. Fork + exec (external / pipeline) | fork, dup2 pipes+redirs, execve | `child_process()` → `close_pipe_and_exec()` | ✅ OK |
| 10. Wait + collect status | waitpid all children | `ft_wait_subprocess()` | ✅ OK |

---

## Part F — Order-of-Operations Deep Dive

### 1. Variable expansion vs. command resolution timing

**Your order:**
```
checker_token():
  for each token:
    ft_quotes_handle(token)       ← expand $VAR here
    if first word:
      ft_cmd_find_path(token)     ← resolve path with expanded string
      token->type = CMD
```

**Correct order (same as yours, this is fine):**
```
expand first → then resolve path
```

✅ This is correct. Expansion before path resolution is the right order.

### 2. When to check if a command is empty after expansion

**Your code:** Never checks. The empty string `""` flows all the way to `execv`.

**Bash:** After expansion + quote removal, before execution:
- If all words are gone → no command to run → exit 0
- If first word is empty → no command to run → exit 0

**This is what causes Bug 2.**

### 3. Redirections for single built-in commands

**Your execution flow for `echo hi > file bye` (single built-in):**
```
ft_cmd_lst_create():
  → token "echo" → CMD, path=/usr/bin/echo
  → token "hi"   → WORD, args[1]="hi"
  → token ">"    → REDIR → opens ./outfiles/outfile01 → cmd->outfd = 3
  → token "bye"  → WORD, args[2]="bye"

ft_exec():
  → is_built_in("echo") && size_cmd == 1 → TRUE
  → run_built_in(cmd, minishell)
     → ft_echo(args)
        → write(STDOUT_FILENO, "hi", 2)     ← writes to terminal fd 1
        → write(STDOUT_FILENO, " ", 1)
        → write(STDOUT_FILENO, "bye", 3)
        → write(STDOUT_FILENO, "\n", 1)
  → return                                   ← cmd->outfd (fd 3) LEAKED, never used!
```

**Bash's flow for the same command:**
```
  → open file → fd 3
  → save: saved = dup(1) → fd 4
  → dup2(3, 1) → STDOUT now points to file
  → close(3)
  → echo "hi bye" → writes to fd 1 → goes into file
  → dup2(4, 1) → restore STDOUT
  → close(4)
```

**This is what causes Bug 1.**

### 4. Error message selection: slash-based dispatch

After your recent fixes, the logic in `close_pipe_and_exec` uses `strerror()` which gives system-level messages. Bash formats its own messages:

| Command | Contains `/` | Exists? | Executable? | Is Dir? | Bash error | Bash exit |
|---------|-------------|---------|-------------|---------|------------|-----------|
| `foo` | No | — | — | — | `command not found` | 127 |
| `./foo` | Yes | No | — | — | `No such file or directory` | 127 |
| `./foo` | Yes | Yes | No | No | `Permission denied` | 126 |
| `./foo` | Yes | Yes | — | Yes | `Is a directory` | 126 |
| `/foo` | Yes | No | — | — | `No such file or directory` | 127 |

Your current `close_pipe_and_exec` uses `strerror(CMD_NOT_FOUND)` which gives a system errno string like `"No such process"` (since 127 isn't a real errno). You should format the error messages yourself, not rely on `strerror()` for exit codes.

### 5. Directory detection before "Permission denied"

When a path contains `/` and points to a **directory**, bash says `"Is a directory"` (exit 126), not `"Permission denied"`. Your `ft_cmd_find_path` filters out directories with `S_ISREG()`, which is correct for path resolution, but in the error reporting phase (`close_pipe_and_exec`), you should explicitly check `S_ISDIR` to print the right message.

This is currently handled (the recent tests pass), but the commented-out code in `close_pipe_and_exec` suggests it was being worked on. The `strerror()` approach might coincidentally work for `EACCES` cases but not for directory cases.

---

## Part G — Recap: What To Fix, In Which File

### ❌ Bug 1 — Single built-in redirections (Test 83)

| | |
|---|---|
| **File** | `src/exec/exec.c` |
| **Function** | `ft_pipe_and_fork()` (the `if (is_built_in && size_cmd == 1)` branch) |
| **What to do** | Before calling `run_built_in()`: save STDOUT/STDIN with `dup()`, apply `dup2(cmd->outfd/infd)`, run built-in, then restore. Also check `cmd->error_file` / bad fds and skip execution if redirections failed. |
| **Difficulty** | 🟢 Easy — ~20 lines |

### ❌ Bug 2 — Empty command after expansion (Test 134)

| | |
|---|---|
| **File** | `src/utils/cmds.c` or `src/exec/child_exec.c` |
| **Function** | `ft_cmd_lst_create()` or `close_pipe_and_exec()` |
| **What to do** | After expansion, if `cmd->args[0]` is `NULL` or `""`, skip execution entirely (exit 0 in child, or don't add cmd to list). |
| **Difficulty** | 🟢 Easy — ~5 lines |

### ⚠️ Warn 1 — Wrong error messages with `strerror()` (Not failing, but fragile)

| | |
|---|---|
| **File** | `src/exec/child_exec.c`, `src/utils/errors.c` |
| **Function** | `close_pipe_and_exec()`, `ft_exit()` |
| **What to do** | Don't pass `strerror(126)` or `strerror(127)` — those are **shell exit codes**, not errno values. Use your own error strings: `"command not found"`, `"Permission denied"`, `"No such file or directory"`, `"Is a directory"`. |
| **Difficulty** | 🟡 Medium — need to rewrite error dispatch in `close_pipe_and_exec()` |

---

### Priority Order

```
1. 🔴 Bug 1 (Test 83)  — single built-in redirections  → MOST IMPACTFUL
2. 🔴 Bug 2 (Test 134) — empty command after expansion  → EASY FIX  
3. 🟡 Warn   (strerror misuse)                          → Cleanup for robustness
```

Once Bug 1 and Bug 2 are fixed → **146/146** 🎉
