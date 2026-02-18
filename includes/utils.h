#ifndef UTILS_H
# define UTILS_H

# include "minishell.h"

// ── UTILS ───────────────────────────────────────────

//UTILS/TOKENS.C
t_token	*ft_token_create(t_minishell *minishell, char *buffer);
int		ft_token_add(t_minishell *minishell, t_token *to_add);
//------

//UTILS/CMDS.C
int		ft_token_affect(t_minishell *minishell, t_cmd *cmd,
			t_token **token, int *i);
int		ft_cmd_lst_create(t_minishell *minishell);
int		ft_cmd_size(t_cmd *cmd_head);
//------

//UTILS/ENV_SETUP.C
t_env	*ft_env_new(t_minishell *minishell, char *str);
int		ft_env_add(t_minishell *minishell, t_env *new_env);
t_env	*ft_env_find(t_env *head_env, char *to_find);
void	ft_env_setup(t_minishell *minishell, char **envp);
void	ft_env_delone(t_env **head_env, char *target_name);
//------

//UTILS/HEREDOC.C
void	ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd,
			t_token *token);
void	ft_heredoc(t_minishell *minishell, t_cmd *cmd,
			t_token *token, int mod);
//------

//UTILS/OUTPUT.C
void	ft_env_print(t_env *head, int fd);
void	ft_env_print_export_no_param(t_env *head, int fd);
void	ft_cmd_print(t_cmd *head);
void	ft_tokens_print(t_token *head);
void	ft_type_print(t_token *token);
void	ft_state_print(char c, char *buffer,
			t_minishell *minishell);
//------

//UTILS/ERRORS.C
int		ft_error(t_minishell *minishell, int error, char *str1, char *str2);
void	ft_exit(t_minishell *minishell, int error, char *str);
//------

//UTILS/SIGNAL_CORE.C
void	signal_callback_handler(int sig);
void	setup_signal(void);
//------

#endif
