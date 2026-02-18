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
# include "parsing.h"

// ── EXEC ────────────────────────────────────────────
# include "exec.h"

// ── BUILT-INS ───────────────────────────────────────
# include "built_ins.h"

// ── UTILS ───────────────────────────────────────────
# include "utils.h"

#endif