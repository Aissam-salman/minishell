/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:58:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/09 16:22:59 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// A SECURISER VIA LA TAILLE MAX
int	ft_buffer_add(char *buffer, char c)
{
	int	len;

	len = ft_strlen(buffer);
	if (len < BUFFER_SIZE)
	{
		buffer[len] = c;
		buffer[len + 1] = '\0';
		return (0);
	}
	else
		return (ft_error(NULL, BUFFER_FAIL, "Insufficient buffer size", NULL));
}

// AFFECTE L'ETAT A MINISHELL POUR
void	ft_state_detect(char c, t_minishell *minishell)
{
	if (c == '"')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_DQUOTE;
		else if (minishell->state == IN_DQUOTE)
			minishell->state = NORMAL;
	}
	else if (c == '\'')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_QUOTE;
		else if (minishell->state == IN_QUOTE)
			minishell->state = NORMAL;
	}
	else if (!ft_ischarset(c, SPACES) && minishell->state == WAITING)
		minishell->state = NORMAL;
}

// INTERPRETE LE CARACTERE EN FONCTION DE L'ETAT
int	ft_state_interpret(char *line, int *index, char *buffer,
		t_minishell *minishell)
{
	int	len_buffer;

	len_buffer = ft_strlen(buffer);
	if (ft_ischarset(line[*index], SPACES) && minishell->state == NORMAL)
	{
		if (len_buffer > 0)
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(minishell, MALLOC_FAIL,
						"Fail Malloc Interpreter", NULL));
		minishell->state = WAITING;
	}
	else if (ft_ischarset(line[*index], OPERATORS)
		&& minishell->state == NORMAL)
	{
		if (len_buffer > 0 && (buffer[0] != line[*index] || len_buffer >= 2))
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(minishell, MALLOC_FAIL,
						"Fail Malloc Interpreter", NULL));
		if (ft_buffer_add(buffer, line[*index]))
			return (BUFFER_FAIL);
	}
	else if (minishell->state != WAITING)
	{
		if (!ft_ischarset(line[*index], OPERATORS) && ft_ischarset(buffer[0],
				OPERATORS))
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(minishell, MALLOC_FAIL,
						"Fail Malloc Interpreter", NULL));
		if (ft_buffer_add(buffer, line[*index]))
			return (BUFFER_FAIL);
	}
	return (0);
}

// CREE LA LISTE DES TOKENS A UTILSER POUR LE PARSING
int	ft_token_lst_create(t_minishell *minishell)
{
	char	*buffer;
	int		line_len;
	int		i;

	line_len = ft_strlen(minishell->line);
	minishell->state = NORMAL;
	buffer = ft_calloc_gc(line_len + 1, sizeof(char), &minishell->gc);
	if (!buffer)
		return (ft_error(minishell, MALLOC_FAIL,
				"Fail Malloc Buffer Interpreter", NULL));
	i = -1;
	while (++i < line_len)
	{
		ft_state_detect(minishell->line[i], minishell);
		if (ft_state_interpret(minishell->line, &i, buffer, minishell))
			return (1);
	}
	minishell->line[i] = '\0';
	if (minishell->state == IN_QUOTE || minishell->state == IN_DQUOTE)
		return (ft_error(minishell, PARSING_FAIL, ERR_QUOTES, NULL));
	if (ft_strlen(buffer) > 0 && ft_token_add(minishell,
			ft_token_create(minishell, buffer)))
		return (ft_error(minishell, MALLOC_FAIL, "Fail Malloc Interpreter",
				NULL));
	return (0);
}

void	ft_type_affect(t_minishell *minishell)
{
	t_token	*current;

	current = minishell->head_token;
	while (current)
	{
		if (current->str[0] == '|')
			current->type = PIPE;
		else if (current->str[0] == '<')
		{
			current->type = IN_CHEVRON;
			if (current->str[1])
				current->type = IN_DCHEVRON;
		}
		else if (current->str[0] == '>')
		{
			current->type = OUT_CHEVRON;
			if (current->str[1])
				current->type = OUT_DCHEVRON;
		}
		else
			current->type = WORD;
		current = current->next;
	}
}

// On récupere la ligne, on traite pour avoir des types de mots
// On les récupere ensuite pour créer des phrases
int	ft_tokenize(t_minishell *minishell)
{
	if (ft_token_lst_create(minishell))
		return (1);
	ft_type_affect(minishell);
	return (0);
}
