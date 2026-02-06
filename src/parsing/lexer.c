/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:58:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/06 13:35:55 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

// TRAITER LES CHANGEMENTS D'ETAT
/*
	Lorsque "
	Lorsque '
	Lorsque SPACE
*/
void	ft_state_detect(char c, char *buffer, t_minishell *minishell)
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
}

// int ft_interpret_state(char line[i], t_minishell *minishell);
// 	check le state et le char .
// 	if operateur et NORMAL 
// 		fin de string return 2 
// 	else if space || operateur et normal 
// 		fin de buffer return 2 
// 	else 
// 		append au buffer return 1

void	ft_state_interprete(char c, char *buffer, t_minishell *minishell)
{
	if (c == '|')
	{
		if (minishell->state == NORMAL)
		else
	}
	else if (c == '<' || c == '>')
	{
		if (minishell->state == NORMAL)
		else
	}
	else if (ft_ischarset(c, SPACES))
	{
		if (minishell->state == NORMAL)
		else

	}
	else

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
	buffer = ft_calloc_gc(ft_strlen(line) + 1, sizeof(char), &minishell->gc);
	while (line[i])
	{
		// ON DETECTE L'ETAT POUR POUVOIR DETERMINER QUOI FAIRE DU CHARACTERE
		ft_state_detect(line[i], minishell);
		ft_state_interprete(line[i], buffer, minishell);
		// ft_interprete_state
		ft_printf("%d\n", minishell->state);
		i++;
		(void)buffer;
		// ON TRAITE line[i] EN FONCTION DE L'ETAT 
		// 
		// int s = ft_interpret_state
		//
		//if (s == 1)
		// append(buffer, line[i])
		//else if  (s == 2)
		// lst_add(&t_element, lst_new(buffer)); 
		//
		//bzero(buffer);
		//append(buffer, line[i])
		//
		// void ft_detect_type(char *buffer, t_minishell *minishell) -> t_element ele->type
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
