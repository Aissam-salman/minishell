# REPORT 3 — Complete Mandatory Audit: Fixes, Return Values, Enums & 50 Tricky Tests

*Generated on 2026-02-18 after full codebase analysis.*
*Goal: Pass every mandatory test of the 42 minishell subject.*

---

## Table of Contents

- [Part A — Lexer / Tokenizer](#part-a--lexer--tokenizer)
- [Part B — Parser / Syntax Checker](#part-b--parser--syntax-checker)
- [Part C — Quote Handling & Variable Expansion](#part-c--quote-handling--variable-expansion)
- [Part D — Redirections & Heredoc](#part-d--redirections--heredoc)
- [Part E — Pipes](#part-e--pipes)
- [Part F — Execution](#part-f--execution)
- [Part G — Built-ins](#part-g--built-ins)
- [Part H — Signals](#part-h--signals)
- [Part I — Environment & `$?`](#part-i--environment--)
- [Part J — Enum / Macro / Header Cleanup](#part-j--enum--macro--header-cleanup)
- [Part K — Return Value Reference Table](#part-k--return-value-reference-table)
- [Part L — 50 Tricky Test Cases](#part-l--50-tricky-test-cases)
- [Part M — Skills Assessment & Learning Plan](#part-m--skills-assessment--learning-plan)

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part A — Lexer / Tokenizer
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
The lexer (`src/parsing/lexer.c` + `src/utils/tokens.c`) splits the input line into a linked list of `t_token` nodes and assigns basic types (`PIPE`, `IN_CHEVRON`, `WORD`, etc.).

### What works ✅
- Basic word splitting on spaces/tabs.
- Recognition of `|`, `<`, `<<`, `>`, `>>`.
- State machine for single-quote / double-quote tracking.
- Rejection of unclosed quotes.

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| A1 | **Operator inside quotes** | `lexer.c` | `echo "hello|world"` currently splits on `\|` because `ft_state_interpret` checks `ft_ischarset(line[*index], OPERATORS)` even when the state is `NORMAL` but the quote was opened on the same token. The check must also verify `state == NORMAL`. **Currently it does**, but the `OPERATORS` check must be guarded by `minishell->state == NORMAL` — this is correct in the current code for the outer `else if`. However, there is a subtle bug: when `"` opens a quote (state → `IN_DQUOTE`), the `ft_state_detect` happens **before** `ft_state_interpret`, so by the time we reach interpret, the state is already `IN_DQUOTE`. But `ft_state_interpret`'s second branch checks `minishell->state == NORMAL` — so the `|` inside quotes is **not** treated as operator. **This appears correct** — verify with test `echo "hello|world"`. |
| A2 | **Empty token at end of operator** | `lexer.c` | `echo hello>outfile` generates tokens `[echo] [hello] [>outfile]` because operator `>` is not split from `outfile` when they are adjacent with no space. The `ft_state_interpret` tries to flush the buffer when it switches between operator and non-operator characters, but the logic around `buffer[0]` being in OPERATORS is fragile. **Test and fix**: `echo hello>outfile` must produce `[echo] [hello] [>] [outfile]`. |
| A3 | **Consecutive operators** | `lexer.c` | `>>>` should produce `[>>] [>]`, but the current code may produce `[>>>]` as one token. The flush happens only when `len_buffer >= 2`, which will work for `>>` → flush → then `>` starts a new buffer. **Verify** with `>>>` and `<<<`. |
| A4 | **Tab handling** | `lexer.c` | `SPACES` macro should be `" \t"`. Currently `SEPARATORS` is `" $'\"\t"` but the space-check in `ft_state_interpret` uses `ft_ischarset(line[*index], SPACES)` — **`SPACES` is not defined anywhere**. If this is defined in libft, verify it equals `" \t\n\r\v\f"` or at least `" \t"`. If it's missing, the shell can't handle tabs as word separators. |

### Recommended changes
- Add a dedicated `#define SPACES " \t"` in `minishell.h` if it doesn't exist.
- Verify the lexer handles `echo hello>outfile`, `echo hello>>outfile`, and `>outfile echo hello` correctly.

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part B — Parser / Syntax Checker
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/parsing/parser.c` + `src/parsing/check.c` + `src/parsing/check2.c` validate the token list for syntax errors, assign `CMD`/`FLAG` types, and resolve command paths.

### What works ✅
- Detection of invalid redirections (`> |`, `>>` at end of line).
- Pipe syntax validation (no `|` at start, no `||`).
- Command path resolution via `PATH` env.

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| B1 | **Redirection-only commands** | `parser.c` | `> outfile` with no command: bash creates the file and exits 0. Currently the parser may not flag a `CMD` token, and `cmd->path` stays `NULL`. `close_pipe_and_exec` then prints "command not found" and exits 127. **Fix**: if a cmd has redirections but no `CMD` token, still create the cmd node and open the files, but don't `execv`. Just `_exit(0)`. |
| B2 | **Multiple redirections** | `parser.c` | `> a > b > c echo hello` must create/truncate `a`, `b`, `c` and write "hello" to `c` (last wins). Currently the `handle_word`/`handle_redirection` flow skips the `cmd_find` flag — the `CMD` is found only after all redirections. **Verify** that `> a > b echo hello` finds `echo` as the CMD and creates both `a` and `b`. |
| B3 | **Wrong exit code for syntax errors** | `parser.c` / `errors.h` | Bash returns **2** for syntax errors (e.g. `|`, `> |`). Currently `ERR_SYNTAX = 2` which is correct in the enum, but `ft_error` is called with `ERR_SYNTAX` (value 2) — **verify** that `minishell->exit_status` is actually set to 2 (it is via `ft_error`). ✅ |
| B4 | **`export` with no `=`** | `parser.c` | `export VAR` (no `=`) should add the variable with no value. Currently `ft_env_new` handles it. But `is_built_in` + the parser must not try to path-resolve `export`. This is fine because `handle_word` calls `ft_cmd_find_path` which sets `token->path` (may stay NULL for builtins), and `is_built_in` catches it. ✅ |
| B5 | **`.` and `..` as commands** | `check2.c` | Running `.` should return `2` ("filename argument required"). Running `..` should return `127` ("command not found"). Currently `ft_cmd_find_path` may match `..` as a directory via `stat()` but `S_ISREG` filters it out. **Verify**. |
| B6 | **`/` as a command** | `check2.c` | `ft_cmd_find_path` has a check for `/` but returns early without setting an error. Bash says `bash: /: Is a directory` and returns 126. Currently returns nothing visible. **Fix**: print appropriate error and set exit_status to 126. |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part C — Quote Handling & Variable Expansion
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/utils/parser_utils.c` handles quote removal and `$VAR` expansion via `ft_quotes_handle` and `ft_expend`.

### What works ✅
- Single-quote preservation (no expansion).
- Double-quote expansion of `$VAR`.
- `$?` expansion to last exit status.
- Nested quotes: `"it's"` works.

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| C1 | **`$` at end of string** | `parser_utils.c` | `echo "$"` should print `$`. Currently `ft_expend` increments `*start` past the `$`, then checks `str[*start]`. If `str[*start]` is `\0`, the `while` loop body is skipped, `ft_env_find` gets an empty buffer, returns NULL, and nothing is appended. The `$` is lost. **Fix**: if `str[*start]` is `\0` or `"`, append a literal `$` to `usable_str`. |
| C2 | **`$` followed by a digit** | `parser_utils.c` | `echo $1` should print nothing (positional params not required). `echo $123` should print `23`. Currently the `while` loop in `ft_expend` reads all alphanums, so `$123` looks up env `123`. **Fix**: if `str[*start]` is a digit, consume only that one char (positional parameter), skip lookup, move on. |
| C3 | **`$$`** | `parser_utils.c` | `echo $$` in bash prints the PID. Not required by the subject, but `$$` currently triggers `ft_expend` twice in a row — the second `$` sees `\0` or end of string and gets lost. At minimum, `$$` should not crash. **Verify no segfault**. |
| C4 | **Empty var in double quotes** | `parser_utils.c` | `echo "$NONEXISTENT"` should print an empty line. Currently it does (ft_env_find returns NULL, nothing appended, `\n` printed by echo). ✅ |
| C5 | **`$VAR` word splitting** | `parser_utils.c` | After expansion, bash performs word splitting on unquoted `$VAR`. For mandatory minishell this is **not required**, but be aware that `export A="hello world"` then `echo $A` should print `hello world` as **one argument**, not split. Currently since expansion happens at the token level (one token → one string), this is fine. ✅ |
| C6 | **`$?` immediately followed by text** | `parser_utils.c` | `echo $?abc` should print `0abc` (assuming last exit was 0). Currently `ft_expend` increments `*start` once for `?`, then returns. The remaining `abc` is handled by the outer loop. **Verify** this works. |
| C7 | **Quotes around redirections** | `parser_utils.c` | `echo hello > "outfile"` — the filename token `"outfile"` must be unquoted to `outfile` before `ft_open`. Currently `ft_quotes_handle` is called in `checker_token` which runs before `ft_cmd_lst_create`. The redirection file token (`WORD` after `>`) is processed by `ft_quotes_handle`. **Verify**. |
| C8 | **Single quotes block expansion** | `parser_utils.c` | `echo '$HOME'` should print `$HOME` literally. Currently the `IN_QUOTE` state blocks entry into `ft_expend`. ✅ |
| C9 | **Expand in heredoc delimiter** | `heredoc.c` | If heredoc delimiter is quoted (`<< 'EOF'`), no expansion inside the heredoc body. If unquoted (`<< EOF`), `$VAR` is expanded in the body. **Currently not implemented** — the heredoc body is written raw to the pipe with no expansion at all. **Fix**: when delimiter is unquoted, expand `$VAR` in each line before writing to pipe. |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part D — Redirections & Heredoc
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/exec/cntrl.c` opens files and sets `cmd->infd`/`cmd->outfd`. `src/utils/heredoc.c` implements heredoc via pipes.

### What works ✅
- `>` (truncate), `>>` (append), `<` (input).
- Heredoc reads until delimiter, pipes content to command.
- Multiple heredocs: only last one feeds stdin.

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| D1 | **`handler_first_cmd` didn't apply `outfd`** | `handler.c` | **FIXED** in our previous session. `outfd` is now dup2'd for single commands. ✅ |
| D2 | **First cmd in pipeline ignores `outfd`** | `handler.c` | `cat file > outfile \| grep x` — bash sends cat's output to `outfile`, not the pipe. Currently `handler_first_cmd` does `dup2(pipe_fd, STDOUT)` when `size_cmd > 1`, ignoring `outfd`. **Fix**: when `size_cmd > 1` AND `outfd != STDOUT_FILENO`, use `outfd` instead of `pipe_fd`. This is a design choice — bash applies the redirection to the specific command, not the pipe. |
| D3 | **Redirection error doesn't stop execution** | `cntrl.c` | `cat < nonexistent` should print `bash: nonexistent: No such file or directory` and return 1. Currently `ft_redirection_handler` returns `GENERAL_ERROR` when `fd == -1`, and `ft_token_affect` returns `GENERAL_ERROR`, and `ft_cmd_lst_create` returns `GENERAL_ERROR`. But then `main.c` checks `minishell.exit_status != 0` and continues — the command is **not** executed. **Verify** this flow is correct. However, when the `fd == -1` path is taken for `IN_DCHEVRON` (heredoc), the function still falls through to the `if (fd == -1)` check and prints a perror for the heredoc delimiter. **Fix**: return early after heredoc handling. |
| D4 | **Multiple redirections: last one wins** | `cntrl.c` | `echo hi > a > b` must create `a` (empty) and write "hi" to `b`. Currently `ft_redirection_exec` closes the previous fd and replaces with the new one. This is correct. **But**: the old file is opened and closed — its content is truncated to empty. ✅ |
| D5 | **Heredoc + redirection combo** | `cmds.c` | `cat << EOF > outfile` — the heredoc feeds stdin, and `> outfile` redirects stdout. Currently the `IN_DCHEVRON` case in `ft_token_affect` handles heredoc separately from other redirections. **Verify** both are applied. The heredoc sets `cmd->infd`, the `>` sets `cmd->outfd`. Should work if both are processed. |
| D6 | **Heredoc signals** | `heredoc.c` | Ctrl-C during heredoc should cancel the heredoc and return to the prompt with exit status 130. Ctrl-D (EOF / NULL from readline) already breaks the loop. **Currently**: no signal handling in heredoc — Ctrl-C triggers the parent's handler which calls `rl_redisplay`, creating a mess. **Fix**: set a custom signal handler for `SIGINT` during heredoc that closes the pipe and returns. |
| D7 | **Heredoc count limit** | `heredoc.c` | Bash limits to 16 heredocs. Not strictly required, but don't crash on many heredocs. |
| D8 | **Heredoc with empty delimiter** | `heredoc.c` | `<< ""` — delimiter is empty string after quote removal. Each line typed would match. **Verify** behavior, should not crash. |
| D9 | **File permission error** | `cntrl.c` | `> /etc/passwd` should print `bash: /etc/passwd: Permission denied` and return 1. Currently `ft_open` returns -1 and `perror` prints it. The return is `GENERAL_ERROR` (1). ✅ |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part E — Pipes
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/exec/exec.c` creates pipes and forks for each command in the pipeline.

### What works ✅
- Basic two-command pipes: `ls | grep txt`.
- Multi-stage pipes: `cat file | grep foo | wc -l`.
- Pipe fd management (parent closes write end, passes read end to next child).

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| E1 | **Pipe + single builtin in pipeline** | `exec.c` | `echo hello \| cat` — `echo` should run in a child (forked), not in the parent. Currently the `is_built_in(cmd) && size_cmd == 1` check only skips fork for single builtins. For pipelines, builtins are always forked. ✅ |
| E2 | **Wait for last command only** | `wait.c` / `exec.c` | Bash's `$?` is the exit code of the **last** command in a pipeline. Currently `ft_wait_subprocess` iterates through all pids and the last `WEXITSTATUS` overwrites. This is correct behavior. ✅ |
| E3 | **Builtin in last position of pipeline** | `exec.c` | `cat file \| export A=hello` — `export` runs in a **child** process, so it cannot modify the parent's env. This is correct bash behavior (exports in subshells don't affect parent). ✅ |
| E4 | **Pipe + redirection combo** | `handler.c` | `cat < infile \| grep foo > outfile` — first cmd reads from `infile` (not pipe), last cmd writes to `outfile` (not stdout). `handler_first_cmd` applies `infd` ✅. `handler_last_cmd` applies `outfd` ✅. |
| E5 | **Broken pipe handling** | N/A | `yes \| head -1` — after `head` exits, `yes` receives SIGPIPE and should die. This is handled by the kernel automatically since SIGPIPE's default action is to terminate. ✅ |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part F — Execution
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/exec/exec.c` + `src/exec/child_exec.c` manage forking and execving.

### What works ✅
- Fork + execv for external commands.
- Built-in detection skips fork for single commands.
- Wait for child processes.

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| F1 | **`execve` vs `execv`** | `child_exec.c` | Currently uses `execv(cmd->path, cmd->args)`. The subject says you must use `execve`. **Fix**: build an `envp` array from `minishell->head_env` and use `execve(cmd->path, cmd->args, envp)`. This is also needed so child processes inherit the updated environment (e.g. after `export`). |
| F2 | **Command not found vs Permission denied** | `child_exec.c` | `close_pipe_and_exec` checks `access(cmd->path, X_OK)` — if it fails, it always prints "command not found" (127). But if the file exists but isn't executable, bash prints "Permission denied" (126). **Fix**: check `access(cmd->path, F_OK)` first — if file exists but not executable → 126. If file doesn't exist → 127. |
| F3 | **Directory as command** | `child_exec.c` | Running `/tmp/` or `./src/` should print `Is a directory` and return 126. Currently `close_pipe_and_exec` doesn't check `S_ISDIR`. **Fix**: add a `stat()` check before `execv`. |
| F4 | **Empty command** | `exec.c` | `""` as a command — should print `bash: : command not found` and return 127. Currently `cmd->args[0]` is `""`, `cmd->path` is `""`, `access` fails → "command not found" is printed. **Verify** the empty string is handled. |
| F5 | **`execve` returns -1** | `child_exec.c` | After `execv` fails, the child calls `ft_exit` which calls `exit(errno)`. The errno might be 2 (ENOENT) or 13 (EACCES). Bash convention: 126 for can't execute, 127 for not found. **Fix**: map errno to the correct exit code. |
| F6 | **Env conversion for `execve`** | N/A (to create) | Create `char **ft_env_to_array(t_minishell *minishell)` that converts the `t_env` linked list to a `char **envp` array for `execve`. Each entry is `"NAME=VALUE"`. |
| F7 | **`close_pipe_and_exec` built-in exit code** | `child_exec.c` | Built-in in a forked child always `_exit(0)`. But if the builtin fails (e.g. `cd nonexistent`), the exit code should reflect the error. **Fix**: `run_built_in` should set `minishell->exit_status`, then `_exit(minishell->exit_status)`. |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part G — Built-ins
<!-- ═══════════════════════════════════════════════════════════════ -->

### G1 — `echo`
**File**: `src/built_in/echo.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | Basic echo | `echo hello world` prints `hello world\n`. |
| ✅ | `-n` flag | `echo -n hello` prints `hello` (no newline). |
| ✅ | Multiple `-n` flags | `echo -n -n -n hello` prints `hello`. |
| ✅ | `-nnn` flag | `echo -nnn hello` prints `hello`. |
| 🔧 | **`echo` with no args** | Returns `void`, should still print `\n`. Currently it does. ✅ |
| 🔧 | **`echo` writes to wrong fd** | `ft_write_safe` hardcodes fd `1`. In a pipeline like `echo hello \| cat`, the fd has been dup2'd so `1` is correct. But `run_built_in` doesn't redirect stdout for single builtins with redirections. **Fix**: for single builtins, dup2 `cmd->outfd` to STDOUT before calling the builtin, and restore after. |
| 🔧 | **Return value** | `echo` should set `exit_status = 0` (always succeeds unless write error). Currently `run_built_in` sets `exit_status = 0` before calling. ✅ |

### G2 — `cd`
**File**: `src/built_in/cd.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | `cd /tmp` | Changes directory. |
| ✅ | `cd ~` | Goes to HOME. |
| ✅ | `cd -` | Goes to OLDPWD and prints the path. |
| 🔧 | **`cd` with too many args** | `cd a b` should print `bash: cd: too many arguments` and return 1. Currently `ft_cd` receives only `cmd->args[1]`, so it takes the first arg and ignores the rest. **Fix**: check `cmd->args[2]` exists → error. |
| 🔧 | **`cd` without HOME** | If HOME is unset, `cd` with no args should print `bash: cd: HOME not set` and return 1. Currently `ft_no_path` returns NULL, and `ft_cd` calls `chdir(NULL)` → segfault or undefined. **Fix**: check for NULL return and print error. |
| 🔧 | **`cd` to non-existent dir** | `cd nonexistent` — currently the `stat` + `S_ISDIR` check guards `chdir`, but if the path is not a directory (or doesn't exist), `chdir` is never called, and `update_old_pwd` / `update_pwd` still run — updating PWD to something wrong. **Fix**: try `chdir` directly. If it fails, print error and return 1. Don't update PWD/OLDPWD on failure. |
| 🔧 | **Return value** | `ft_cd` returns `int` but `run_built_in` ignores it. **Fix**: `minishell->exit_status = ft_cd(...)`. |

### G3 — `pwd`
**File**: `src/built_in/pwd.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | Basic pwd | Prints current directory. |
| 🔧 | **Deleted directory** | If the current directory is deleted, `getcwd` returns NULL. Bash still prints the directory from `$PWD`. **Fix**: fall back to `ft_env_find(head_env, "PWD")`. |
| 🔧 | **Output fd** | `ft_pwd` uses `ft_printf` which writes to fd 1. For `pwd > outfile` as a single builtin, stdout is not redirected. Same issue as echo (G1). |

### G4 — `export`
**File**: `src/built_in/export.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | `export A=hello` | Adds to env. |
| ✅ | `export` (no args) | Prints all env vars in `export` format. |
| 🔧 | **`export` prints to wrong fd** | `ft_export` receives `fd` but `run_built_in` passes `2` (stderr!) instead of `1` (stdout). **Fix**: pass `cmd->outfd` (or 1) instead of 2. |
| 🔧 | **`export` format** | `export` with no args should print `declare -x VAR="value"` (with quotes around value). Currently prints `export VAR=value`. **Fix**: update format to `declare -x VAR="value"`. |
| 🔧 | **`export` sorted** | `export` with no args should print in **alphabetical order**. Currently prints in insertion order. **Fix**: sort before printing. |
| 🔧 | **`export` with `+`** | `export A+=hello` should append to existing value. Currently `ft_env_format_check` only checks `isalpha(str[0])` — `A+=hello` passes, but `ft_env_new` splits on `=` and the name becomes `A+`, not `A`. **Fix**: handle `+=` syntax. |
| 🔧 | **`export` validation** | `export 1VAR=hello` should print `bash: export: '1VAR=hello': not a valid identifier` and return 1. `export ""` should error too. Currently `ft_env_format_check` checks `isalpha(str[0])` — ✅ for `1VAR`. But `export _VAR=hello` should be valid (`_` is allowed). **Fix**: accept `_` as first char. Also validate that name contains only `[a-zA-Z0-9_]`. |
| 🔧 | **Multiple args** | `export A=1 B=2 C=3` — currently `run_built_in` only passes `cmd->args[1]`. **Fix**: loop over all args from index 1. |
| 🔧 | **`export` with no `=`** | `export VAR` should add VAR with no value (visible in `export` output but not in `env`). Currently handled by `ft_env_new` which sets `content = NULL`. ✅ but verify `env` skips these (it does: `if (head->content)` check). ✅ |

### G5 — `unset`
**File**: `src/built_in/unset.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | `unset VAR` | Removes from env. |
| ✅ | Multiple args | `unset A B C` — loops and removes each. |
| 🔧 | **Invalid identifier** | `unset 1VAR` should print error and return 1. Currently removes nothing (no match) silently. **Fix**: validate name format, print error for invalid identifiers. |
| 🔧 | **Unset of non-existent var** | `unset NONEXISTENT` should silently succeed (return 0). Currently it loops and finds no match — fine. ✅ |

### G6 — `env`
**File**: `src/built_in/env.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | `env` | Prints all env vars. |
| 🔧 | **`env` with args** | `env` should accept no arguments. `env something` should error (or in bash, it runs `something` with env). For mandatory, just reject args. Currently ignores args. **Fix**: print error if args are provided. |
| 🔧 | **Output fd** | `ft_env` hardcodes `outfd` parameter but `run_built_in` passes `1`. For `env > outfile` as single builtin, same fd issue. |

### G7 — `exit`
**File**: `src/built_in/exit.c`

| Status | Issue | Description |
|--------|-------|-------------|
| ✅ | `exit` | Exits with last status. |
| ✅ | `exit 42` | Exits with code 42. |
| 🔧 | **Debug print** | `ft_atoi_safe` has `ft_printf("code atoi: %d\n", *res)` — **remove** this debug line. |
| 🔧 | **`exit` with too many args** | `exit 1 2` should print `bash: exit: too many arguments`, **not exit**, and return 1. Currently it returns (doesn't exit) ✅, but verify `exit_status` is set to 1. |
| 🔧 | **`exit` argument overflow** | `exit 9999999999999999999` should print `numeric argument required` and exit 2. Currently `ft_atoi_safe` checks `> 2147483647` — but bash accepts values up to `long long` range and then wraps with modulo 256. **Fix**: change to `long long` range check. |
| 🔧 | **`exit` with negative** | `exit -1` should exit with 255 (because -1 % 256 = 255 in unsigned). Currently `exit(code % 256)` — C's `%` with negatives is implementation-defined. **Fix**: `exit((code % 256 + 256) % 256)` or cast to `unsigned char`. |
| 🔧 | **`exit` in pipeline** | `echo hello \| exit 42` — `exit` runs in a child, so it exits the child, not the shell. The shell should continue. Currently builtins in pipelines are forked → `_exit()` is called. But `ft_buildin_exit` calls `exit()` not `_exit()` — should use `_exit()` in child context. |
| 🔧 | **`exit` prints "exit"** | Should print `exit\n` to stderr. Currently prints to stdout with `write(1, "exit\n", 5)`. **Fix**: write to fd 2 (stderr). |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part H — Signals
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current state
`src/utils/signal_core.c` sets up `SIGINT` and `SIGQUIT` handlers for the parent. `src/exec/handler.c` resets them to default in children.

### What works ✅
- Ctrl-C at prompt: new line, prompt redisplayed.
- Ctrl-\ at prompt: ignored.
- Ctrl-C / Ctrl-\ in child: default behavior (terminate/core dump).

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| H1 | **`$?` after Ctrl-C** | `signal_core.c` | After Ctrl-C at the prompt, `$?` should be 130. Currently the signal handler doesn't set `exit_status`. **Fix**: use a global variable (allowed by subject: one global for signal indication) to communicate signal reception to the main loop. |
| H2 | **Ctrl-C during heredoc** | `heredoc.c` | Ctrl-C should cancel the heredoc, close the pipe, and return to the prompt with `$? = 130`. **Currently not handled.** |
| H3 | **Ctrl-D at prompt** | `main.c` | `readline` returns NULL → calls `ft_buildin_exit(minishell, 0)`. But the signature expects `char **args`, not `int`. **Fix**: this is a type mismatch / needs a separate exit path for Ctrl-D. Currently it's `ft_buildin_exit(&minishell, 0)` where `0` is cast to `char **` → **undefined behavior / crash**. Fix immediately: create a dedicated function or pass proper args. |
| H4 | **Global variable** | N/A | The subject allows exactly **one global variable** to indicate a received signal. Currently there is no global. **Create**: `extern int g_signal_received;` and use it to communicate between signal handler and main loop. |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part I — Environment & `$?`
<!-- ═══════════════════════════════════════════════════════════════ -->

### What needs to be created / fixed 🔧

| #  | Issue | File | Description |
|----|-------|------|-------------|
| I1 | **`$?` uses `cached_status`** | `main.c` / `parser_utils.c` | `ft_expend` expands `$?` using `minishell->cached_status` which is set at reset time. This means `echo $?` always shows the **previous** command's status. This is correct behavior. ✅ |
| I2 | **`SHLVL`** | `env_setup.c` | Bash increments `SHLVL` on startup. **Fix**: at the end of `ft_env_setup`, find `SHLVL`, increment by 1 (or set to 1 if not present). |
| I3 | **`_` variable** | N/A | Bash sets `_` to the last argument of the previous command. Not strictly required for mandatory, but some testers check it. |
| I4 | **Env starts empty** | `env_setup.c` | If minishell is started with `env -i ./minishell`, there's no env at all. `ft_env_find(head_env, "PATH")` returns NULL, and `ft_get_path` returns NULL. Commands like `ls` won't be found. This is correct (bash behaves the same). But `cd` crashes because `ft_no_path` returns NULL → `chdir(NULL)`. **Fix**: null-check in `ft_cd`. |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part J — Enum / Macro / Header Cleanup
<!-- ═══════════════════════════════════════════════════════════════ -->

### Current `e_errors` enum (errors.h)

```c
typedef enum e_errors {
    SUCCESS              = 0,
    GENERAL_ERROR        = 1,
    ERR_SYNTAX           = 2,
    NO_SUCH_FILE_O_DIR   = 3,    // ← Unused, overlaps with ENOENT (2)
    ERR_CHECK_TOKEN      = 4,    // ← Unused
    ERR_HANDLE_PIPE      = 5,    // ← Unused
    ERR_REDIRECTION      = 6,    // ← Unused
    PARSING_FAIL         = 50,   // ← Non-standard, never shown to user
    BUFFER_FAIL          = 98,   // ← Non-standard
    MALLOC_FAIL          = 99,   // ← Non-standard
    PERMISSION_DENIED    = 126,
    CMD_NOT_FOUND        = 127,
    SIGNAL               = 128,
} t_errors;
```

### Recommended cleanup

```c
typedef enum e_errors {
    SUCCESS              = 0,    // Command succeeded
    GENERAL_ERROR        = 1,    // Catchall for general errors (e.g. cd fail)
    MISUSE_BUILTIN       = 2,    // Syntax errors, invalid usage (bash convention)
    CANNOT_EXECUTE       = 126,  // Permission denied / is a directory
    CMD_NOT_FOUND        = 127,  // Command not found
    SIGNAL_BASE          = 128,  // 128 + N = killed by signal N (e.g. 130 = SIGINT)
    SIGNAL_INT           = 130,  // 128 + 2 (SIGINT / Ctrl-C)
    SIGNAL_QUIT          = 131,  // 128 + 3 (SIGQUIT / Ctrl-\)
} t_errors;

// Internal error codes (not shown to user, not set as exit_status)
# define ERR_MALLOC     -1
# define ERR_BUFFER     -2
# define ERR_INTERNAL   -3
```

### Key changes:
1. **Remove** `NO_SUCH_FILE_O_DIR`, `ERR_CHECK_TOKEN`, `ERR_HANDLE_PIPE`, `ERR_REDIRECTION` — they are never used or redundant.
2. **Rename** `ERR_SYNTAX` → `MISUSE_BUILTIN` to match bash convention (exit code 2).
3. **Add** `CANNOT_EXECUTE = 126` (was `PERMISSION_DENIED`, but 126 also covers "is a directory").
4. **Add** `SIGNAL_INT = 130` and `SIGNAL_QUIT = 131`.
5. **Separate** internal errors (`MALLOC_FAIL`, `BUFFER_FAIL`, `PARSING_FAIL`) from exit codes — make them negative or `#define` constants, since they should never be returned to the user.

### Current `e_type` enum (minishell.h)

```c
typedef enum e_type {
    DEFAULT = 0,
    PIPE,            // 1
    IN_CHEVRON,      // 2  (<)
    IN_DCHEVRON,     // 3  (<<)
    OUT_CHEVRON,     // 4  (>)
    OUT_DCHEVRON,    // 5  (>>)
    WORD,            // 6
    CMD,             // 7
    FLAG,            // 8
    NBR_TYPES,       // 9
} t_types;
```

This is fine. **Suggestions**:
- Add `R_FILE` (redirection filename) type for the token after a redirection operator, so it's explicit and you don't need `token->next` everywhere.
- `NBR_TYPES` is unused — keep or remove, no impact.

### Macros to add/change

```c
// Replace
# define OPERATORS "|<>"
# define SEPARATORS " $\'\"\t"

// With
# define OPERATORS "|<>"
# define SPACES " \t"
# define EXPAND_STOP " \t$'\"="    // Chars that stop $VAR expansion
```

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part K — Return Value Reference Table
<!-- ═══════════════════════════════════════════════════════════════ -->

Every situation and the expected `$?` value:

| Situation | `$?` | Notes |
|-----------|------|-------|
| Command succeeds | `0` | `ls`, `echo hello` |
| Command general failure | `1` | `cd nonexistent`, `export 1=a` |
| Syntax error | `2` | `| ls`, `> |`, unclosed quotes |
| `exit` with non-numeric arg | `2` | `exit abc` (then exits) |
| `exit` with too many args | `1` | `exit 1 2` (does NOT exit) |
| Command not found | `127` | `notacmd`, `""` |
| Permission denied (file) | `126` | `./src/` (directory), non-executable file |
| Is a directory | `126` | `/tmp/` |
| Ctrl-C at prompt | `130` | `128 + 2` |
| Ctrl-\ at prompt | N/A | Ignored, no change to `$?` |
| Ctrl-C during command | `130` | Child killed by SIGINT |
| Ctrl-\ during command | `131` | Child killed by SIGQUIT |
| Ctrl-D at prompt | N/A | Exit the shell (same as `exit`) |
| Ctrl-C during heredoc | `130` | Cancel heredoc, return to prompt |
| `cat < nonexistent` | `1` | No such file or directory |
| `cat > /root/file` | `1` | Permission denied |
| Successful pipe | exit code of **last** command | `false \| true` → 0 |
| `echo $?` after error | previous `$?` | Expansion happens before exec |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part L — 50 Tricky Test Cases
<!-- ═══════════════════════════════════════════════════════════════ -->

### Lexer & Tokenizer (1–5)

| # | Command | Expected behavior |
|---|---------|-------------------|
| 1 | `echo hello\|world` (no spaces around pipe) | Prints `hello` then pipes to `world` (cmd not found) |
| 2 | `echo "hello\|world"` | Prints `hello\|world` (pipe inside quotes) |
| 3 | `>>>outfile` | Syntax: `>>` then `>outfile` — error or `>> > outfile` |
| 4 | `echo\t\thello` (tabs) | Prints `hello` |
| 5 | `   ` (only spaces) | Nothing happens, prompt redisplays |

### Quotes (6–15)

| # | Command | Expected |
|---|---------|----------|
| 6 | `echo "hello'world"` | `hello'world` |
| 7 | `echo 'hello"world'` | `hello"world` |
| 8 | `echo "hello""world"` | `helloworld` |
| 9 | `echo '''hello'''` | `hello` |
| 10 | `echo ""` | (empty line) |
| 11 | `echo ''` | (empty line) |
| 12 | `echo "hello` | `bash: syntax error: unclosed quote` |
| 13 | `echo "$USER"` | Prints username |
| 14 | `echo '$USER'` | `$USER` (literal) |
| 15 | `echo "'$USER'"` | `'username'` (single quotes inside double) |

### Variable Expansion (16–25)

| # | Command | Expected |
|---|---------|----------|
| 16 | `echo $` | `$` |
| 17 | `echo "$"` | `$` |
| 18 | `echo $?` | Last exit code (e.g., `0`) |
| 19 | `echo $?$?` | `00` (if last was 0) |
| 20 | `echo $?abc` | `0abc` |
| 21 | `echo $NONEXISTENT` | (empty line) |
| 22 | `echo "$NONEXISTENT"` | (empty line) |
| 23 | `echo $USER$HOME` | `usernamevalue/home/path` (concatenated) |
| 24 | `echo $1` | (empty — positional param) |
| 25 | `export A="hello world" && echo $A` | `hello world` |

### Redirections (26–35)

| # | Command | Expected |
|---|---------|----------|
| 26 | `> outfile` | Creates empty `outfile`, exit 0 |
| 27 | `< nonexistent` | `bash: nonexistent: No such file or directory`, exit 1 |
| 28 | `echo hello > a > b > c` | `a` and `b` empty, `c` contains `hello` |
| 29 | `cat < infile > outfile` | Copies infile to outfile |
| 30 | `echo hello >> outfile` then `echo world >> outfile` | `outfile` contains `hello\nworld` |
| 31 | `cat << EOF > outfile` | Heredoc content written to outfile |
| 32 | `cat << EOF \| grep hello` | Heredoc piped to grep |
| 33 | `cat << "EOF"` then `$HOME` then `EOF` | `$HOME` printed literally (quoted delimiter) |
| 34 | `cat << EOF` then `$HOME` then `EOF` | Value of HOME printed (unquoted delimiter) |
| 35 | `> outfile echo hello` | `outfile` contains `hello` (redirection before cmd) |

### Pipes (36–40)

| # | Command | Expected |
|---|---------|----------|
| 36 | `ls \| cat \| cat \| cat \| cat` | Same output as `ls` |
| 37 | `echo hello \| cat \| cat \| cat` | `hello` |
| 38 | `cat /dev/urandom \| head -c 10` | 10 bytes of random data, then stops |
| 39 | `false \| true` | `$?` = 0 (last command) |
| 40 | `true \| false` | `$?` = 1 (last command) |

### Built-ins (41–50)

| # | Command | Expected |
|---|---------|----------|
| 41 | `cd .. && pwd` | Prints parent directory |
| 42 | `cd / && cd - ` | Prints old directory, goes back |
| 43 | `cd too many args` | `bash: cd: too many arguments`, exit 1 |
| 44 | `export A=1 B=2 C=3 && echo $A $B $C` | `1 2 3` |
| 45 | `export 1INVALID=hello` | `bash: export: '1INVALID=hello': not a valid identifier`, exit 1 |
| 46 | `unset PATH && ls` | `bash: ls: No such file or directory` (or command not found) |
| 47 | `exit 42` | Shell exits with code 42 |
| 48 | `exit abc` | `bash: exit: abc: numeric argument required`, exits 2 |
| 49 | `exit 1 2` | `bash: exit: too many arguments`, does NOT exit, `$?` = 1 |
| 50 | `echo -n -nnn -n hello` | `hello` (no newline) |

---

<!-- ═══════════════════════════════════════════════════════════════ -->
## Part M — Skills Assessment & Learning Plan
<!-- ═══════════════════════════════════════════════════════════════ -->

### ✅ Skills You Master

| Topic | Evidence | Confidence |
|-------|----------|------------|
| **Linked lists** | Token list, cmd list, env list — clean `new/add/last/size/delone` implementations. | ⭐⭐⭐⭐⭐ |
| **Lexer / Tokenizer design** | State machine approach with `NORMAL/IN_QUOTE/IN_DQUOTE/WAITING` — solid architecture. | ⭐⭐⭐⭐ |
| **Memory management with GC** | Custom garbage collector wrapping malloc — all allocations tracked. Prevents leaks elegantly. | ⭐⭐⭐⭐⭐ |
| **File I/O and open/close** | Correct use of `open()` flags (`O_WRONLY | O_CREAT | O_TRUNC`, etc.), proper `close()` on fds. | ⭐⭐⭐⭐ |
| **Fork / exec pattern** | Clean fork → child_process → execv flow. Parent/child separation is correct. | ⭐⭐⭐⭐ |
| **Pipe plumbing** | Correct pipe creation, fd passing between processes, closing unused ends. Multi-stage pipes work. | ⭐⭐⭐⭐ |
| **Basic built-ins** | `echo`, `pwd`, `cd`, `env`, `unset`, `export` all have working basic implementations. | ⭐⭐⭐⭐ |
| **Error messaging** | Centralized `ft_error` function with stderr output. Clean pattern. | ⭐⭐⭐⭐ |
| **Project structure** | Well-organized into `parsing/`, `exec/`, `built_in/`, `utils/`. Separation of concerns. | ⭐⭐⭐⭐⭐ |
| **Makefile & libft integration** | Working build system with custom libft. | ⭐⭐⭐⭐ |

---

### 🔧 Skills to Review / Upgrade

#### 1. File Descriptor Management in Built-ins
**Gap**: Single built-ins with redirections (`echo hello > outfile`) don't get their stdout/stdin redirected because they run in the parent process without any `dup2`.

**How to learn**:
- Draw a diagram of which process has which fd for every case: single builtin, single external, pipeline with builtin, pipeline with external.
- Write a tiny test program that does: `dup2(fd, STDOUT_FILENO)` → `printf("hello")` → restore old stdout. This pattern is what you need for single builtins.
- Key concept: **save original fd** with `dup(STDOUT_FILENO)`, redirect, run builtin, then `dup2(saved, STDOUT_FILENO)` to restore.

#### 2. Signal Handling in Complex Contexts
**Gap**: No global variable for signal state. Heredoc doesn't handle Ctrl-C. Signal handler doesn't update `$?`. Ctrl-D at prompt has a type mismatch bug.

**How to learn**:
- Read `man 7 signal` — understand the difference between `signal()` and `sigaction()`.
- Understand **async-signal-safety**: only certain functions can be called from a signal handler. `write()` is safe, `printf()` is NOT.
- Practice: write a program that uses `sigaction` with a global `volatile sig_atomic_t g_signal` to pass info from handler to main loop.
- Key rule: the signal handler should **only** set the global variable. All logic (printing, exiting) happens in the main loop.

#### 3. `execve` with Environment
**Gap**: Using `execv` instead of `execve`. Child processes don't receive the updated environment.

**How to learn**:
- Read `man execve` — understand the three arguments: path, argv, envp.
- Write a function that converts your `t_env` linked list to a `char **` array. Each element is `ft_strjoin(name, "=", value)`.
- Test: `export MY_VAR=hello`, then run `env | grep MY_VAR` — it should appear. Currently it won't because `execv` inherits the **original** envp, not your modified one.

#### 4. Exit Code Conventions
**Gap**: Many built-ins don't propagate their return value to `minishell->exit_status`. Error cases return wrong codes (127 for everything instead of distinguishing 126 vs 127).

**How to learn**:
- Print this table on paper and keep it next to you:
  - `0` = success
  - `1` = general error
  - `2` = misuse (syntax error, invalid builtin usage)
  - `126` = cannot execute (permission denied, is directory)
  - `127` = not found
  - `128+N` = killed by signal N
- Go through each builtin in `run_built_in` and ensure `minishell->exit_status` is set from the return value.
- Go through `close_pipe_and_exec` and add proper checks: `stat()` for directory, `access(F_OK)` for existence, `access(X_OK)` for permission.

#### 5. Edge Cases in Expansion
**Gap**: `$` at end of string, `$` followed by digit, `$$`, expansion inside heredoc body.

**How to learn**:
- Open bash and test every weird `$` case. Write down what bash does.
- In `ft_expend`, add a check at the very beginning: if `str[*start]` is `\0`, `"`, `'`, or a space → append literal `$` and return.
- For digits: if `str[*start]` is a digit → skip one char (positional params not supported) → return.
- For heredoc expansion: after reading each line in `ft_heredoc`, run the expand logic on it before writing to the pipe (only if delimiter was unquoted).

#### 6. Robust Parsing of Complex Cases
**Gap**: redirection-only commands (`> outfile`), redirection before command (`> outfile echo hello`), multiple redirections.

**How to learn**:
- Understand that in bash, redirections can appear **anywhere** in the command: `> out echo hello < in` is valid and means `echo hello < in > out`.
- The parser should extract all redirections first, then what's left is the command + arguments.
- Alternative (your current approach): process tokens left-to-right, handle redirections inline. This works but requires careful handling of the `CMD` flag — the first non-redirection WORD is the CMD.

#### 7. Heredoc Signal Handling & Edge Cases  
**Gap**: Ctrl-C during heredoc, quoted vs unquoted delimiters, expansion in heredoc body.

**How to learn**:
- Test in bash: `cat << EOF`, type a few lines, then Ctrl-C. Observe: the heredoc is cancelled, `$?` is 130.
- Test: `cat << "EOF"`, type `$HOME`, type `EOF`. The `$HOME` is printed literally.
- Test: `cat << EOF`, type `$HOME`, type `EOF`. The value of HOME is printed.
- Implementation: check if the delimiter token had quotes (check original token before quote removal). If yes → no expansion in body. If no → expand each line.

---

### 📊 Priority Matrix

| Priority | Task | Impact | Effort |
|----------|------|--------|--------|
| 🔴 P0 | Fix Ctrl-D crash (H3 — type mismatch in `ft_buildin_exit` call) | Crashes the shell | 10 min |
| 🔴 P0 | Switch `execv` → `execve` with env array (F1, F6) | Commands don't inherit env | 1 hour |
| 🔴 P0 | Fix single builtin redirections (`echo hi > out`) (G1) | Core feature broken | 30 min |
| 🔴 P0 | Remove `ft_printf` debug line in `exit.c` (G7) | Prints garbage to user | 1 min |
| 🟠 P1 | Fix `export` — wrong fd, no multi-arg, format (G4) | Several tests fail | 1 hour |
| 🟠 P1 | Fix `cd` — too many args, NULL path, error propagation (G2) | Several tests fail | 30 min |
| 🟠 P1 | Add global variable for signals + Ctrl-C `$?=130` (H1, H4) | Signal tests fail | 1 hour |
| 🟠 P1 | Fix `$` edge cases in expansion (C1, C2) | Expansion tests fail | 30 min |
| 🟠 P1 | Fix 126 vs 127 error codes (F2, F3, F5) | Wrong exit codes | 30 min |
| 🟡 P2 | Heredoc signal handling (D6, H2) | Ctrl-C during heredoc | 1 hour |
| 🟡 P2 | Heredoc expansion (C9) | Expansion in heredoc body | 1 hour |
| 🟡 P2 | Increment `SHLVL` (I2) | Some testers check this | 15 min |
| 🟡 P2 | Handle redirection-only commands (B1) | `> outfile` case | 30 min |
| 🟡 P2 | Clean up enums and macros (Part J) | Readability | 30 min |
| 🟢 P3 | `export` alphabetical sort | Minor | 30 min |
| 🟢 P3 | `export` `declare -x` format | Minor | 15 min |
| 🟢 P3 | `export +=` syntax | Minor | 30 min |

---

### 🎯 Recommended Learning Path

```
Week 1: Fix P0s (crash fixes, execve, builtin redirections)
         └─ Practice: write a 50-line program that fork+execve+waitpid
         └─ Practice: write a program that dup2's stdout to a file, runs a builtin, restores

Week 2: Fix P1s (export, cd, signals, expansion, error codes)
         └─ Read: man sigaction, man signal(7)
         └─ Practice: write a program with one global sig_atomic_t, handle SIGINT

Week 3: Fix P2s (heredoc signals, heredoc expansion, SHLVL, redir-only cmds)
         └─ Test extensively against bash
         └─ Run the 50 tricky tests from Part L

Week 4: Polish P3s and run all testers
         └─ Run minishell_tester, test with `valgrind --leak-check=full`
         └─ Clean code, remove all debug prints, verify norm
```

---

*End of REPORT 3 — Good luck! 🚀*
