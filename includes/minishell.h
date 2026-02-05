#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"

typedef enum e_type {
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	NBR_TYPES,
}	t_types;

typedef struct s_element {
	char				*str;
	t_types				type;
	struct s_element	*next;
}	t_elements;

typedef struct s_minishell {
	t_elements	*head;
	t_list		*gc;
	char		*line;
}	t_minishell;
#endif