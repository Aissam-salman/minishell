/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:58:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/09 16:22:59 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
#include <stdio.h>
#include <stdlib.h>

// TRAITER LES CHANGEMENTS D'ETAT
/*
	Lorsque "
	Lorsque '
	Lorsque SPACE
*/

// A SECURISER VIA LA TAILLE MAX
int	ft_buffer_add(char *buffer, char c)
{
	int	i;

	i = 0;
	while (buffer[i] )
		i++;
	if (i < BUFFER_SIZE)
	{
		buffer[i] = c;
		buffer[i + 1] = '\0';
		return (0);
	}
	else
		return (ft_error(BUFFER_FAIL, "Insufficient buffer size\n"));

}

// AFFECTE L'ETAT A MINISHELL POUR
void	ft_state_detect(char c, t_minishell *minishell)
{
	// SI GUILLEMETS
	if (c == '"')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_DQUOTE;
		else if (minishell->state == IN_DQUOTE)
			minishell->state = NORMAL;
	}
	// SI APOSTROPHE
	else if (c == '\'')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_QUOTE;
		else if (minishell->state == IN_QUOTE)
			minishell->state = NORMAL;
	}
	// SI EN PAUSE ET TROUVE UN AUTRE
	else if (!ft_ischarset(c, SPACES) && minishell->state == WAITING)
		minishell->state = NORMAL;
}

// INTERPRETE LE CARACTERE EN FONCTION DE L'ETAT
int	ft_state_interpret(char *line, int *index, char *buffer,
		t_minishell *minishell)
{
	// SI ESPACES
	if (ft_ischarset(line[*index], SPACES) && minishell->state == NORMAL)
	{
		if (ft_strlen(buffer) > 0)
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(MALLOC_FAIL, "Fail Malloc Interpreter\n"));
		minishell->state = WAITING;
	}
	// SI OPERATORS
	if (ft_ischarset(line[*index], OPERATORS) && minishell->state == NORMAL)
	{
		if (ft_strlen(buffer) > 0 && buffer[0] != line[*index])
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(MALLOC_FAIL, "Fail Malloc Interpreter\n"));
	}
	// SI STATE != WAITING
	if (minishell->state != WAITING)
	{
		if (!ft_ischarset(line[*index], OPERATORS) && ft_ischarset(buffer[0],
				OPERATORS))
			if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
				return (ft_error(MALLOC_FAIL, "Fail Malloc Interpreter\n"));
		if (ft_buffer_add(buffer, line[*index]))
			return (BUFFER_FAIL);
	}
	return (0);
}

// CREE LA LISTE DES TOKENS A UTILSER POUR LE PARSING
int	ft_token_lst_create(t_minishell *minishell)
{
	char	*line;
	char	*buffer;
	int		line_len;
	int		i;

	// On recupere la ligne
	line = minishell->line;
	line_len = ft_strlen(line);
	minishell->state = NORMAL;

	// INITIALISATION DU TABLEAU ARGS
	buffer = ft_calloc_gc(line_len + 1, sizeof(char), &minishell->gc);
	if (!buffer)
		return (ft_error(MALLOC_FAIL, "Fail Malloc Buffer Interpreter"));

	// BOUCLE POUR TRAITER CHAQUE CHAR DE MINISHELL.LINE
	i = 0;
	while (i < line_len) 
	{
		// ON DETECTE L'ETAT POUR POUVOIR DETERMINER QUOI FAIRE DU CHARACTERE
		ft_state_detect(line[i], minishell);

		// ON TRAITE line[i] EN FONCTION DE L'ETAT
		// ON INTERPRETE L'ETAT POUR CREER LA CHAINE DE TOKENS
		if (ft_state_interpret(line, &i, buffer, minishell))
			return (1);
		i++;
	}
	line[i] = '\0';
	if (minishell->state == IN_QUOTE || minishell->state == IN_DQUOTE)
		return (ft_error(PARSING_FAIL, "Syntax error: unclosed quotes\n"));
	if (ft_strlen(buffer) > 0)
		if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
			return (ft_error(MALLOC_FAIL, "Fail Malloc Interpreter\n"));
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
	// ON RECUPERE LES TYPES DANS UN PREMIER TEMPS
	// ft_create_elem_lst(minishell);
	if (ft_token_lst_create(minishell))
		return (1);
	ft_type_affect(minishell);
	return (0);
	// ft_create_cmd_lst(minishell);
}
