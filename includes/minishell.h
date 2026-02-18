#ifndef MINISHELL_H
# define MINISHELL_H

// FAITS MAISON
# include "../libft/libft.h"
# include "errors.h"

// PRODUITS IMPORTES
# include <signal.h>
# include <errno.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/wait.h>

// MACROS
# define BUFFER_SIZE 5000

# define OPERATORS "|<>"
# define SEPARATORS " $\'\"\t"

// ENUM TYPES ELEMENTS
typedef enum	e_type
{
	DEFAULT = 0,
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	CMD,
	// LAST_HEREDOC,
	// R_FILE,
	FLAG,
	NBR_TYPES,
}					t_types;

// ENUMS MACHINE STATES
typedef enum	e_state
{
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
	NBR_STATE,
}					t_state;

// STRUCTURE POUR CMD
typedef struct	s_cmd
{
	char			*path;
	char			**args;
	int				infd;
	int				outfd;
	// int				pipefd[2];
	struct s_cmd	*next;
}					t_cmd;

// STRUCTURE POUR UN ELEMENT
typedef struct	s_token
{
	char			*str;
	t_types			type;
	struct s_token	*next;
	char			*path;
}					t_token;

typedef struct	s_env
{
	char			*name;
	char			*content;
	struct s_env	*next;
}					t_env;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct	s_minishell
{
	t_token			*head_token;
	t_cmd			*head_cmd;
	t_env			*head_env;
	t_list			*gc;
	char			*line;
	t_state			state;
	int				exit_status;
	int				cached_status;
}					t_minishell;

// STRUCTURE POUR EXEC CHILD
typedef struct	s_child
{
	int				index;
	int				prev_pipe;
	int 			size_cmd;
}					t_child;

// ── PARSING ─────────────────────────────────────────

// UTILS/CMDS.C
int					ft_cmd_lst_create(t_minishell *minishell);
/////////

// UTILS/ENV_SETUP.C
t_env				*ft_env_new(t_minishell *minishell, char *str);
int					ft_env_add(t_minishell *minishell, t_env *new_env);
t_env				*ft_env_find(t_env	*head_env, char *to_find);
void				ft_env_setup(t_minishell *minishell, char **envp);
void				ft_env_delone(t_env **head_env, char *target_name);
/////////

// UTILS/OUTPUT.C
void				ft_env_print(t_env *head, int fd);
void				ft_cmd_print(t_cmd *head);
void				ft_tokens_print(t_token *head);
void				ft_type_print(t_token *token);
void				ft_state_print(char c, char *buffer,
						t_minishell *minishell);
/////////

// UTILS/TOKENS.C
t_token				*ft_token_create(t_minishell *minishell, char *buffer);
int					ft_token_add(t_minishell *minishell, t_token *to_add);
/////////

// PARSING/LEXER.C
void				ft_state_detect(char c, t_minishell *minishell);
int					ft_buffer_add(char *buffer, char c);
int					ft_tokenize(t_minishell *minishell);

// parsing/parser.c
int					checker_token(t_minishell *minishell);

// parsing/check.c
int					ft_check_flags(char *str);
int					ft_check_redirection(char *str);

// parsing/check2.c
void				ft_cmd_find_path(t_minishell *minishell, t_token *token);
int					is_redirection(t_token *token);
int					ft_check_pipe(char *str);

// ── EXEC ────────────────────────────────────────────

// UTILS/PARSER_UTILS.C
void				ft_quotes_handle(t_minishell *minishell, t_token *token);
/////////

// HELPERS/cntrl.C
int				ft_redirection_handler(t_minishell *minishell, t_cmd *cmd, t_token *token);
/////////

// UTILS/CMDS.C
int				ft_token_affect(t_minishell *minishell, t_cmd *cmd, t_token **token, int *i);
int				ft_cmd_lst_create(t_minishell *minishell);
/////////
int   				ft_cmd_size(t_cmd *cmd_head);

// EXECUTION/EXEC.C
void				ft_exec(t_minishell *minishell);
/////////

// exec/child_exec.c
void				child_set(t_child *child, int i, int prev_pipe, int size_cmd);
void				child_process(t_minishell *minishell, t_cmd *cmd,
								t_child *child, int pipe_fd[2]);
t_child				*ft_child_new(t_minishell *minishell);
void				close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell,
								int pipe_fd[2]);

// exec/handler.c
void				handler_signal_child(void);
void				handler_first_cmd(int infd, int size_cmd, int pipe_fd);
void				handler_last_cmd(int prev_pipe, int outfd);
void				handler_mid_cmd(int prev_pipe, int pipe_fd);

// exec/wait.c
void				handler_status(int status, t_cmd *cmd, t_minishell *minishell);
void				ft_wait_subprocess(t_minishell *minishell, int size_cmd,
								int *pids);

// exec/cntrl.c
int					ft_redirection_handler(t_minishell *minishell, t_cmd *cmd,
								t_token *token);
void				ft_redirection_exec(int new_fd, int *old_fd);
int					ft_open(char *path, t_types mod);

// ── BUILT-INS ───────────────────────────────────────

// built_in/extra_bin.c
void	ft_extra_bin(t_minishell *minishell, int mod);

// built_in/is_built_in.c
int					is_built_in(t_cmd *cmd);
void				run_built_in(t_cmd *cmd, t_minishell *minishell);

// built_in/echo.c
void				ft_echo(char **args);

// built_in/cd.c
int				ft_cd(t_minishell *minishell, char *path);

// built_in/pwd.c
int					ft_pwd(void);

// built_in/export.c
int					ft_export(t_minishell *minishell, int fd, char *str);

// built_in/unset.c
void				ft_unset(t_env **head_env, char **args);

// built_in/env.c
void				ft_env(t_minishell *minishell, int outfd);

// built_in/exit.c
void				ft_buildin_exit(t_minishell *minishell, char **args);

// ── UTILS ───────────────────────────────────────────

// utils/tokens.c
t_token				*ft_token_create(t_minishell *minishell, char *buffer);
int					ft_token_add(t_minishell *minishell, t_token *to_add);

// utils/cmds.c
int					ft_token_affect(t_minishell *minishell, t_cmd *cmd,
								t_token **token, int *i);
int					ft_cmd_lst_create(t_minishell *minishell);
int					ft_cmd_size(t_cmd *cmd_head);

// utils/env_setup.c
t_env				*ft_env_new(t_minishell *minishell, char *str);
int					ft_env_add(t_minishell *minishell, t_env *new_env);
t_env				*ft_env_find(t_env *head_env, char *to_find);
void				ft_env_setup(t_minishell *minishell, char **envp);
void				ft_env_delone(t_env **head_env, char *target_name);

// utils/parser_utils.c
void				ft_quotes_handle(t_minishell *minishell, t_token *token);

// utils/heredoc.c
void				ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd,
								t_token *token);
void				ft_heredoc(t_minishell *minishell, t_cmd *cmd,
								t_token *token, int mod);

// utils/output.c
void				ft_env_print(t_env *head, int fd);
void				ft_env_print_export_no_param(t_env *head, int fd);
void				ft_cmd_print(t_cmd *head);
void				ft_tokens_print(t_token *head);
void				ft_type_print(t_token *token);
void				ft_state_print(char c, char *buffer,
								t_minishell *minishell);

// utils/errors.c
int	ft_error(t_minishell *minishell, int error, char *str1, char *str2);
void				ft_exit(t_minishell *minishell, int error, char *str);

// utils/signal_core.c
void				signal_callback_handler(int sig);
void				setup_signal(void);
/////////

#endif
