# Redundancy Report — Minishell

> Auto-generated analysis of the codebase as of 2026-02-18.

---

## 1. Linked-list `_last` / `_add` / `_size` patterns

Three separate data types (`t_cmd`, `t_token`, `t_env`) each have their own
hand-written `_last`, `_add`, and sometimes `_size` functions that do
**exactly** the same thing — walk a `->next` singly-linked list.

| Function | File | Does |
|---|---|---|
| `ft_cmd_last` | `src/utils/cmds.c` | Walk `->next`, return last node |
| `ft_token_last` | `src/utils/tokens.c` | Walk `->next`, return last node |
| `ft_env_last` | `src/utils/env_setup.c` | Walk `->next`, return last node |
| `ft_cmd_add` | `src/utils/cmds.c` | Call `_last`, append node |
| `ft_token_add` | `src/utils/tokens.c` | Call `_last`, append node |
| `ft_env_add` | `src/utils/env_setup.c` | Call `_last`, append node |
| `ft_cmd_size` | `src/utils/cmds.c` | Count nodes in list |

All three `_last` functions are identical in logic, and all three `_add`
functions follow the same structure:

```
if head is NULL → set head
else             → last->next = new
```

**Suggestion:** Since these are different structs, they can't easily share one
function in C without `void *` casting. But you could unify with a macro or
keep them as-is — just be aware the pattern is copy-pasted three times.

---

## 2. `ft_is_flag` (echo.c) vs `ft_check_flags` (check.c)

Both functions determine whether a string is a "flag" (starts with `-`).

| Function | File | Logic |
|---|---|---|
| `ft_is_flag` | `src/built_in/echo.c` | Checks if `-` then only `n` chars |
| `ft_check_flags` | `src/parsing/check.c` | Checks if `-` then any alphanumeric chars |

They are **not identical** — `ft_is_flag` is echo-specific (`-n` only), while
`ft_check_flags` is a general flag validator. However, the naming is confusing
and `ft_check_flags` is used only in the parser to mark tokens as `FLAG` type,
while `ft_is_flag` is used again separately at echo runtime.

**Suggestion:** Rename for clarity. For example `ft_is_echo_nflag` and
`ft_is_generic_flag`, or consolidate by making `ft_check_flags` accept a
parameter for allowed flag characters.

---

## 3. `have_flag_first` (echo.c) vs `ft_is_flag` (echo.c)

Both live in `src/built_in/echo.c`:

| Function | Logic |
|---|---|
| `ft_is_flag` | Full check: `-` then only `n` chars, handles `--n`, etc. |
| `have_flag_first` | Quick check: `str[0] == '-' && str[1] == 'n'` |

`have_flag_first` is a **strict subset** of `ft_is_flag`. Every string
that passes `have_flag_first` also passes `ft_is_flag`.

**Suggestion:** Remove `have_flag_first` entirely and use `ft_is_flag`
everywhere in `ft_echo`.

---

## 4. `ft_check_redirection` (check.c) vs `is_redirection` (check2.c)

| Function | File | Operates on | Logic |
|---|---|---|---|
| `ft_check_redirection` | `src/parsing/check.c` | `char *str` | Checks if the **string** is `<`, `<<`, `>`, or `>>` |
| `is_redirection` | `src/parsing/check2.c` | `t_token *token` | Checks if `token->type` is one of the 4 redir enums |

They answer the same question ("is this a redirection?") at two different
levels of abstraction — raw string vs. typed token. After `ft_type_affect`
runs, every redirection string already has the correct `token->type`, so
`ft_check_redirection` becomes redundant for anything that happens post-lexer.

**Suggestion:** After the lexer assigns types, always use `is_redirection`.
`ft_check_redirection` is only needed inside `handle_redirection` in `parser.c`
for syntax validation of the raw string — consider whether that check is even
still necessary given that the lexer already validates operator length.

---

## 5. `ft_atoi_safe` (exit.c) vs `ft_atoi` / `ft_atol` / `ft_overint` (libft)

| Function | File | Returns |
|---|---|---|
| `ft_atoi` | `libft/conversion/ft_atoi.c` | `int` (no overflow check) |
| `ft_atol` | `libft/conversion/ft_atol.c` | `long` |
| `ft_overint` | `libft/conversion/ft_overint.c` | `1` if value overflows `int` |
| `ft_atoi_safe` | `src/built_in/exit.c` | `0` on success, `1` on overflow/bad input |

`ft_atoi_safe` re-implements the same skip-whitespace → sign → digit loop
from `ft_atoi`, but with overflow detection and error return. The libft already
provides `ft_atol` + `ft_overint` which together do the same job.

**Suggestion:** Replace `ft_atoi_safe` with a combination of `ft_atol` +
`ft_overint`, or extend `ft_atol` with an error-return variant in libft.

---

## 6. `ft_env_print` (output.c) vs `ft_env_print_export_no_param` (output.c)

Both are in `src/utils/output.c`:

| Function | Extra output |
|---|---|
| `ft_env_print` | Prints `NAME=CONTENT\n` |
| `ft_env_print_export_no_param` | Prints `export NAME=CONTENT\n` |

The only difference is the `"export "` prefix.

**Suggestion:** Merge into one function with a `bool prefix` or `char *prefix`
parameter:
```c
void ft_env_print(t_env *head, int fd, char *prefix);
```

---

## 7. `ft_exit` (errors.c) vs cleanup in `ft_buildin_exit` / `close_pipe_and_exec`

The cleanup sequence `ft_gc_free_all → rl_clear_history → exit(code)` is
written in three places:

| Location | File |
|---|---|
| `ft_exit` | `src/utils/errors.c` |
| `ft_buildin_exit` | `src/built_in/exit.c` (twice — error path and normal path) |
| `close_pipe_and_exec` | `src/exec/child_exec.c` (before `_exit`) |

**Suggestion:** Have `ft_buildin_exit` and `close_pipe_and_exec` call
`ft_exit` instead of inlining the cleanup. `ft_exit` already does exactly
that.

---

## 8. `save_pwd` (cd.c) vs `ft_pwd` (pwd.c)

| Function | File | Does |
|---|---|---|
| `save_pwd` | `src/built_in/cd.c` | `getcwd(NULL, 0)`, return string |
| `ft_pwd` | `src/built_in/pwd.c` | `getcwd(NULL, 0)`, print and free |

Both call `getcwd(NULL, 0)` with the same error handling pattern.

**Suggestion:** Have `ft_pwd` use `save_pwd` internally, or extract a shared
`get_cwd_safe` helper that both call.

---

## 9. Heredoc handling in two places

Heredoc (`IN_DCHEVRON`) is handled in **both**:
- `ft_token_affect` in `src/utils/cmds.c` (lines ~120–130)
- `ft_redirection_handler` in `src/exec/cntrl.c` (line ~47)

Both check for `IN_DCHEVRON` and call `ft_heredoc_handle`. The one in
`ft_redirection_handler` is likely dead code since `ft_token_affect` catches
`IN_DCHEVRON` before it reaches the general redirection path.

**Suggestion:** Verify which path actually runs and remove the dead one.

---

## Summary Table

| # | What | Severity | Fix effort |
|---|---|---|---|
| 1 | `_last` / `_add` x3 | Low | Cosmetic — keep or macro-ify |
| 2 | `ft_is_flag` vs `ft_check_flags` | Medium | Rename or consolidate |
| 3 | `have_flag_first` vs `ft_is_flag` | High | Delete `have_flag_first` |
| 4 | `ft_check_redirection` vs `is_redirection` | Medium | Remove post-lexer string check |
| 5 | `ft_atoi_safe` vs libft's `ft_atol` + `ft_overint` | Medium | Reuse libft |
| 6 | `ft_env_print` vs `ft_env_print_export_no_param` | High | Add prefix parameter |
| 7 | Inlined cleanup vs `ft_exit` | High | Call `ft_exit` |
| 8 | `save_pwd` vs `ft_pwd` | Low | Share helper |
| 9 | Double heredoc handling | Medium | Remove dead path |
