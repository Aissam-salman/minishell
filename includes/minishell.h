#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include "errors.h"

typedef enum e_type {
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	NBR_TYPES,
}	t_types;

typedef enum e_state {
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	NBR_STATE,
	NEUTRAL,
}	t_state;

typedef struct s_element {
	char				*str;
	t_types				type;
	struct s_element	*next;
}	t_elements;

typedef struct s_minishell {
	t_elements	*head;
	t_list		*gc;
	char		*line;
	t_state			state;
}	t_minishell;

void	ft_parse(t_minishell *minishell);

#endif