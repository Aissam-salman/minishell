/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:58:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/05 18:57:03 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// TRAITER LES CHANGEMENTS D'ETAT
/*
	Lorsque "
	Lorsque '
	Lorsque SPACE
*/
void	ft_detect_state(char c, t_minishell *minishell)
{
	// SI GUILLEMETS
	if (c == '"')
	{
		ft_printf("Double Quote\n");
		if (minishell->state == NORMAL)
			minishell->state = IN_DQUOTE;
		else if (minishell->state == IN_DQUOTE)
			minishell->state = NORMAL;
	}
	// SI APOSTROPHE
	else if (c ==  '\'')
	{
		ft_printf("Simple Quote\n");
		if (minishell->state == NORMAL)
			minishell->state = IN_QUOTE;
		else if (minishell->state == IN_QUOTE)
			minishell->state = NORMAL;
	}
	// SI ESPACES
	else if (ft_ischarset(c, SPACES))
	{
		// ON CONVERTIT LE BUFFER EN STRUCT ET ON SET A NEUTRAL
		// if (minishell->state == NORMAL)
		ft_printf("Space\n");
		if (minishell->state == NORMAL)
			minishell->state = NEUTRAL;
		else if (minishell->state == NEUTRAL)
			minishell->state = NORMAL;
	}
}



// ON RECUPERE LA LIGNE
void	ft_create_elem_lst(t_minishell *minishell)
{
	int i;
	char *line;
	char *buffer;

	// On recupere la ligne
	line = minishell->line;
	i = 0;
	// On parcourt la ligne pour ajouter a chaque fois
	while (line[i])
	{
		// ON DETECTE L'ETAT POUR POUVOIR DETERMINER QUOI FAIRE DU CHARACTERE
		buffer = ft_calloc_gc(ft_strlen(line) + 1, sizeof(char), &minishell->gc);
		ft_detect_state(line[i], minishell);
		ft_printf("%d\n", minishell->state);
		i++;
		(void)buffer;
		// ON TRAITE line[i] EN FONCTION DE L'ETAT 
		// Si NEUTRAL ? 
		//  word ou operator, besoin de append au mot current 
		//  j'aurais fait une sub boucle  tant !NEUTRAL i++ pour avoir le truc,
		//  malloc ensuite 
	}

}

// On récupere la ligne, on traite pour avoir des types de mots 
// On les récupere ensuite pour créer des phrases

void	ft_parse(t_minishell *minishell)
{
	// ON RECUPERE LES TYPES DANS UN PREMIER TEMPS
	ft_create_elem_lst(minishell);
	
	// UTILISATION DU PARSER
	//

}
