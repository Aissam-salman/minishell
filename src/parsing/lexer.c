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
#include <stdio.h>

t_elements *create_mocks_element();
void ft_create_cmd_lst(t_minishell *minishell);

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
		// if (!(minishell->state == IN_DQUOTE || minishell->state == IN_QUOTE))
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
	// ft_create_elem_lst(minishell);
	
	ft_create_cmd_lst(minishell);
}

void ft_create_cmd_lst(t_minishell *minishell)
{
	(void)minishell;
	t_elements *head = NULL;

	head = create_mocks_element();

	while (head)
	{
		printf("%s\n", head->str);
		head = head->next;
	}
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

t_elements *create_mocks_element()
{
	t_elements *head = NULL;
	t_elements element2;
	t_elements element3;
	t_elements element4;
	t_elements element5;
	t_elements element6;
	// PIPE,
	// IN_CHEVRON,
	// IN_DCHEVRON,
	// OUT_CHEVRON,
	// OUT_DCHEVRON,
	// WORD,
	// NBR_TYPES,
	// FILE ?? 
	head->str = ft_strdup("<");
	head->type = IN_CHEVRON;
	element2.str = ft_strdup("logs.txt");
	element2.type = WORD;
	element3.str = ft_strdup("cat");
	element3.type = WORD;
	element4.str = ft_strdup("|");
	element4.type = PIPE;
	element5.str = ft_strdup("grep");
	element5.type = WORD;
	element6.str = ft_strdup("\"error\"");
	element6.type = WORD;

	head->next = &element2;
	head->next->next = &element3;
	head->next->next->next = &element4;
	head->next->next->next->next = &element5;
	head->next->next->next->next->next = NULL;
	(void)element6;
	return (head);
}
