/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:58:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/07 15:23:36 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// TRAITER LES CHANGEMENTS D'ETAT
/*
	Lorsque "
	Lorsque '
	Lorsque SPACE
*/

// A SECURISER VIA LA TAILLE MAX
void	ft_buffer_add(char *buffer, char c)
{
	int i;

	i = 0;
	while (buffer[i])
		i++;
	buffer[i] = c;
	buffer[i + 1] = '\0';
}

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
	else if (c ==  '\'')
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

void	ft_state_interpret(char *line, int *index, char *buffer, t_minishell *minishell)
{
	if (ft_ischarset(line[*index], SPACES) && minishell->state == NORMAL)
	{
			// SIGNALE LA FIN DU BUFFER
			// INITIALISATION NOUVEAU NOEUD AVEC CONTENT BUFFER
			// ft_printf("Buffer end\n");

			// CREEER ET AJOUTER BUFFER A LA LIST T_ELEMENT
			// ft_token_create(buffer, minishell);

			// REMETTRE A ZERO BUFFER

			// SI PIPE
				// REGARDER SI line[index + 1] == line[index]
					// SI OUI -> RETURN SYNTAX ERROR : minishell: syntax error near unexpected token '||'
					// ERROR ls >>> wc zsh: parse error near `>'
		if (ft_strlen(buffer) > 0)
		{
			ft_print_state(line[*index],buffer, minishell);
			ft_token_add(minishell, ft_token_create(minishell, buffer));
			ft_bzero(buffer, ft_strlen(buffer));
		}
		minishell->state = WAITING;
		return;
	}
	// IF IT IS AN OPERATOR
	if (ft_ischarset(line[*index], OPERATORS) && minishell->state == NORMAL)
	{
		if (ft_strlen(buffer) > 0)
		{
			ft_print_state(line[*index],buffer, minishell);
			ft_bzero(buffer, ft_strlen(buffer));
		}
		if (line[*index] == '<' || line[*index] == '>')
		{
			if (line[*index + 1] == line[*index])
				ft_printf("DOUBLE CHEVRON\n");
			else
				ft_printf("SIMPLE CHEVRON OU PIPE \n");

		}
		return;
	}
	if (minishell->state != WAITING)
		ft_buffer_add(buffer, line[*index]);
}

void	ft_create_elem_lst(t_minishell *minishell)
{
	char *line;
	char *buffer;
	int		line_len;
	int i;

	// On recupere la ligne
	line = minishell->line;
	line_len = ft_strlen(line);
	i = 0;
	// On parcourt la ligne pour ajouter a chaque fois
	minishell->state = NORMAL;
	buffer = ft_calloc_gc(ft_strlen(line) + 1, sizeof(char), &minishell->gc);
	while (i < line_len)
	{
		// ON DETECTE L'ETAT POUR POUVOIR DETERMINER QUOI FAIRE DU CHARACTERE
		ft_state_detect(line[i], minishell);

		// ON TRAITE line[i] EN FONCTION DE L'ETAT 
		// ON INTERPRETE L'ETAT POUR CREER LE
		ft_state_interpret(line, &i, buffer, minishell);
		i++;

		// void ft_detect_type(char *buffer, t_minishell *minishell) -> t_element ele->type
	}
	if (ft_strlen(buffer) > 0)
		ft_token_add(minishell, ft_token_create(minishell, buffer));
	ft_print_state(0, buffer, minishell);
}


// On récupere la ligne, on traite pour avoir des types de mots 
// On les récupere ensuite pour créer des phrases

void	ft_parse(t_minishell *minishell)

{
	// ON RECUPERE LES TYPES DANS UN PREMIER TEMPS
	ft_create_elem_lst(minishell);
	ft_print_tokens(minishell->head_token);
	
	// UTILISATION DU PARSER

	//
	//
	//loop sur t_element
	//  int ft_check_redirection(minishell, element) < > << >>
	//        error 
	//  int ft_check_cmd(minishell, element)
	//        error 
	//  int ft_check_expends()
	//  	eroor 
	//  int ft_check_pipe()
	//  		
}
