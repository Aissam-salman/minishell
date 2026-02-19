/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 20:49:28 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 22:00:58 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H
# include "../libft/libft.h"

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
	GARBAGE,
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
	int				error_file;
	struct s_cmd	*next;
}					t_cmd;

// STRUCTURE POUR UN ELEMENT
typedef struct s_token
{
	char			*str;
	t_types			type;
	struct s_token	*next;
	char			*path;
}					t_token;

typedef struct s_env
{
	char			*name;
	char			*content;
	struct s_env	*next;
}					t_env;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct s_minishell
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
typedef struct s_child
{
	int				index;
	int				prev_pipe;
	int				size_cmd;
	int				pipe_fd[2];
}					t_child;

#endif
