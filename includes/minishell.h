#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
# include "errors.h"

// ENUM TYPES ELEMENTS
typedef enum e_type {
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	NBR_TYPES,
}	t_types;

// ENUMS ETATS MACHINE STATE
typedef enum e_state {
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	NBR_STATE,
	NEUTRAL,
}	t_state;

// STRUCTURE POUR UN ELEMENT
typedef struct s_element {
	char				*str;
	t_types				type;
	struct s_element	*next;
}	t_elements;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct s_minishell {
	t_elements	*head;
	t_list		*gc;
	char		*line;
	t_state			state;
}	t_minishell;

void	ft_parse(t_minishell *minishell);

#endif
