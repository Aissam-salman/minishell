# 🔧 Need Fix After Tester — 72/146 (74 ❌)

> Generated from `./tester` run on 2025-02-18.
> Updated after second `./tester` run on 2025-02-18 — **same score**.

---

## ~~Issue #0 — 🔴 SEGFAULT on every single command~~ ✅ FIXED

The `ft_buildin_exit(&minishell, 0)` crash on EOF has been fixed.
The shell now exits cleanly when `readline` returns `NULL`.

---

## Issue #1 — `export` rejects all valid identifiers (Tests 24–29)

| Test | Command | Expected exit | Got |
|------|---------|---------------|-----|
| 24 | `export hello` | 0 | 1 |
| 25 | `export HELLO=123` | 0 | 1 |
| 27 | `export HELLO=123 A` | 0 | 1 |
| 28 | `export HELLO="123 A-"` | 0 | 1 |
| 29 | `export hello world` | 0 | 1 |

**Root cause:** `ft_env_format_check` in `src/built_in/export.c` has inverted logic:

```c
if (!ft_isalpha(str[0]) || str[0] != '_')
```

This should be `&&` not `||`. A letter like `H` passes `ft_isalpha` but fails
`str[0] != '_'`, so valid names get rejected. Also, export only processes
`args[1]` — it needs to loop over all arguments.

**Files:** `src/built_in/export.c`

---

## Issue #2 — `echo "$"` and `echo $` eat the `$` (Tests 13, 17, 19)

| Test | Command | Expected | Got |
|------|---------|----------|-----|
| 13 | `echo "… $ …"` | includes `$` | `$` is missing |
| 17 | `echo "$"` | `$` | *(empty)* |
| 19 | `echo $` | `$` | *(empty)* |

**Root cause:** `ft_expend` in `src/utils/parser_utils.c` tries to expand `$`
even when the next character is `"`, `'`, space, or end-of-string. A lone `$`
should be printed literally.

**File:** `src/utils/parser_utils.c`

---

## Issue #3 — Quoted filenames in redirections fail (Tests 61–90+)

Massive group of failures: **any redirection** whose filename is in quotes
(`<"./path"`, `>"./path"`) triggers `No such file or directory`.

| Example tests | Pattern |
|---|---|
| 61, 62, 64, 67–75, 79, 86–90 | `<"file"` or `>"file"` |

**Root cause:** Quotes are not stripped from the redirection target token before
`ft_open` is called. The shell tries to open `"./test_files/infile"` literally
(with quotes in the filename).

**File:** `src/utils/cmds.c` (`ft_token_affect`) and/or `src/exec/cntrl.c`
(`ft_redirection_handler`). The token's `.str` must go through `ft_quotes_handle`
*before* being used as a file path for `open()`.

---

## Issue #4 — Exit codes wrong on redirection errors (Tests 76, 85, 104–108, 115–120, 130)

When a redirection fails (missing file, permission denied), the shell continues
execution and returns exit code `0` instead of `1`.

**Root cause:** `ft_redirection_handler` returns `GENERAL_ERROR`, but the caller
in `ft_token_affect` doesn't propagate it properly — the command still runs.
Bash stops execution of the entire command when any redirection fails.

**File:** `src/utils/cmds.c`, `src/exec/cntrl.c`

---

## Issue #5 — Built-in output with `>` goes to stdout, not to file (Tests 83, 91–92, 95–101, 121–129)

`echo hi > outfile01 bye` → should write `hi bye` to the file and print nothing.  
Instead it prints `hi bye` to stdout and the file is empty or wrong.

Also: `echo hi > outfile01 | echo bye` → the first `echo` output should go into
the file, not be lost.

**Root cause:** Built-ins (`echo`, etc.) write to `STDOUT_FILENO` directly.
When a built-in runs in the parent process (single command, no pipe), it doesn't
apply the `dup2` of `cmd->outfd` onto `STDOUT_FILENO`. Built-ins need to write
to `cmd->outfd` instead of hard-coded `1`.

**Files:** `src/built_in/echo.c` (writes to `STDOUT_FILENO`), `src/exec/exec.c`
(parent built-in path doesn't redirect fds), `src/built_in/pwd.c`,
`src/built_in/env.c`

---

## Issue #6 — `cd` doesn't report errors (Tests 40–41)

| Test | Command | Expected exit | Got |
|------|---------|---------------|-----|
| 40 | `cd $PWD hi` | 1 (`too many arguments`) | 0 |
| 41 | `cd 123123` | 1 (`No such file or directory`) | 0 |

**Root cause:** `ft_cd` doesn't check for too many arguments and doesn't return
an error when `chdir` fails on a non-existent path (the `stat` guard only runs
when the path is a directory).

**File:** `src/built_in/cd.c`

---

## Issue #7 — `exit hello` prints debug `"c"` (Test 50)

```
exit hello → mini output = (c)   bash output = ()
```

**Root cause:** There's a leftover `ft_printf("c\n")` debug line in
`ft_buildin_exit` before the error message.

**File:** `src/built_in/exit.c` line 65

---

## Issue #8 — `env` prints vars on one line, not one per line (Test 53)

`env | sort | grep -v SHLVL | grep -v ^_` → all env vars are on a single line
instead of one per line, causing output mismatch.

**Root cause:** `ft_env_print` uses `ft_putstr_fd` for the `\n` — verify this
actually outputs a newline. The tester output shows all vars concatenated on one
line.

**File:** `src/utils/output.c`

---

## Issue #9 — `export X=1 | env` should NOT show X (Test 56)

`export GHOST=123 | env | grep GHOST` → mini finds `GHOST`, bash doesn't.

**Root cause:** In a pipeline, each command runs in a subshell. An `export`
inside a pipe should NOT affect the parent env. Currently the built-in path
runs `export` in the parent even when there's a pipe.

**File:** `src/exec/exec.c` (the `is_built_in && size_cmd == 1` shortcut)

---

## Issue #10 — `$EMPTY` should be ignored, not "command not found" (Tests 134–135)

| Test | Command | Expected exit | Got |
|------|---------|---------------|-----|
| 134 | `$EMPTY` | 0 (no-op) | 127 (command not found) |
| 135 | `$EMPTY echo hi` | 0, prints `hi` | 127 |

**Root cause:** After expansion, an empty `$EMPTY` produces an empty string
that gets treated as a command. Bash silently ignores it.

**File:** `src/parsing/parser.c` / `src/utils/parser_utils.c`

---

## Issue #11 — Directory / permission handling (Tests 133, 136, 140–141)

| Test | Command | Expected exit | Got |
|------|---------|---------------|-----|
| 133 | `$PWD` (expands to dir) | 126 + `Is a directory` | 13 + `Permission denied` |
| 136 | `./test_files/invalid_permission` | 126 + `Permission denied` | 127 + `command not found` |
| 140 | `test_files` (dir in PATH) | 127 + `command not found` | 13 + `Permission denied` |
| 141 | `./test_files` (dir) | 126 + `Is a directory` | 13 + `Permission denied` |

**Root cause:** `ft_cmd_find_path` and `close_pipe_and_exec` don't distinguish
between:
- A **directory** → exit 126, `"Is a directory"`
- A **file without execute perm** → exit 126, `"Permission denied"`
- **Not found** → exit 127, `"command not found"`

**Files:** `src/parsing/check2.c`, `src/exec/child_exec.c`

---

## Issue #12 — Pipe + redirection combos lose output (Tests 72, 78, 80, etc.)

`echo hi | cat "./test_files/infile"` → mini only prints file content, bash
prints both stdin (`hi`) and file content.

`cat <missing | echo oi` → mini prints nothing, bash prints `oi` (second command
should still run even if first fails).

**Root cause:** When a command in a pipeline has a redirection error, the entire
pipeline is aborted instead of just failing that one command.

**File:** `src/utils/cmds.c` (`ft_cmd_lst_create` / `ft_token_affect`)

---

## Summary — Priority Order

| Priority | Issue | Status | Impact | Tests affected |
|----------|-------|--------|--------|----------------|
| ~~P0~~ | ~~SEGV in `ft_buildin_exit` on EOF~~ | ✅ FIXED | ~~All 146 tests crash~~ | — |
| 🔴 P1 | Quoted filenames in redirections | ❌ OPEN | 30+ tests | 61–90+ |
| 🔴 P2 | `export` format check inverted | ❌ OPEN | 5 tests | 24–29 |
| 🟠 P3 | Built-in output ignores `>` redirections | ❌ OPEN | 20+ tests | 83–101, 121–129 |
| 🟠 P4 | Redirection errors don't set exit code | ❌ OPEN | 15+ tests | 76, 85, 104–120 |
| 🟠 P5 | Pipeline error handling (partial abort) | ❌ OPEN | 10+ tests | 72, 78–80, 131 |
| 🟡 P6 | `cd` missing error cases | ❌ OPEN | 2 tests | 40–41 |
| 🟡 P7 | `exit hello` debug print | ❌ OPEN | 1 test | 50 |
| 🟡 P8 | `$` literal expansion | ❌ OPEN | 3 tests | 13, 17, 19 |
| 🟡 P9 | `$EMPTY` not ignored | ❌ OPEN | 2 tests | 134–135 |
| 🟡 P10 | Directory/permission exit codes | ❌ OPEN | 4 tests | 133, 136, 140–141 |
| 🟢 P11 | `export` in pipe affects parent env | ❌ OPEN | 1 test | 56 |
| 🟢 P12 | `env` output format (one-line) | ❌ OPEN | 1 test | 53 |

**P0 is fixed!** 🎉 Now fix P1 + P2 to unlock ~35 tests. Then P3 + P4 for another ~35.
