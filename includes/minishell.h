#ifndef MINISHELL_H
# define MINISHELL_H

// FAITS MAISON
# include "../libft/libft.h"
# include "errors.h"

// PRODUITS IMPORTES
# include <errno.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <signal.h>
# include <readline/readline.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <sys/wait.h>

// MACROS
# define BUFFER_SIZE 1000

# define OPERATORS "|<>"
# define SEPARATORS " $\'\"\t"

// ENUM TYPES ELEMENTS
typedef enum e_type
{
	DEFAULT = 0,
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	CMD,
	LAST_HEREDOC,
	// R_FILE,
	FLAG,
	NBR_TYPES,
}					t_types;

// ENUMS MACHINE STATES
typedef enum e_state
{
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
	NBR_STATE,
}					t_state;

// STRUCTURE POUR CMD
typedef struct s_cmd
{
	char			*path;
	char			**args;
	int				infd;
	int				outfd;
	int				pipefd[2];
	struct s_cmd	*next;
}					t_cmd;

// STRUCTURE POUR UN ELEMENT
typedef struct s_token
{
	char			*str;
	t_types			type;
	struct s_token	*next;
	int				code_error;
	char			*path;
}					t_token;

typedef struct s_env
{
	char			*name;
	char			*content;
	struct s_env	*next;
}		t_env;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct s_minishell
{
	t_token			*head_token;
	t_cmd			*head_cmd;
	t_list			*gc;
	t_env			*head_env;
	char			*line;
	char			**envs;
	t_state			state;
	int				exit_status;
}					t_minishell;

// STRUCTURE POUR EXEC CHILD
typedef struct s_child
{
	int index;
	int prev_pipe;
	int size_cmd;
} t_child;

// PARSING/LEXER.C
void				ft_state_detect(char c, t_minishell *minishell);
int					ft_buffer_add(char *buffer, char c);
int					ft_tokenize(t_minishell *minishell);
/////////

// UTILS/CMDS.C
void				ft_create_cmd_lst(t_minishell *minishell);
/////////

// UTILS/ENV_SETUP.C
t_env				*ft_env_new(t_minishell *minishell, char *str);
int					ft_env_add(t_minishell *minishell, t_env *new);
t_env				*ft_env_find(t_env	*head_env, char *to_find);
void				ft_env_setup(t_minishell *minishell, char **envp);
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

// PARSING/PARSER.C
void				checker_token(t_minishell *minishell);
/////////

// PARSING/HELPER/CHECK.C
int					ft_check_flags(char *str);
int					ft_check_redirection(char *str);
int					ft_check_heredoc_end(char *str);
int					ft_check_file_of_redirection(t_token *token);
int					ft_check_file(t_token *token);
/////////

// PARSING/HELPER/CHECK2.C
void				ft_check_cmd(t_minishell *minishell, t_token *token);
int					is_redirection(t_token *token);
int					ft_check_pipe(char *str);
/////////

// PARSING/HELPER/EXPANDER.C
char				*ft_check_expands(t_minishell *minishell, char *str);
/////////

// PARSING/HELPER/FILTER.C
void				ft_filter_quote(t_token *token, t_minishell *minishell);
int					is_need_expands(t_token *token);
/////////

// UTILS/ERRORS.C
int					ft_error(int error, char *str1, char *str2);
void				ft_exit(t_minishell *minishell, int error, char *str);
/////////

// UTILS/PARSER_UTILS.C
void	ft_quotes_handle(t_minishell *minishell, t_token *token);
/////////

// HELPERS/cntrl.C
void				ft_heredoc(t_cmd *cmd, t_token *token);
void				ft_redirection_handler(t_cmd *cmd, t_token *token);
/////////

// UTILS/CMDS.C
void				ft_cmd_lst_create(t_minishell *minishell);
/////////
int   				ft_cmd_size(t_cmd *cmd_head);

// EXECUTION/EXEC.C
void				ft_exec(t_minishell *minishell);
void				handler_signal_child();
void				handler_first_cmd(int infd, int size_cmd, int pipe_fd);
void				handler_last_cmd(int prev_pipe, int outfd);
void				close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2]);
/////////

// EXECUTION/CHILD_EXEC.C
void				child_set(t_child *child, int i, int prev_pipe, int size_cmd);
void 				child_process(t_minishell *minishell, t_cmd *cmd,
					   				t_child *child, int pipe_fd[2]);
t_child 			*ft_child_new(t_minishell *minishell);
void 				close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell, int pipe_fd[2]);
//////////

// EXECUTION/HANDLER.C
void				handler_signal_child();
void				handler_first_cmd(int infd, int size_cmd, int pipe_fd);
void				handler_last_cmd(int prev_pipe, int outfd);
void				handler_mid_cmd(int prev_pipe, int pipe_fd);
/////////

// EXECUTION/WAIT.C
void				handler_status(int status, t_cmd *cmd);
void				ft_wait_subprocess(t_minishell *minishell, int size_cmd, int *pids);

// BUILT-INS
int					ft_export(t_minishell *minishell, int fd, char *str);
void				ft_env(t_env *head_env, int	outfd);
/////////

#endif
