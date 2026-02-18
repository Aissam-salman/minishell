# Execution Logic: `cmd1 << EOF1 << EOF2 >> outfile | cmd2`

This document details the step-by-step execution logic for the complex shell command:
`cmd1 << EOF1 << EOF2 >> outfile | cmd2`

## 1. Parsing & Heredoc Collection

Before any process execution or forking begins, the shell parses the command line and handles "Here-Documents" (heredocs).

1.  **Detection**: The shell identifies the pipeline (`|`) and the redirections associated with `cmd1`:
    *   `<< EOF1` (Input Redirection - Heredoc)
    *   `<< EOF2` (Input Redirection - Heredoc)
    *   `>> outfile` (Output Redirection - Append)
2.  **Sequential Reading**: Heredocs are read **immediately** during the parsing phase, one by one, usually before the pipeline forks are created.
    *   The shell sees `<< EOF1`. It reads from the standard input line-by-line until it encounters `EOF1`. The content is stored in **memory** (e.g., a linked list or a single string).
    *   The shell then sees `<< EOF2`. It reads from standard input again until it encounters `EOF2`. This content is also stored in memory.
3.  **Expansion**: Depending on whether the delimiters (`EOF1`, `EOF2`) are quoted, variable expansion (`$VAR`) may occur within the collected content.

*Note: The actual execution of commands hasn't started yet. The shell just prepares the input data.*

## 2. Process Creation (Forking)

Once parsing is complete and heredocs are collected, the shell sets up the pipeline.

1.  **Pipe Creation**: The shell calls `pipe(pipe_fds)` to create a unidirectional data channel.
    *   `pipe_fds[0]` is the **Read end**.
    *   `pipe_fds[1]` is the **Write end**.
2.  **Forking**: The shell typically works from left to right or forks all children.
    *   **Fork 1 (for cmd1)**: The parent calls `fork()`. The child process (Child A) is created for `cmd1`.
    *   **Fork 2 (for cmd2)**: The parent calls `fork()`. The child process (Child B) is created for `cmd2`.

## 3. File Descriptor Manipulation (The Child of cmd1)

This section focuses on **Child A** (cmd1). This process inherits open file descriptors, including the pipe.

**Initial State**: `STDIN_FILENO` (0), `STDOUT_FILENO` (1), `STDERR_FILENO` (2) are connected to the terminal (usually). The `pipe_fds` are open.

### Step-by-Step Sequence:

1.  **Setup Pipeline Connection**:
    *   Before applying specific command redirections, the pipeline structure is established.
    *   `dup2(pipe_fds[1], STDOUT_FILENO)`: Replaces FD 1 (stdout) with the write-end of the pipe.
    *   `close(pipe_fds[0])`: Child A does not need the read-end.
    *   `close(pipe_fds[1])`: The write-end is now duplicated on FD 1, so the original FD is closed.

2.  **Redirection 1: `<< EOF1` (Using Pipes)**:
    *   *Create a generic pipe for the heredoc*: `pipe(hdoc_pipe1)`.
    *   *Data transfer*: The process writes the content of "Heredoc 1" (stored in memory) to `hdoc_pipe1[1]`.
    *   *Close Write End*: `close(hdoc_pipe1[1])` (sends EOF to the reader).
    *   `dup2(hdoc_pipe1[0], STDIN_FILENO)`: Redirects FD 0 (stdin) to read from the pipe.
    *   `close(hdoc_pipe1[0])`
    *   *Result*: `cmd1` is effectively reading from Heredoc 1.

3.  **Redirection 2: `<< EOF2` (Using Pipes)**:
    *   *Create a new pipe*: `pipe(hdoc_pipe2)`.
    *   *Data transfer*: The process writes the content of "Heredoc 2" to `hdoc_pipe2[1]`.
    *   *Close Write End*: `close(hdoc_pipe2[1])`.
    *   `dup2(hdoc_pipe2[0], STDIN_FILENO)`: **Shadows the previous input**. FD 0 is explicitly overwritten to point to the new pipe.
    *   `close(hdoc_pipe2[0])`
    *   *Result*: `cmd1` will now read strictly from Heredoc 2. The previous pipe `hdoc_pipe1` is closed automatically when FD 0 was overwritten (or should be closed explicitly if not dup'd over).

4.  **Redirection 3: `>> outfile`**:
    *   `fd_file = open("outfile", O_WRONLY | O_CREAT | O_APPEND)`
    *   `dup2(fd_file, STDOUT_FILENO)`: **Shadows the pipe**. FD 1, which was pointing to the pipe, is now overwritten to point to `outfile`.
    *   `close(fd_file)`
    *   *Interaction with Pipe*: Since FD 1 no longer points to the pipe, `cmd1` writes output to `outfile`. The write-end of the pipe is closed in this child (unless `2>&1` was used, which it wasn't). `cmd2` will likely see an EOF on its input immediately because no process holds the write end of the pipe open anymore.

### File Descriptor Table State (cmd1)

| Step | FD 0 (STDIN) | FD 1 (STDOUT) | FD 2 (STDERR) | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **Start** | Terminal | Terminal | Terminal | `pipe_fds` exist in background |
| **Pipeline Logic** | Terminal | **Pipe Write-End** | Terminal | `dup2(pipe[1], 1)` |
| **`<< EOF1`** | **Heredoc Pipe 1** | Pipe Write-End | Terminal | First input applied |
| **`<< EOF2`** | **Heredoc Pipe 2** | Pipe Write-End | Terminal | **Pipe 1 replaced by Pipe 2** |
| **`>> outfile`** | Heredoc Pipe 2 | **outfile** | Terminal | **Pipe replaced** by file |

## 4. Parental Cleanup

After forking both children, the parent process enters the cleanup and waiting phase.

1.  **Close Pipe FDs**: This is critical to prevent hanging.
    *   `close(pipe_fds[0])`: The parent doesn't read from the pipe.
    *   `close(pipe_fds[1])`: The parent doesn't write to the pipe.
    *   *Why?* If the parent keeps `pipe_fds[1]` open, `cmd2` will never receive an EOF (End of File) because there is still a potential writer (the parent) connected to the pipe input. `cmd2` would hang forever waiting for input.

2.  **Wait**:
    *   `waitpid(pid_cmd1)`
    *   `waitpid(pid_cmd2)`
    *   The shell usually waits for the last command in the pipeline (`cmd2`) to set the exit status variable `$?`, but waits for all children to ensure no zombies remain.
