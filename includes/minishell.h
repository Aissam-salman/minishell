#ifndef MINISHELL_H
# define MINISHELL_H

#include "../libft/libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include "errors.h"
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>

# define OPERATORS "|<>"

// ENUM TYPES ELEMENTS
typedef enum e_type {
	DEFAULT = 0,
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	NBR_TYPES,
	CMD,
	R_FILE,
	FLAG
}	t_types;

// ENUMS ETATS MACHINE STATE
typedef enum e_state {
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	NBR_STATE,
	WAITING,
}	t_state;

// STRUCTURE POUR CMD
typedef struct s_cmd
{
	char *path;
	char **args;
	int infd;
	int outfd;
	struct s_cmd *next;
} t_cmd;

// STRUCTURE POUR UN ELEMENT
typedef struct s_token {
	char			*str;
	t_types			type;
	struct s_token	*next;
}	t_token;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct s_minishell {
	t_token		*head_token;
	t_cmd		*head_cmd;
	t_list		*gc;
	char		*line;
	char		**envs;
	t_state			state;
	int			exit_status;
}	t_minishell;


// PARSING/LEXER.C
void	ft_tokenize(t_minishell *minishell);

void	ft_create_cmd_lst(t_minishell *minishell);
// UTILS/OUTPUT.C
void	ft_cmd_print(t_cmd *head);
void	ft_tokens_print(t_token *head);
void	ft_type_print(t_token *token);
void	ft_state_print(char c, char *buffer, t_minishell *minishell);

// UTILS/TOKENS.C
t_token *ft_token_create(t_minishell *minishell, char *buffer);
int	ft_token_add(t_minishell *minishell, t_token *to_add);

// UTILS/ERRORS.C
int	ft_error(int error, char *str);
void	ft_exit(t_minishell *minishell, int error, char *str );

// HELPERS/cntrl.C
void	ft_redirection_handler(t_cmd *cmd, t_token *token);

// UTILS/CMDS.C
void ft_cmd_lst_create(t_minishell *minishell);

// EXEC.C
void	ft_exec(t_minishell *minishell);
#endif
