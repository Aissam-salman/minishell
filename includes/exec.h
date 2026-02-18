#ifndef EXEC_H
# define EXEC_H

# include "minishell.h"

// ── EXEC ────────────────────────────────────────────

//EXEC/EXEC.C
void	ft_exec(t_minishell *minishell);
//------

//EXEC/CHILD_EXEC.C
void	child_set(t_child *child, int i, int prev_pipe, int size_cmd);
void	child_process(t_minishell *minishell, t_cmd *cmd,
			t_child *child, int pipe_fd[2]);
t_child	*ft_child_new(t_minishell *minishell);
void	close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell,
			int pipe_fd[2]);
//------

//EXEC/HANDLER.C
void	handler_signal_child(void);
void	handler_first_cmd(int infd, int outfd, int size_cmd, int pipe_fd);
void	handler_last_cmd(int prev_pipe, int outfd);
void	handler_mid_cmd(int prev_pipe, int outfd, int pipe_fd);
//------

//EXEC/WAIT.C
void	handler_status(int status, t_cmd *cmd, t_minishell *minishell);
void	ft_wait_subprocess(t_minishell *minishell, int size_cmd,
			int *pids);
//------

//EXEC/CNTRL.C
int		ft_redirection_handler(t_minishell *minishell, t_cmd *cmd,
			t_token *token);
void	ft_redirection_exec(int new_fd, int *old_fd);
int		ft_open(char *path, t_types mod);
//------

#endif
//------
